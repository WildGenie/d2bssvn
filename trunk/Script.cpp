#include <io.h>
#include <algorithm>
#include <process.h>

#include "Script.h"
#include "Core.h"
#include "Constants.h"
#include "D2Ptrs.h"
#include "JSUnit.h"
#include "Helpers.h"
#include "ScriptEngine.h"
#include "D2BS.h"

using namespace std;

Script::Script(const char* file, ScriptType scriptType) :
			context(NULL), globalObject(NULL), scriptObject(NULL), script(NULL), execCount(0),
			isAborted(false), isPaused(false), isReallyPaused(false), scriptType(scriptType),
			threadHandle(INVALID_HANDLE_VALUE), threadId(0)
{
	InitializeCriticalSection(&lock);
	EnterCriticalSection(&lock);

	if(scriptType == Command)
	{
		fileName = string("Command Line");
	}
	else
	{
		if(!!_access(file, 0))
			throw std::exception("File not found");

		char* tmpName = _strdup(file);
		if(!tmpName)
			throw std::exception("Could not dup filename");

		_strlwr_s(tmpName, strlen(file)+1);

		fileName = string(tmpName);
		replace(fileName.begin(), fileName.end(), '/', '\\');
		free(tmpName);
	}

	try
	{
		context = JS_NewContext(ScriptEngine::GetRuntime(), 0x2000);
		if(!context)
			throw std::exception("Couldn't create the context");
	}
	catch(std::exception &)
	{
		LeaveCriticalSection(&lock);
		throw;
	}

	try
	{
		JS_SetContextPrivate(context, this);
		JS_BeginRequest(context);

		globalObject = JS_GetGlobalObject(context);
		jsval meVal = JSVAL_VOID;
		if(JS_GetProperty(context, globalObject, "me", &meVal) != JS_FALSE)
		{
			JSObject* meObject = JSVAL_TO_OBJECT(meVal);
			me = (myUnit*)JS_GetPrivate(context, meObject);
		}
		else
			throw std::exception("Couldn't get me object");

		if(scriptType == Command)
			script = JS_CompileScript(context, globalObject, fileName.c_str(), strlen(fileName.c_str()), "Command Line", 1);
		else
			script = JS_CompileFile(context, globalObject, fileName.c_str());

		if(!script)
			throw std::exception("Couldn't compile the script");
	}
	catch(std::exception &)
	{

		JS_EndRequest(context);
		JS_DestroyContext(context);

		LeaveCriticalSection(&lock);
		throw;
	}

	try
	{
		scriptObject = JS_NewScriptObject(context, script);
		if(!scriptObject)
			throw std::exception("Couldn't create the script object");

		if(JS_AddNamedRoot(context, &scriptObject, "script object") == JS_FALSE)
			throw std::exception("Couldn't add named root for scriptObject");

		JS_EndRequest(context);
		JS_ClearContextThread(context);

		LeaveCriticalSection(&lock);
	}
	catch(std::exception&)
	{
		if(scriptObject)
		{
			JS_RemoveRoot(&scriptObject);
		}
		else
			JS_DestroyScript(context, script);
 
		JS_EndRequest(context);
		JS_DestroyContext(context);

		LeaveCriticalSection(&lock);
		throw;
	}
}

Script::~Script(void)
{
	EnterCriticalSection(&lock);
	Stop(true, true);

	if(!JS_GetContextThread(context))
		JS_SetContextThread(context);

	JS_BeginRequest(context);

	// these calls can, and probably should, be moved to the context callback on cleanup
	JS_RemoveRoot(&globalObject);
	JS_RemoveRoot(&scriptObject);

	JS_EndRequest(context);
	JS_DestroyContext(context);

	context = NULL;
	scriptObject = NULL;
	globalObject = NULL;
	script = NULL;

	includes.clear();
	if(threadHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(threadHandle);
		threadHandle = INVALID_HANDLE_VALUE;
	}

	LeaveCriticalSection(&lock);
	DeleteCriticalSection(&lock);
}

