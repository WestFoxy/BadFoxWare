#pragma once

#include "IAppSystem.hpp"
#include "IConVar.hpp"

class ConCommandBase;
class ConCommand;
class ConVar;

typedef int CVarDLLIdentifier_t;

class IConsoleDisplayFunc
{
public:
    virtual void ColorPrint(const uint8_t* clr, const char *pMessage) = 0;
    virtual void Print(const char *pMessage) = 0;
    virtual void DPrint(const char *pMessage) = 0;
};

/*class ICvar : public IAppSystem
{
public:
    virtual CVarDLLIdentifier_t        AllocateDLLIdentifier() = 0; // 9
    virtual void                       RegisterConCommand(ConCommandBase *pCommandBase) = 0; //10
    virtual void                       UnregisterConCommand(ConCommandBase *pCommandBase) = 0;
    virtual void                       UnregisterConCommands(CVarDLLIdentifier_t id) = 0;
    virtual const char*                GetCommandLineValue(const char *pVariableName) = 0;
    virtual ConCommandBase*            FindCommandBase(const char *name) = 0;
    virtual const ConCommandBase*      FindCommandBase(const char *name) const = 0;
    virtual ConVar*                    FindVar(const char *var_name) = 0; //16
    virtual const ConVar*              FindVar(const char *var_name) const = 0;
    virtual ConCommand*                FindCommand(const char *name) = 0;
    virtual const ConCommand*          FindCommand(const char *name) const = 0;
    virtual void                       InstallGlobalChangeCallback(FnChangeCallback_t callback) = 0;
    virtual void                       RemoveGlobalChangeCallback(FnChangeCallback_t callback) = 0;
    virtual void                       CallGlobalChangeCallbacks(ConVar *var, const char *pOldString, float flOldValue) = 0;
    virtual void                       InstallConsoleDisplayFunc(IConsoleDisplayFunc* pDisplayFunc) = 0;
    virtual void                       RemoveConsoleDisplayFunc(IConsoleDisplayFunc* pDisplayFunc) = 0;
    virtual void                       ConsoleColorPrintf(const Color &clr, const char *pFormat, ...) const = 0;
    virtual void                       ConsolePrintf(const char *pFormat, ...) const = 0;
    virtual void                       ConsoleDPrintf(const char *pFormat, ...) const = 0;
    virtual void                       RevertFlaggedConVars(int nFlag) = 0;
};*/
#define PRINTF_FORMAT_STRING _Printf_format_string_
class ICvar 
{
public:
	virtual void Func0();
	virtual void Func1();
	virtual void Func2();
	virtual void Func3();
	virtual void Func4();
	virtual void Func5();
	virtual void Func6();
	virtual void Func7();
	virtual void Func8();
	virtual void Func9();
	virtual void RegisterConCommand(ConVar *pCommandBase) = 0;
	virtual void UnregisterConCommand(ConVar *pCommandBase) = 0;
	virtual void Func12();
	virtual void Func13();
	virtual void Func14();
	virtual void Func15();
	virtual ConVar* FindVar(const char* getVar);
	virtual void Func17();
	virtual void Func18();
	virtual void Func19();
	virtual void Func20();
	virtual void Func21();
	virtual void Func22();
	virtual void Func23();
	virtual void Func24();
	virtual void ConsoleColorPrintf(const Color& clr, const char *format, ...);
	virtual void ConsolePrintf(const char *format, ...);
	virtual void ConsoleDPrintf(PRINTF_FORMAT_STRING const char *pFormat, ...);
	virtual void Func28();
	virtual void Func29();
	virtual void Func30();
	virtual void Func31();
	virtual void Func32();
	virtual void Func33();
	virtual void Func34();
	virtual void Func35();
	virtual void Func36();
	virtual void Func37();
};
