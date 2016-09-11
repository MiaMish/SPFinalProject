/*
 * sp_config_utils_unit_tests.h
 */

#ifndef SP_CONFIG_UTILS_UNIT_TESTS_H_
#define SP_CONFIG_UTILS_UNIT_TESTS_H_

/*
 * tests if convertStringToNum works
 * @return true if all tests pass
 */
bool stringToIntTest();

/*
 * tests if convertFieldToNum works
 * @return true if all tests pass
 */
bool fieldToNumTest();

/*
 * tests if convertMethodToString works
 * @return true if all tests pass
 */
bool methodToStringTest();

/*
 * tests if convertTypeToString works
 * @return true if all tests pass
 */
bool typeToStringTest();

/*
 * tests if extractFieldAndValue works
 * @return true if all tests pass
 */
bool extractFieldAndValueTest();

/*
 * run all tests in this header file
 * @return 1 if all tests pass
 */
int main_utils_test();


#endif /* SP_CONFIG_UTILS_UNIT_TESTS_H_ */
