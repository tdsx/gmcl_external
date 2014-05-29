#include "Interface.h"
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <string>
#include <cerrno>
#include "cryptlib.h" 
#include "modes.h"
#include "filters.h"
#include "base64.h"

using namespace std;
using namespace GarrysMod;
using namespace CryptoPP;

typedef int(*loadstring)(lua_State* state, const char* code);
typedef int(*loadfile)(lua_State* state, const char* fileName);
typedef void(*msg)(char const* pMsg, ...);
typedef void(*warning)(char const* pMsg, ...);
typedef int(*loadbuffer)(lua_State *state, const char *buff, size_t sz, const char* name);
int luaL_dofile(lua_State* state, const char* fileName);
int luaL_dostring(lua_State* state, std::string code, const char* name);

HMODULE lua_shared = GetModuleHandle("lua_shared.dll");
HMODULE tier0 = GetModuleHandle("tier0.dll");

loadstring luaL_loadstring = (loadstring)GetProcAddress(lua_shared, "luaL_loadstring");
loadfile luaL_loadfile = (loadfile)GetProcAddress(lua_shared, "luaL_loadfile");
loadbuffer luaL_loadbuffer = (loadbuffer)GetProcAddress(lua_shared, "luaL_loadbuffer");
msg Msg = (msg)GetProcAddress(tier0, "Msg");
warning Warning = (warning)GetProcAddress(tier0, "Warning");

std::string get_file_contents(const char *filename)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	
	if (in)
	{
		return(std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()));
	}

	throw(errno);
}

int luaL_dostring(lua_State* state, std::string code, const char* name)
{
	luaL_loadbuffer(state, std::string(code).c_str(), strlen(std::string(code).c_str()), name);

	if (LUA->IsType(-1, Type::FUNCTION))
	{
		LUA->Call(0, 0);

		return 0;
	}

	return 1;
}

int luaL_dofile(lua_State* state, const char* fileName)
{
	luaL_loadfile(state, fileName);

	if (LUA->IsType(-1, Type::FUNCTION))
	{
		LUA->Call(0, 0);

		return 0;
	}

	return 1;
}