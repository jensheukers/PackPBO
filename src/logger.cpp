/**
*	Filename: logger.cpp
*
*	Description: Source file for logger class
*	Version: 20/11/2018
*
*	© 2018, www.jensheukers.nl
*/
#include "logger.h"
#include <fstream>

Logger::Logger(std::string _logFile, bool _loudOutput) {
	this->_logFile = _logFile;
	this->_loudOutput = _loudOutput;

	std::ofstream _file(_logFile, std::ofstream::out);
}

Logger::~Logger() {
	this->Log("Logger Terminated");
}

void Logger::Log(std::string line) {
	std::ofstream _logFileStream(_logFile, std::ofstream::app);

	_logFileStream << line << "\n"; //Log to file

	if (_loudOutput) {
		std::cout << line << "\n"; //Log to console
	}

	_logFileStream.close();
}

void Logger::LogLine(int lenght) {
	std::string line;
	for (int i = 0; i <= lenght; i++) {
		line.append("-");
	}
	this->Log(line);
}