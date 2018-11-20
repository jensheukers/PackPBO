/**
*	Filename: main.cpp
*
*	Description: Main entry point source file for application
*	Version: 20/11/2018
*
*	© 2018, www.jensheukers.nl
*/
#include <iostream>
#include <filesystem>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <sys/stat.h>
#include "logger.h"

namespace fs = std::experimental::filesystem;

std::string _versionString = "Version: 20/11/2018";
std::string _exeDir; /// @brief Directory of the executable

std::string _pboSrc; /// @brief PBO Source directory
std::string _pboDest; /// @brief PBO Destination directory
std::string _dzToolsDir; /// @brief Destination of the DayZ Tools
std::string _dzToolsImageToPaaDir; /// @brief Destination of the ImageToPaa executable

Logger* logger; /// @brief Pointer to Logger class
bool _extensiveLogging; /// @brief if true will log everything to the console 

std::vector<std::string> _CopyDirectTypes; /// @brief List of files to copy over directly

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

			if (segments[0] == "ListOfFilesToCopyDirectly") {
				std::stringstream ss(segments[1]);
				std::string segment;
				while (std::getline(ss, segment, ';')) {
					_CopyDirectTypes.push_back(segment);
				}
			}
		}
		_settingsFile.close();
		return true;
	}

	return false;
}

/**
* Prepare the PBO for binarize
*/
int PreparePBO() {

	//Create log folder in P:Drive if not exists
	if (!DirExists("P:/temp", logger)) { //Directory should not exist
		logger->Log("Creating temp folder in P: Drive...");
		fs::create_directories("P:/temp");
	}

	//Check if directory is created
	if (!DirExists("P:/temp", logger)) {
		logger->Log("Failed to create temp folder in P: Drive!"); // print error
		return 1; // return
	};	
	
	//Get project name
	std::stringstream ss(_pboSrc);
	std::vector<std::string> path;
	std::string segment;
	while (std::getline(ss, segment, '\\')) {
		path.push_back(segment);
	}

	std::string _pboTempSrc = "P:\\temp\\";
	_pboTempSrc.append(path[path.size() - 1]);

	if (DirExists(_pboTempSrc.c_str(), logger)) {
		fs::remove_all(_pboTempSrc);
	}

	fs::create_directories(_pboTempSrc);

	if (!DirExists(_pboTempSrc.c_str(), logger)) {
		logger->Log("Failed to create temp folder in P: Drive!"); // print error
		return 1; // return
	}

	//Create project folder in temp folder
	logger->Log("Directory created succesfully");

	logger->Log("Attempting to copy files...");
	//Copy over files to temp folder
	try
	{
		fs::copy(_pboSrc, _pboTempSrc, fs::copy_options::overwrite_existing | fs::copy_options::recursive);
	}
	catch (std::exception& e)
	{
		std::cout << e.what();
		return 1;
	}

	logger->Log("Succesfully copied over files");

	//Convert all .png's to .paa's
	if (DirExists("P:\\temp\\data\\layers", logger)) {
		logger->Log("Converting .png's to .paa's");

		std::string command = "\""; // Define path to TextureToPAA.exe
		command.append(_dzToolsImageToPaaDir);
		command.append("\\ImageToPAA.exe");
		command.append("\" ");
		command.append(_pboTempSrc);
		command.append("\\data\\layers"); // Run with this argument

		int result = system(command.c_str()); // Run the command

		if (result != 0) { // If Failed
			return 1; // Return error
		}
	}

	return 0;
}

/**
* Actually create the pbo and binarize
*/
int CreatePBO() {


	return 0;
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

		// Default direct copy files to not be binarized
		std::vector<std::string>_CopyDirectTypesDefault;
		_CopyDirectTypesDefault.insert(_CopyDirectTypesDefault.end(), {"*.emat","*.edds","*.ptc","*.c","*.imageset","*.layout","*.ogg","*.paa","*.rvmat"});

		_settingsFile << "ListOfFilesToCopyDirectly=";
		for (int i = 0; i < _CopyDirectTypesDefault.size(); i++) {
			_settingsFile << _CopyDirectTypesDefault[i];

			if (i != _CopyDirectTypesDefault.size() - 1) {
				_settingsFile << ";";
			}
		}
		_settingsFile << "\n";
		_settingsFile.close();
	}

	logger = new Logger(GetFilePath("log.txt"), _extensiveLogging); // Create logger instance

	logger->Log("PackPBO is created by Jens Heukers, www.jensheukers.nl");
	logger->Log(_versionString);
	logger->Log("\n");

	if (!DirExists("P:\\", logger)) {
		logger->Log("P: Drive is not mounted, Please mount P Drive and try again");
		delete logger;
		return 1;
	}

	std::string _dzToolsBinDir = _dzToolsDir;
	_dzToolsBinDir.append("\\Bin");
	if (!DirExists(_dzToolsBinDir.c_str(), logger)) {
		logger->Log("Cannot find DayZ tools directory, or directory is faulty");
		delete logger;
		return 1;
	}

	_dzToolsImageToPaaDir = _dzToolsDir;
	_dzToolsImageToPaaDir.append("\\Bin\\ImageToPAA");
	if (!DirExists(_dzToolsImageToPaaDir.c_str(), logger)) {
		logger->Log("Cannot find ImageToPAA directory, please re-install your DayZ Tools");
		delete logger;
		return 1;
	}

	// Get source file directory
	std::cout << "Please specify path to source files: ";
	std::getline(std::cin, _pboSrc); 
	
	logger->Log("SET: Source file dir:");
	logger->Log(_pboSrc);

	if (!DirExists(_pboSrc.c_str(), logger)) {
		delete logger;
		return 1;
	}

	// Get destination directory
	std::cout << "Please specify destination: ";
	std::getline(std::cin, _pboDest);

	logger->Log("SET: Destination dir:");
	logger->Log(_pboDest);

	if (!DirExists(_pboDest.c_str(), logger)) {
		delete logger;
		return 1;
	}
	
	int exitCode; 
	std::string exitLog;
	
	logger->LogLine(50);
	logger->Log("Calling PreparePBO()");
	logger->LogLine(50);
	exitCode = PreparePBO();
	exitLog = "PreparePBO() exited with Code: ";
	logger->Log(exitLog.append(std::to_string(exitCode)));
	logger->LogLine(50);
	logger->Log("Calling CreatePBO()");
	logger->LogLine(50);
	exitCode = CreatePBO();
	exitLog = "CreatePBO() exited with Code: ";
	logger->Log(exitLog.append(std::to_string(exitCode)));

	//Exit and Cleanup
	if (exitCode == 0) {
		logger->Log("All operations completed succesfully");
	}
	else {
		logger->Log("One or more operations failed..");
	}

	do {
		std::cout << "Press a key to continue...";
	} while (std::cin.get() != '\n');

	delete logger;

	return 0;
}