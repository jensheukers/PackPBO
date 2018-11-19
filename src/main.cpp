/**
*	Filename: main.cpp
*
*	Description: Main entry point source file for application
*	Version: 19/11/2018
*
*	© 2018, www.jensheukers.nl
*/
#include <iostream>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include "logger.h"

std::string _versionString = "Version: 19/11/2018";
std::string _exeDir; /// @brief Directory of the executable

std::string _pboSrc; /// @brief PBO Source directory
std::string _pboDest; /// @brief PBO Destination directory
std::string _dzToolsDir; /// @brief Destination of the DayZ Tools

Logger* logger; /// @brief Pointer to Logger class

/**
* Get the absolute path to any file inside build folder
*/
std::string GetFilePath(std::string file) {
	std::string convertedString = _exeDir;
	convertedString.append("\\");
	return convertedString.append(file);
};

/**
* Checks if a directory actually exists
*/
bool DirExists(const char* dir, Logger* logger) {
	struct stat info;
	if (stat(dir, &info) != 0) {
		logger->Log("Cannot Access:");
		logger->Log(dir);
		return false;
	}
	else if (info.st_mode & S_IFDIR) {
		return true;
	}
	else {
		logger->Log(dir);
		logger->Log("Is not a directory!");
		return false;
	}
	return false;
}

/**
* Read the settings file
*/
int ReadSettings() {
	// Attempt to read file
	std::string line;
	std::ifstream _settingsFile(GetFilePath("settings.txt"));

	if (_settingsFile.is_open())
	{
		while (_settingsFile.good())
		{
			std::getline(_settingsFile, line);
		}
		_settingsFile.close();
		return true;
	}

	return false;
}

/**
* Execute program
*/
int main(int argc, char* argv[]) {

	//Find the executable folder location
	std::string _exeDirArg = argv[0];
	std::size_t found = _exeDirArg.find_last_of("/\\");
	_exeDir = _exeDirArg.substr(0, found);

	//Read settings, and check for first time startup
	if (!ReadSettings()) {

		// Create settings file
		std::cout << "Please specify path to DayZ Tools: " << std::endl;
		std::getline(std::cin, _dzToolsDir);

		//Write to file
		std::ofstream _settingsFile(GetFilePath("settings.txt"), std::ofstream::out);

		_settingsFile << "DayZToolsDir" << "=" << _dzToolsDir << "\n";
	}

	logger = new Logger(GetFilePath("log.txt"), true); // Create logger instance

	logger->Log("PackPBO is created by Jens Heukers, www.jensheukers.nl");
	logger->Log(_versionString);
	logger->Log("\n");

	std::cout << "Please specify path to source files: ";
	std::getline(std::cin, _pboSrc); 
	
	logger->Log("SET: Source file dir:");
	logger->Log(_pboSrc);

	if (!DirExists(_pboSrc.c_str(), logger)) {
		return 1;
	}

	std::cout << "Please specify destination: ";
	std::getline(std::cin, _pboDest);

	logger->Log("SET: Destination dir:");
	logger->Log(_pboDest);

	if (!DirExists(_pboDest.c_str(), logger)) {
		return 1;
	}

	return 0;
}