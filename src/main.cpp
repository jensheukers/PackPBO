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
#include <sstream>
#include <vector>
#include <fstream>
#include <sys/stat.h>
#include "logger.h"

std::string _versionString = "Version: 19/11/2018";
std::string _exeDir; /// @brief Directory of the executable

std::string _pboSrc; /// @brief PBO Source directory
std::string _pboDest; /// @brief PBO Destination directory
std::string _dzToolsDir; /// @brief Destination of the DayZ Tools

Logger* logger; /// @brief Pointer to Logger class
bool _extensiveLogging; /// @brief if true will log everything to the console 

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
		if (logger) {
			logger->Log("Cannot Access:");
			logger->Log(dir);
		}
		return false;
	}
	else if (info.st_mode & S_IFDIR) {
		return true;
	}
	else {
		if (logger) {
			logger->Log(dir);
			logger->Log("Is not a directory!");
		}
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
		while (std::getline(_settingsFile, line)) {
			std::stringstream ss(line);
			std::string segment;
			std::vector<std::string> segments;
			while (std::getline(ss, segment, '=')) {
				segments.push_back(segment);
			}

			if (segments[0] == "DayZToolsDir") {
				_dzToolsDir = segments[1];
			}

			if (segments[0] == "ExtensiveLogging") {
				if (segments[1] == "true") {
					_extensiveLogging = true;
				}
				else {
					_extensiveLogging = false;
				}
			}
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

		if (!DirExists(_dzToolsDir.c_str(), nullptr)) {
			return 1;
		}

		//Write to file
		std::ofstream _settingsFile(GetFilePath("settings.txt"), std::ofstream::out);

		//Dayz Tools Directory
		_settingsFile << "DayZToolsDir" << "=" << _dzToolsDir << "\n";

		// Extensive loggin
		std::cout << "Would you like to enable extensive logging? [Y/N] ";
		std::string state;

		std::getline(std::cin, state);

		if (state == "y" || state == "Y") {
			_settingsFile << "ExtensiveLogging=true" << "\n";
			_extensiveLogging = true;
		}
		else {
			_settingsFile << "ExtensiveLogging=false" << "\n";
			_extensiveLogging = false;
		}
	}

	logger = new Logger(GetFilePath("log.txt"), _extensiveLogging); // Create logger instance

	logger->Log("PackPBO is created by Jens Heukers, www.jensheukers.nl");
	logger->Log(_versionString);
	logger->Log("\n");

	if (!DirExists(_dzToolsDir.c_str(), logger)) {
		logger->Log("Cannot find DayZ tools directory");
		return 1;
	}

	// Get source file directory
	std::cout << "Please specify path to source files: ";
	std::getline(std::cin, _pboSrc); 
	
	logger->Log("SET: Source file dir:");
	logger->Log(_pboSrc);

	if (!DirExists(_pboSrc.c_str(), logger)) {
		return 1;
	}

	// Get destination directory
	std::cout << "Please specify destination: ";
	std::getline(std::cin, _pboDest);

	logger->Log("SET: Destination dir:");
	logger->Log(_pboDest);

	if (!DirExists(_pboDest.c_str(), logger)) {
		return 1;
	}

	return 0;
}