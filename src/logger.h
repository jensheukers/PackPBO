/**
*	Filename: logger.h
*
*	Description: Header file for logger class
*	Version: 19/11/2018
*
*	© 2018, www.jensheukers.nl
*/
#include <iostream>
#include <string>

class Logger {
private:
	std::string _logFile; /// @brief file to log to.
	bool _loudOutput; /// @brief defines if everything should be logged in console.
public:
	/**
	* Constructor
	*/
	Logger(std::string _logFile, bool _loudOutput);

	/**
	* Log a line to the logfile and console
	*/
	void Log(std::string line);
};