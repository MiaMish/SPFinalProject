/*
 * SPUtils.h
 */

#ifndef SPCONFIGUTILS_H_
#define SPCONFIGUTILS_H_



/** suffix for features file **/
#define spFeatsSuffix ".feats"

/** the options for the cut method when the kd-tree is build **/
typedef enum sp_methods {
	RANDOM = 0, MAX_SPREAD = 1, INCREMENTAL = 2
} SplitMethod;

/** the options for the image suffix **/
typedef enum imageTypes {
	jpg = 0, png = 1, bmp = 2, gif = 3
} ImageType;

#define MAX_SIZE 1024

#define SKIP_SPACES(ptr)  \
		while (*ptr == ' ') {  \
			ptr++;  \
		}

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
 * @param line - a pointer to not-NULL char array
 * @param value - a pointer to not-NULL char array
 *
 * checks whether line is in format ( )*string( )*=()*string( )*
 * if yes - saves to value the second string and returns fieldId of first string
 *
 * return 0 - line is empty or a comment
 * return -1 if line format is not field = value
 * return convertFieldToNum(field) if line is in correct format
 */
int extractFieldAndValue(const char* line, char* value);

/*
 * prints to stdout massage regarding error in configuration file
 *
 * @param filename - the name of the configuration file
 * @param lineNumber >= 0 - the line in the configuration file where error occured
 * @param msg - string with the massage to be printed
 */
void printErrorInConfig(const char* filename, int lineNumber, char* msg);

#endif /* SPCONFIGUTILS_H_ */
