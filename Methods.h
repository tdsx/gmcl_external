#include "Interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace GarrysMod::Lua;

namespace Methods
{
	// A function to get the global table.
	static int GetGlobal(lua_State* state)
	{
		LUA->PushSpecial(SPECIAL_GLOB);
		int Global = LUA->ReferenceCreate();

		return Global;
	}

	// A function to get a field from the global table.
	static int GetGlobalField(lua_State* state, const char* Name)
	{
		LUA->PushSpecial(SPECIAL_GLOB);
		LUA->GetField(-1, Name);
		int Obj = LUA->ReferenceCreate();
		LUA->ReferencePush(Obj);

		return Obj;
	}

	static int print(lua_State* state, const char* message)
	{
		GetGlobalField(state, "print");
		LUA->PushString(message);
		LUA->Call(1, 0);
		LUA->Pop();

		return 0;
	}

	namespace table
	{
		static int Create(lua_State* state)
		{
			LUA->CreateTable();
			int TABLE = LUA->ReferenceCreate();

			return TABLE;
		}

		static int PushTable(lua_State* state, int TargetTable, int Table, const char* Name)
		{
			LUA->ReferencePush(TargetTable);
			LUA->ReferencePush(Table);
			LUA->SetField(-2, Name);

			return 0;
		}

		static int PushCFunc(lua_State* state, int Table, const char* Name, CFunc Function)
		{
			LUA->PushSpecial(SPECIAL_GLOB);
			LUA->ReferencePush(Table);
			LUA->PushCFunction(Function);  LUA->SetField(-2, Name);

			return 0;
		}
	};
};