int Script::GetExecutionCount(void)
{
	return execCount;
}

DWORD Script::GetThreadId(void)
{
	return (threadHandle == NULL ? -1 : threadId);
}

void Script::Run(void)
{
	// only let the script run if it's not already running
	if(IsRunning())
		return;

	isAborted = false;
	DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &threadHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);
	threadId = GetCurrentThreadId();

	jsval main = JSVAL_VOID, dummy = JSVAL_VOID;
	//JS_AddRoot(&main);

	JS_SetContextThread(GetContext());
	JS_BeginRequest(GetContext());

	if(JS_ExecuteScript(GetContext(), globalObject, script, &dummy) != JS_FALSE &&
	   JS_GetProperty(GetContext(), globalObject, "main", &main) != JS_FALSE &&
	   JSVAL_IS_FUNCTION(GetContext(), main))
	{
		JS_CallFunctionValue(GetContext(), globalObject, main, 0, NULL, &dummy);
	}

	if(GetScriptType() == Command)
	{
		// if we just processed a command, print the results of the command
		if(!JSVAL_IS_NULL(dummy) && !JSVAL_IS_VOID(dummy))
		{
			JS_ConvertValue(GetContext(), dummy, JSTYPE_STRING, &dummy);
			Print(JS_GetStringBytes(JS_ValueToString(GetContext(), dummy)));
		}
	}

	//JS_RemoveRoot(&main);
	JS_EndRequest(GetContext());
	JS_ClearContextThread(GetContext());

	execCount++;
	Stop();
}

void Script::UpdatePlayerGid(void)
{
	me->dwUnitId = (*p_D2CLIENT_PlayerUnit == NULL ? NULL : (*p_D2CLIENT_PlayerUnit)->dwUnitId);
}

void Script::Pause(void)
{
	//EnterCriticalSection(&lock);
	if(!IsAborted() && !IsPaused())
		isPaused = true;
	//LeaveCriticalSection(&lock);
}

void Script::Resume(void)
{
	//EnterCriticalSection(&lock);
	if(!IsAborted() && IsPaused())
		isPaused = false;
	//LeaveCriticalSection(&lock);
}

bool Script::IsPaused(void)
{
	return isPaused;
}

void Script::Stop(bool force, bool reallyForce)
{
	// if we've already stopped, just return
	if(isAborted)
		return;

	EnterCriticalSection(&lock);

	if(context && JS_IsRunning(context))
		JS_TriggerOperationCallback(context);

	// tell everyone else that the script is aborted FIRST
	isAborted = true;
	isPaused = false;
	isReallyPaused = false;

	ClearAllEvents();
	Genhook::Clean(this);

	// normal wait: 500ms, forced wait: 300ms, really forced wait: 100ms
	int maxCount = (force ? (reallyForce ? 10 : 30) : 50);
	for(int i = 0; IsRunning(); i++)
	{
		// if we pass the time frame, just ignore the wait because the thread will end forcefully anyway
		if(i >= maxCount)
			break;
		Sleep(10);
	}

	if(threadHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(threadHandle);
		threadHandle = INVALID_HANDLE_VALUE;
	}

	if(GetScriptType() != Command)
		Print("Script %s ended", fileName.c_str() + strlen(Vars.szScriptPath) + 1);

	LeaveCriticalSection(&lock);
}

bool Script::IsIncluded(const char* file)
{
	char* fname = _strdup(file);
	if(!fname)
		return false;

	_strlwr_s(fname, strlen(fname)+1);
	StringReplace(fname, '/', '\\', strlen(fname));

	uint count = 0;
	count = includes.count(string(fname));
	free(fname);

	return !!count;
}

