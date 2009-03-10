//#include "D2BS.h"
#include "Script.h"

#include "debugnew/debug_new.h"

VOID ChatEvent(CHAR* lpszNick, CHAR* lpszMsg)
{
	ScriptList scripts = Script::GetScripts();
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!(*it)->IsRunning())
			continue;
		JSContext* cx = (*it)->GetContext();
		if(JS_GetContextThread(cx))
			JS_ClearContextThread(cx);
		JS_SetContextThread(cx);
		AutoRoot** argv = new AutoRoot*[2];
		argv[0] = new AutoRoot(cx, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, lpszNick)));
		argv[1] = new AutoRoot(cx, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, lpszMsg)));
		(*it)->ExecEventAsync("chatmsg", 2, argv);
		if(JS_GetContextThread(cx))
			JS_ClearContextThread(cx);
	}
}

VOID BNCSChatEvent(CHAR* lpszNick, CHAR* lpszMsg)
{
	ScriptList scripts = Script::GetScripts();
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!(*it)->IsRunning())
			continue;
		JSContext* cx = (*it)->GetContext();
		if(JS_GetContextThread(cx))
			JS_ClearContextThread(cx);
		JS_SetContextThread(cx);
		AutoRoot** argv = new AutoRoot*[2];
		argv[0] = new AutoRoot(cx, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, lpszNick)));
		argv[1] = new AutoRoot(cx, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, lpszMsg)));
		(*it)->ExecEventAsync("gamemsg", 2, argv);
		if(JS_GetContextThread(cx))
			JS_ClearContextThread(cx);
	}
}

VOID LifeEvent(DWORD dwLife, DWORD dwMana)
{
	ScriptList scripts = Script::GetScripts();
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!(*it)->IsRunning())
			continue;
		JSContext* cx = (*it)->GetContext();
		if(JS_GetContextThread(cx))
			JS_ClearContextThread(cx);
		JS_SetContextThread(cx);
		AutoRoot** argv = new AutoRoot*[2];
		argv[0] = new AutoRoot(cx, INT_TO_JSVAL(dwLife));
		argv[1] = new AutoRoot(cx, INT_TO_JSVAL(dwMana));
		(*it)->ExecEventAsync("melife", 2, argv);
		if(JS_GetContextThread(cx))
			JS_ClearContextThread(cx);
	}
}

VOID CopyDataEvent(DWORD dwMode, CHAR* lpszMsg)
{
	ScriptList scripts = Script::GetScripts();
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!(*it)->IsRunning())
			continue;
		JSContext* cx = (*it)->GetContext();
		if(JS_GetContextThread(cx))
			JS_ClearContextThread(cx);
		JS_SetContextThread(cx);
		AutoRoot** argv = new AutoRoot*[2];
		argv[0] = new AutoRoot(cx, INT_TO_JSVAL(dwMode));
		argv[1] = new AutoRoot(cx, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, lpszMsg)));
		(*it)->ExecEventAsync("copydata", 2, argv);
		if(JS_GetContextThread(cx))
			JS_ClearContextThread(cx);
	}
}

VOID ChatCmdEvent(CHAR* lpszMsg)
{
	ScriptList scripts = Script::GetScripts();
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!(*it)->IsRunning())
			continue;
		JSContext* cx = (*it)->GetContext();
		if(JS_GetContextThread(cx))
			JS_ClearContextThread(cx);
		JS_SetContextThread(cx);
		AutoRoot** argv = new AutoRoot*[1];
		argv[0] = new AutoRoot(cx, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, lpszMsg)));
		(*it)->ExecEventAsync("chatcmd", 1, argv);
		if(JS_GetContextThread(cx))
			JS_ClearContextThread(cx);
	}
}

VOID KeyDownUpEvent(WPARAM key, BYTE bUp)
{
	ScriptList scripts = Script::GetScripts();
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!(*it)->IsRunning())
			continue;
		JSContext* cx = (*it)->GetContext();
		if(JS_GetContextThread(cx))
			JS_ClearContextThread(cx);
		JS_SetContextThread(cx);
		AutoRoot** argv = new AutoRoot*[1];
		argv[0] = new AutoRoot(cx, INT_TO_JSVAL(key));
		(*it)->ExecEventAsync((bUp ? "keyup" : "keydown"), 1, argv);
		if(JS_GetContextThread(cx))
			JS_ClearContextThread(cx);
	}
}

VOID PlayerAssignEvent(DWORD dwUnitId)
{
	ScriptList scripts = Script::GetScripts();
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!(*it)->IsRunning())
			continue;
		JSContext* cx = (*it)->GetContext();
		if(JS_GetContextThread(cx))
			JS_ClearContextThread(cx);
		JS_SetContextThread(cx);
		AutoRoot** argv = new AutoRoot*[1];
		argv[0] = new AutoRoot(cx, INT_TO_JSVAL(dwUnitId));
		(*it)->ExecEventAsync("playerassign", 1, argv);
		if(JS_GetContextThread(cx))
			JS_ClearContextThread(cx);
	}
}

VOID MouseClickEvent(int button, POINT pt, bool bUp)
{
	ScriptList scripts = Script::GetScripts();
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!(*it)->IsRunning())
			continue;
		JSContext* cx = (*it)->GetContext();
		if(JS_GetContextThread(cx))
			JS_ClearContextThread(cx);
		JS_SetContextThread(cx);
		AutoRoot** argv = new AutoRoot*[3];
		argv[0] = new AutoRoot(cx, INT_TO_JSVAL(button));
		argv[1] = new AutoRoot(cx, INT_TO_JSVAL(pt.x));
		argv[2] = new AutoRoot(cx, INT_TO_JSVAL(pt.y));
		(*it)->ExecEventAsync((bUp ? "mouseup" : "mousedown"), 3, argv);
		if(JS_GetContextThread(cx))
			JS_ClearContextThread(cx);
	}
}

VOID MouseMoveEvent(POINT pt)
{
/*	ScriptList scripts = Script::GetScripts();
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!(*it)->IsRunning())
			continue;
		JSContext* cx = (*it)->GetContext();
		JS_ClearContextThread(cx);
		JS_SetContextThread(cx);
		AutoRoot** argv = new AutoRoot*[2];
		argv[0] = new AutoRoot(cx, INT_TO_JSVAL(pt.x));
		argv[1] = new AutoRoot(cx, INT_TO_JSVAL(pt.y));
		(*it)->ExecEventAsync("mousemove", 2, argv);
		JS_ClearContextThread(cx);
	}*/
}

VOID ScriptBroadcastEvent(uintN argc, jsval* args)
{
	ScriptList scripts = Script::GetScripts();
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!(*it)->IsRunning())
			continue;
		JSContext* cx = (*it)->GetContext();
		if(JS_GetContextThread(cx))
			JS_ClearContextThread(cx);
		JS_SetContextThread(cx);
		AutoRoot** argv = new AutoRoot*[argc];
		for(uintN i = 0; i < argc; i++)
			argv[i] = new AutoRoot(cx, args[i]);
		(*it)->ExecEventAsync("scriptmsg", argc, argv);
		if(JS_GetContextThread(cx))
			JS_ClearContextThread(cx);
	}
}
