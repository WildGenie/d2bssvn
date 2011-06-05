#define EXPORTED
#include "Engine.hpp"

#include <io.h>
#include <direct.h>
#include <iostream>

JSAPI_FUNC(console_print);
JSAPI_FUNC(console_newline);

JSAPI_EMPTY_CTOR(Console)

static JSClass console = { "Console", JSCLASS_IS_GLOBAL, JSCLASS_DEFAULT_WITH_CTOR(Console) };
JSFunctionSpec console_methods[] = {
	JS_FS("print", console_print, 1, JSPROP_STATIC),
	JS_FS_END
};

static JSClassSpec cmd_classes[] = {
	JS_CS(&console, nullptr, nullptr, nullptr, nullptr, nullptr),
	JS_CS_END
};

static JSModuleSpec console_mods[] = {
	JS_MS("console", cmd_classes, console_methods, nullptr),
	JS_MS_END
};

JSAPI_FUNC(console_print)
{
	JSString* str = nullptr;
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &str))
		return JS_FALSE;

	JS::Anchor<JSString*> anchor(str);
	const jschar* output = JS_GetStringCharsZ(cx, str);
	std::wcout << output << std::endl;
	return JS_TRUE;
}

void InitClasses(JSContext* cx, JSObject* obj)
{
	// no global classes to init
}

void reporter(JSContext *cx, const char *message, JSErrorReport *report)
{
	std::cout << "[";
	if(JSREPORT_IS_STRICT(report->flags))
		std::cout << "Strict ";
	if(JSREPORT_IS_WARNING(report->flags))
		std::cout << "Warning";
	else
		std::cout << "Error";

	std::cout << "] ";
	if(report->filename != nullptr)
		std::cout << "At " << report->filename << ":" << report->lineno;
	std::cout << " (" << report->errorNumber << ") ";
	std::cout << message << std::endl;
}

int main(int argc, const char** argv)
{
	wchar_t path[MAX_PATH] = L"", script[MAX_PATH] = L"";
	_wgetcwd(path, MAX_PATH);
	swprintf_s(script, MAX_PATH, L"%s\\%s", path, L"test.js");

	if(_waccess(script, 0) == -1)
		return 0;

	Core::Engine engine(path, 0x80000, InitClasses, reporter);
	engine.RegisterModule(console_mods);
	std::cout << "Starting test.js" << std::endl;
	engine.CompileScript(L"test.js")->Start();
	getchar();

	return 0;
}