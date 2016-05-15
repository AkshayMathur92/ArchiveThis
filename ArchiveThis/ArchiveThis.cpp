// ArchiveThis.cpp : Defines the entry point for the console application.
#include <string>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::tr2::sys;

//Get destination from registry HKEY_CURRENT_USER\\SOFTWARE\\ARCHIVETHIS\\DESTINATION
int getDestination(char* destination, DWORD keyNameSizeBuf) {

	HKEY hKeyRoot = HKEY_CURRENT_USER, hKey;
	LPTSTR lpSubKey = L"SOFTWARE\\ArchiveThis";
	LONG lResult;

	// If registry entry is present then pick destination value from there 
	lResult = RegOpenKeyEx(hKeyRoot, lpSubKey, 0, KEY_READ, &hKey);

	if (lResult == ERROR_SUCCESS) {
		RegQueryValueExA(hKey, "Destination", NULL, NULL, (LPBYTE)destination, &keyNameSizeBuf);
		RegCloseKey(hKey);
	}
	else {
		RegCloseKey(hKeyRoot);
		std::cout << "Error reading registry";
		return 1;
	}
	RegCloseKey(hKeyRoot);
	return ERROR_SUCCESS;
}
int fileCopy(fs::path sourcePath, fs::path destinationPath) {
	fs::directory_iterator sourceDir(sourcePath);
	for (fs::directory_iterator _end; sourceDir != _end; sourceDir++) {
		if (fs::is_directory(sourceDir->path())) {
			fs::create_directory(destinationPath / sourceDir->path().filename());
			fileCopy(sourceDir->path() , destinationPath / sourceDir->path().filename());
		}
		else {
			fs::copy_file(sourceDir->path(), destinationPath / sourceDir->path().filename(),fs::copy_options::update_existing);
		}
	}
	return 0;
}
int fileCopy(std::string source, std::string destination) {
	fs::path sourcePath(source);
	fs::path destinationPath(destination);
	if (fs::is_directory(sourcePath)) {
		destinationPath = destinationPath / sourcePath.filename();
		fs::create_directory(destinationPath);
		return fileCopy(sourcePath, destinationPath);
	}
	else {
		return fs::copy_file(sourcePath, destinationPath/sourcePath.filename());
	}
}
int main(int argc, char* argv[])
{
	if (argc > 1) {
		std::string source = argv[1];
		char destination[512];
		if (getDestination(destination, 512) == ERROR_SUCCESS)
			return fileCopy(source, destination);
		else
			return fileCopy(source, "C:\\Share");
	}
	else
		return 1;
}