bool Script::Include(const char* file)
{
	// since includes will happen on the same thread, locking here is acceptable
	EnterCriticalSection(&lock);
	char* fname = _strdup((char*)file);
	if(!fname)
		return false;
	_strlwr_s(fname, strlen(fname)+1);
	StringReplace(fname, '/', '\\', strlen(fname));
	// FIXME
	// ignore already included, 'in-progress' includes, and self-inclusion
	if(!!includes.count(string(fname)) ||
	    !!inProgress.count(string(fname)) ||
	    (fileName == string(fname)))
	{
		LeaveCriticalSection(&lock);
		free(fname);
		return true;
	}
	bool rval = false;

	JS_BeginRequest(GetContext());

	JSScript* script = JS_CompileFile(GetContext(), GetGlobalObject(), fname);
	if(script)
	{
		JSObject* scriptObj = JS_NewScriptObject(GetContext(), script);
		JS_AddRoot(&scriptObj);
		jsval dummy;
		inProgress[fname] = true;
		rval = !!JS_ExecuteScript(GetContext(), GetGlobalObject(), script, &dummy);
		if(rval)
			includes[fname] = true;
		inProgress.erase(fname);
		JS_RemoveRoot(&scriptObj);
	}

	JS_EndRequest(GetContext());
	LeaveCriticalSection(&lock);
	free(fname);
	return rval;
}

bool Script::IsRunning(void)
{
	return context && !(!JS_IsRunning(context) || IsPaused());
}

bool Script::IsAborted()
{
	return isAborted;
}

bool Script::IsListenerRegistered(const char* evtName)
{
	return strlen(evtName) > 0 && functions.count(evtName) > 0;
}

void Script::RegisterEvent(const char* evtName, jsval evtFunc)
{
	EnterCriticalSection(&lock);
	if(JSVAL_IS_FUNCTION(context, evtFunc) && strlen(evtName) > 0)
	{
		AutoRoot* root = new AutoRoot(evtFunc);
		functions[evtName].push_back(root);
	}
	LeaveCriticalSection(&lock);
}

bool Script::IsRegisteredEvent(const char* evtName, jsval evtFunc)
{
	// nothing can be registered under an empty name
	if(strlen(evtName) < 1)
		return false;

	// if there are no events registered under that name at all, then obviously there
	// can't be a specific one registered under that name
	if(functions.count(evtName) < 1)
		return false;

	for(FunctionList::iterator it = functions[evtName].begin(); it != functions[evtName].end(); it++)
		if((*it)->value() == evtFunc)
			return true;

	return false;
}

void Script::UnregisterEvent(const char* evtName, jsval evtFunc)
{
	if(strlen(evtName) < 1)
		return;

	EnterCriticalSection(&lock);
	AutoRoot* func = NULL;
	for(FunctionList::iterator it = functions[evtName].begin(); it != functions[evtName].end(); it++)
	{
		if((*it)->value() == evtFunc)
		{
			func = *it;
			break;
		}
	}
	functions[evtName].remove(func);
	func->Release();
	delete func;
	LeaveCriticalSection(&lock);
}

void Script::ClearEvent(const char* evtName)
{
	EnterCriticalSection(&lock);
	for(FunctionList::iterator it = functions[evtName].begin(); it != functions[evtName].end(); it++)
	{
		AutoRoot* func = *it;
		func->Release();
		delete func;
	}
	functions[evtName].clear();
	LeaveCriticalSection(&lock);
}

void Script::ClearAllEvents(void)
{
	EnterCriticalSection(&lock);
	for(FunctionMap::iterator it = functions.begin(); it != functions.end(); it++)
		ClearEvent(it->first.c_str());
	functions.clear();
	LeaveCriticalSection(&lock);
}

