#include <windows.h>
#include "Interface.h"
#include "Methods.h"
#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <conio.h>
#include <vector>
#include <numeric>
#include "auxiliary.h"
#include "cryptlib.h" 
#include "modes.h"
#include "filters.h"
#include "base64.h"

using namespace GarrysMod::Lua;
using namespace Methods;
using namespace std;
using namespace CryptoPP;

// Uninteresting stuff.
inline bool FILE_EXISTS(const std::string& name) 
{
	struct stat buffer;

	return (stat(name.c_str(), &buffer) == 0);
}

void ReplaceStringInPlace(std::string& subject, const std::string& search,
	const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

bool IsDirectory(std::string path)
{
	if (path.find(".") != std::string::npos)
	{
		return false;
	}

	return true;
}

int SearchDirectory(vector<string>& refvecFiles,
	const string & fefcstrRootDirectory,
	const string & refcstrExtension,
	bool bSearchSubdirectories)
{
	string	 strFilePath;			 // Filepath
	string	 strPattern;			  // Pattern
	string	 strExtension;			// Extension
	HANDLE		  hFile;				   // Handle to file
	WIN32_FIND_DATA FileInformation;		 // File information

	strPattern = fefcstrRootDirectory + "\\*.*";

	hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (FileInformation.cFileName[0] != '.')
			{
				strFilePath.erase();
				strFilePath = fefcstrRootDirectory + "\\" + FileInformation.cFileName;

				if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					//True if you want to search subdirectories
					if (bSearchSubdirectories)
					{

						// Search subdirectory
						int iRC = SearchDirectory(refvecFiles,
							strFilePath,
							refcstrExtension,
							bSearchSubdirectories);
						if (iRC)
							return iRC;

					}
				}
				else
				{
					// Check extension
					strExtension = FileInformation.cFileName;
					strExtension = strExtension.substr(strExtension.rfind(".") + 1);

					if (strExtension == refcstrExtension)
					{
						// Save filename



						// c_filenames.AddString(FileInformation.cFileName);

						refvecFiles.push_back(strFilePath);
					}
				}
			}
		} while (::FindNextFile(hFile, &FileInformation) == TRUE);

		// Close handle
		::FindClose(hFile);

		DWORD dwError = ::GetLastError();
		if (dwError != ERROR_NO_MORE_FILES)
			return dwError;
	}


	return 0;
}

void toClipboard(const std::string &s)
{
	OpenClipboard(0);
	EmptyClipboard();
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size());
	if (!hg){
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg), s.c_str(), s.size());
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
	GlobalFree(hg);
}

// Interesting stuff.
int runEncrypted(lua_State* state)
{
	LUA->CheckString(1);

	if (luaL_dostring(state, Decrypt(LUA->GetString(1)).c_str(), "external"))
	{
		LUA->PushBool(false);
	} else {
		LUA->PushBool(true);
	}

	return 1;
}

int EncryptLuaFunc(lua_State* state)
{
	LUA->CheckString(1);
	
	std::string encrypted = Encrypt(LUA->GetString(1));
	toClipboard(encrypted);

	LUA->PushString(encrypted.c_str());

	return 1;
}

int includeExternal(lua_State* state)
{
	LUA->CheckString(1);

	if (!IsDirectory(LUA->GetString(1)))
	{
		std::string targetFile = LUA->GetString(1);

		if (FILE_EXISTS(targetFile.c_str()))
		{
			ReplaceStringInPlace(targetFile, "\\", "/");
			
			if (luaL_dofile(state, targetFile.c_str()))
			{
				Warning("[ERROR] %s\n", LUA->GetString(-1));
			}
		} else {
			Warning("[ERROR] %s does not exist!\n", targetFile);
		}
	} else {
		std::vector<std::string> vecFiles;
		bool recursive = LUA->GetBool(2);

		SearchDirectory(vecFiles, LUA->GetString(1), "lua", recursive);

		for (std::vector<std::string>::iterator ITERATOR = vecFiles.begin(); ITERATOR != vecFiles.end(); ++ITERATOR)
		{
			std::string iterated = *ITERATOR;

			ReplaceStringInPlace(iterated, "\\", "/");
			
			if (luaL_dofile(state, iterated.c_str()))
			{
				Warning("[ERROR] %s\n", LUA->GetString(-1));
			}
		}
	}

	return 0;
}

int encryptFile(lua_State* state)
{
	LUA->CheckString(1);

	if (!IsDirectory(LUA->GetString(1)))
	{
		std::string targetFile = LUA->GetString(1);

		if (FILE_EXISTS(targetFile.c_str()))
		{
			fstream writer(targetFile.c_str());
			std::string encrypted = Encrypt(get_file_contents(targetFile.c_str()));
			encrypted.erase(std::remove(encrypted.begin(), encrypted.end(), '\n'), encrypted.end());
			std::string towrite = "runEncrypted(\"" + std::string(encrypted) + "\")";
			writer << towrite;
			writer.close();
		}
		else {
			Warning("[ERROR] %s does not exist!", targetFile);
		}
	}
	else {
		std::vector<std::string> vecFiles;
		bool recursive = LUA->GetBool(2);

		SearchDirectory(vecFiles, LUA->GetString(1), "lua", recursive);

		for (std::vector<std::string>::iterator ITERATOR = vecFiles.begin(); ITERATOR != vecFiles.end(); ++ITERATOR)
		{
			std::string iterated = *ITERATOR;

			fstream writer(iterated.c_str());
			std::string encrypted = Encrypt(get_file_contents(iterated.c_str()));
			encrypted.erase(std::remove(encrypted.begin(), encrypted.end(), '\n'), encrypted.end());
			std::string towrite = "runEncrypted(\"" + std::string(encrypted) + "\")";
			writer << towrite;
			writer.close();
		}
	}

	return 0;
}

GMOD_MODULE_OPEN()
{
	int GlobalTable = GetGlobal(state);

	table::PushCFunc(state, GlobalTable, "includeExternal", includeExternal);
	table::PushCFunc(state, GlobalTable, "runEncrypted", runEncrypted);
	table::PushCFunc(state, GlobalTable, "encrypt", EncryptLuaFunc);
	table::PushCFunc(state, GlobalTable, "encryptFile", encryptFile);

	LUA->ReferenceFree(GlobalTable);

	print(state, "External module loaded.");
	
	return 0;
}

GMOD_MODULE_CLOSE()
{
	return 0;
}