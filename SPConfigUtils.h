/*
 * SPUtils.h
 */

#ifndef SPCONFIGUTILS_H_
#define SPCONFIGUTILS_H_

#define MAX_SIZE 1024

/** suffix for features file **/
#define spFeatsSuffix "feats"

/** the options for the cut method when the kd-tree is build **/
typedef enum sp_methods {
	RANDOM = 0, MAX_SPREAD = 1, INCREMENTAL = 2
} SplitMethod;

/** the options for the image suffix **/
typedef enum imageTypes {
	jpg, png, bmp, gif
} ImageType;

/*
 * if str is not a decimal number, returns -1
 * else returns an integer representing str
 */
int convertStringToNum(char str[]);

/*
 * each field name is assigned an integer
 * if field isn't one of the fields of SPConfig than returns -1
 */
int convertFieldToNum(char* field);

/* @param method
 * @returns method as string
 */
const char* convertMethodToString(SplitMethod method);

/* @param type
 * @returns type as string
 */
const char* convertTypeToString(ImageType type);

/*
 * recieves a  string and tries to split it into field and value
 * msg = SP_CONFIG_SUCCESS if line is in correct format
 * msg = SP_CONFIG_INVALID_STRING if line is not in the correct format
 * msg = SP_CONFIG_INVALID_INTEGER if value is supposed to be an integer,
 * msg = SP_CONFIG_INVALID_LINE if field doesn't exist but is not one
 *
 * @return non-negative number if line is valid
 * @return -1 if line is not valid
 */
int findFieldAndValue(char* line, SP_CONFIG_MSG* msg,
		char* field, char* value);

/*
 * @param filename - the name of the configuration file
 * @param lineNumber >= 0 - the line in the configuration file where error occured
 * @param msg - string with the massage to be printed
 */
void printError(const char* filename, int lineNumber, char* msg);

#endif /* SPCONFIGUTILS_H_ */