// TODO: clean this up and re-build it to match SpawnEvent... or maybe call SpawnEvent directly
#if 0
void Script::ExecEvent(const char* evtName, const char* format, uintN argc, void* argv)
{
	if((!IsAborted() && !IsPaused() && functions.count(evtName)) &&
		IsRunning() && !(GetScriptType() == InGame && ClientState() != ClientStateInGame))
	{
		FunctionList funcs = functions[evtName];
		JSContext* cx = JS_NewContext(ScriptEngine::GetRuntime(), 8192);
		JS_EnterLocalRootScope(cx);
		// build the arg list
		void* markp = NULL;
		jsval* argv = NULL;
		argv = JS_PushArguments(cx, &markp, format, argv);

		for(FunctionList::iterator it = funcs.begin(); it != funcs.end(); it++)
		{
			jsval dummy = JSVAL_VOID;
			JS_CallFunctionValue(cx, GetGlobalObject(), (*it)->value(), argc, argv, &dummy);
		}

		// and clean it up
		JS_PopArguments(cx, markp);
		JS_LeaveLocalRootScope(cx);
		JS_DestroyContextNoGC(cx);
	}
}
#endif

void Script::ExecEventAsync(const char* evtName, const char* format, ArgList* args)
{
	if(!IsAborted() && !IsPaused() && functions.count(evtName))
	{
		Event* evt = new Event;
		evt->owner = this;
		evt->object = globalObject;
		evt->functions = functions[evtName];
		strcpy_s(evt->format, 10, format);
		evt->args = args;

		SpawnEvent(evt);
	}
	else
	{
		// clean up args to prevent mem leak
		for(ArgList::iterator it = args->begin(); it != args->end(); it++)
		{
			switch(it->second)
			{
				case JSVal:
					JS_RemoveRoot((jsval*)&(it->first));
					break;
				case String:
					JS_RemoveRoot((JSString*)&(it->first));
					break;
			}
		}
		delete args;
	}
}

DWORD WINAPI ScriptThread(void* data)
{
	Script* script = (Script*)data;
	if(script)
	{
		script->Run();
		if(Vars.bDisableCache)
			ScriptEngine::DisposeScript(script);
	}
	return 0;
}

void SpawnEvent(Event* evt)
{
	if(!evt)
		return;

	if(evt->owner->IsRunning() && !(evt->owner->GetScriptType() == InGame && ClientState() != ClientStateInGame))
	{
		JSContext* cx = JS_NewContext(ScriptEngine::GetRuntime(), 8192);
		JS_SetContextPrivate(cx, evt->owner);
		JS_BeginRequest(cx);
		JS_EnterLocalRootScope(cx);
		// build the arg list
		uintN argc = evt->args->size();
		jsval* argv = new jsval[argc];
		int i = 0;
		for(ArgList::iterator it = evt->args->begin(); it != evt->args->end(); it++, i++)
		{
			switch(it->second)
			{
				case String:
					argv[i] = STRING_TO_JSVAL((JSString*)it->first);
					break;
				case SignedInt: case UnsignedInt: case Double: case UnsignedShort:
					JS_NewNumberValue(cx, (jsdouble)it->first, &argv[i]);
					break;
				case Boolean:
					argv[i] = BOOLEAN_TO_JSVAL((JSBool)it->first);
					break;
				case JSVal:
					argv[i] = (jsval)it->first;
					break;
			}
		}

		for(FunctionList::iterator it = evt->functions.begin(); it != evt->functions.end(); it++)
		{
			jsval dummy = JSVAL_VOID;
			JS_CallFunctionValue(cx, evt->object, (*it)->value(), argc, argv, &dummy);
		}

		// and clean it up
		JS_LeaveLocalRootScope(cx);
		JS_EndRequest(cx);
		JS_DestroyContextNoGC(cx);

		for(ArgList::iterator it = evt->args->begin(); it != evt->args->end(); it++)
		{
			switch(it->second)
			{
				case JSVal:
					JS_RemoveRoot((jsval*)&(it->first));
					break;
				case String:
					JS_RemoveRoot((JSString*)&(it->first));
					break;
			}
		}

		delete[] argv;
	}

	delete evt->args;
	delete evt;
}
