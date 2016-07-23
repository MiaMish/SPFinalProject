#include "SPLogger.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//File open mode
#define SP_LOGGER_OPEN_MODE "w"

//error types
#define ERROR "---ERROR---"
#define WARNING "---WARNING---"
#define INFO "---INFO---"
#define DEBUG "---DEBUG---"

#define NEG -1
#define INPUT_MAX 1024

// Global variable holding the logger
SPLogger logger = NULL;

/**
 * @param msg - The message to be printed
 * @return
 * SP_LOGGER_UNDIFINED 			- If the logger is undefined
 * SP_LOGGER_INVAlID_ARGUMENT	- If msg is null
 * SP_LOGGER_SUCCESS			- otherwise
 */
SP_LOGGER_MSG spLoggerBeforePrint(const char* msg);

/**
 * The given message is printed. A new line is printed at the end of msg
 *
 * @param msg - The message to be printed
 * @return
 * SP_LOGGER_WRITE_FAIL			- If Write failure occurred
 * SP_LOGGER_SUCCESS			- otherwise
 */
SP_LOGGER_MSG spLoggerPrint(const char* msg);

/**
 * 	if type is in the right level: Prints the type message. The debug message format is given below:
 * 	---TYPE---
 * 	and for type != INFO:
 * 	- file: <file>
 *  - function: <function>
 *  - line: <line>
 *  for all types:
 *  - message: <msg>
 *
 * 	<file> 	   - is the string given by file, it represents the file in which
 * 		   		 the debug print call occurred.
 * 	<function> - is the string given by function, it represents the function in which
 * 			   	 the debug print call occurred.
 * 	<line> 	   - is the string given by line, it represents the line in which
 * 		   		 the debug print call occurred
 * 	<msg> 	   - is the string given by msg, it contains the msg given by the user
 *
 *
 * 	A new line will be printed after the print call.
 *
 * @param msg     	- The message to printed
 * @param file    	- A string representing the filename in which spLoggerPrintWarning call occurred
 * @param function 	- A string representing the function name in which spLoggerPrintWarning call ocurred
 * @param line		- A string representing the line in which the function call occurred
 * @return
 * SP_LOGGER_UNDIFINED 			- If the logger is undefined
 * SP_LOGGER_INVAlID_ARGUMENT	- If any of msg or file or function are null or line is negative
 * SP_LOGGER_WRITE_FAIL			- If Write failure occurred
 * SP_LOGGER_SUCCESS			- otherwise
 */
SP_LOGGER_MSG spLoggerPrintType(char* type, const char* msg, const char* file,
		const char* function, const int line);

/**
 * @param type		-ERROR || WARNING || INFO || DEBUG
 * @return
 * true				-if type can be printed in the logger's level
 * false 			-otherwise
 */
bool correctLevel(char* type);


struct sp_logger_t {
	FILE* outputChannel; //The logger file
	bool isStdOut; //Indicates if the logger is stdout
	SP_LOGGER_LEVEL level; //Indicates the level
};

SP_LOGGER_MSG spLoggerCreate(const char* filename, SP_LOGGER_LEVEL level) {
	if (logger != NULL) { //Already defined
		return SP_LOGGER_DEFINED;
	}
	logger = (SPLogger) malloc(sizeof(struct sp_logger_t));
	if (logger == NULL) { //Allocation failure
		return SP_LOGGER_OUT_OF_MEMORY;
	}
	logger->level = level; //Set the level of the logger
	if (filename == NULL) { //In case the filename is not set use stdout
		logger->outputChannel = stdout;
		logger->isStdOut = true;
	} else { //Otherwise open the file in write mode
		logger->outputChannel = fopen(filename, SP_LOGGER_OPEN_MODE);
		if (logger->outputChannel == NULL) { //Open failed
			free(logger);
			logger = NULL;
			return SP_LOGGER_CANNOT_OPEN_FILE;
		}
		logger->isStdOut = false;
	}
	return SP_LOGGER_SUCCESS;
}

void spLoggerDestroy() {
	if (!logger) {
		return;
	}
	if (!logger->isStdOut) {//Close file only if not stdout
		fclose(logger->outputChannel);
	}
	free(logger);//free allocation
	logger = NULL;
}

SP_LOGGER_MSG spLoggerPrintError(const char* msg, const char* file,
		const char* function, const int line) {
	return spLoggerPrintType(ERROR, msg, file, function, line);
}

SP_LOGGER_MSG spLoggerPrintWarning(const char* msg, const char* file,
		const char* function, const int line) {
	return spLoggerPrintType(WARNING, msg, file, function, line);
}

SP_LOGGER_MSG spLoggerPrintInfo(const char* msg) {
	return spLoggerPrintType(INFO, msg, NULL, NULL, NEG);
}

SP_LOGGER_MSG spLoggerPrintDebug(const char* msg, const char* file,
		const char* function, const int line) {
	return spLoggerPrintType(DEBUG, msg, file, function, line);
}

SP_LOGGER_MSG spLoggerPrintMsg(const char* msg) {
	SP_LOGGER_MSG beforeMsgPrint = spLoggerBeforePrint(msg);

	if(beforeMsgPrint != SP_LOGGER_SUCCESS) {
		return beforeMsgPrint;
	}

	return spLoggerPrint(msg);
}

SP_LOGGER_MSG spLoggerBeforePrint(const char* msg) {
	if (!logger) { /* logger == NULL */
		return SP_LOGGER_UNDIFINED;
	}
	if(msg == NULL) {
		return SP_LOGGER_INVAlID_ARGUMENT;
	}
	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrint(const char* msg) {
	int fprintfResult;

	fprintfResult = fprintf(logger->outputChannel, "%s\n", msg);
	fflush(logger->outputChannel);
	if (fprintfResult < 0) {
		return SP_LOGGER_WRITE_FAIL;
	}

	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintType(char* type, const char* msg, const char* file,
		const char* function, const int line) {
	int sprintffResult;
	char newMsg[INPUT_MAX];
	SP_LOGGER_MSG beforeMsgPrint = spLoggerBeforePrint(msg);

	if(beforeMsgPrint != SP_LOGGER_SUCCESS) {
		return beforeMsgPrint;
	}

	if(strcmp(type, INFO) != 0) {
		if(line < 0 || file == NULL || function == NULL) {
			return SP_LOGGER_INVAlID_ARGUMENT;
		}
		if(!correctLevel(type)) {
			return SP_LOGGER_SUCCESS;
		}
		sprintffResult = sprintf(newMsg, "%s\n- file: %s\n- function: %s\n- line: %d\n- message: %s",
				type, file, function, line, msg);
	} else {
		if(!correctLevel(type)) {
			return SP_LOGGER_SUCCESS;
		}
		sprintffResult = sprintf(newMsg, "%s\n- message: %s", type, msg);
	}

	if (sprintffResult < 0) {
		return SP_LOGGER_WRITE_FAIL;
	}

	return spLoggerPrint(newMsg);
}

SP_LOGGER_LEVEL stringToLevel(char* type) {
	if (strcmp(type, ERROR) == 0) {
		return SP_LOGGER_ERROR_LEVEL;
	}
	if (strcmp(type, WARNING) == 0) {
		return SP_LOGGER_WARNING_ERROR_LEVEL;
	}
	if (strcmp(type, INFO) == 0) {
		return SP_LOGGER_INFO_WARNING_ERROR_LEVEL;
	}
	else {
		return SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL;
	}
}

bool correctLevel(char* type) {
	SP_LOGGER_LEVEL logLevel = stringToLevel(type);
	return logLevel <= logger->level;
}
