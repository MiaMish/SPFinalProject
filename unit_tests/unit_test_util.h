#ifndef UNIT_TEST_UTIL_H_
#define UNIT_TEST_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/*
 * macro to fail test
 */
#define FAIL(msg) do {\
		fprintf(stderr,"%s Line %d: %s", __FILE__, __LINE__, msg);\
		fflush(NULL);\
		return false;\
	} while(0)

/*
 * macro to assert an expression stands
 */
#define ASSERT_TRUE(expression) do { \
                if(!((expression))) { \
                        FAIL("expression is false :: "); \
                } \
        } while (0)

/*
 * macro to assert an expression does not stand
 */
#define ASSERT_FALSE(expression) do { \
                if((expression)) { \
                        FAIL("expression is true  ::"); \
                } \
		} while (0)

/*
 * macro to assert two operands are equal using == operator
 */
#define ASSERT_EQUALS(a,b) ASSERT_TRUE((a)==(b))

/*
 * macro to assert a given pointer is NULL
 */
#define ASSERT_NULL(a) ASSERT_TRUE((a)==NULL)

/*
 * macro to assert a given pointer is not NULL
 */
#define ASSERT_NOT_NULL(a) ASSERT_FALSE((a)==NULL)

/*
 * macro to run test and announce result
 */
#define RUN_TEST(f) do { \
			if(f()==true){ \
				fprintf(stdout, "%s  PASS\n",#f);\
				fflush(NULL);\
			}else{ fprintf(stderr, "%s  FAIL\n",#f);\
			} }while (0)

#ifdef __cplusplus
}
#endif

#endif /* UNIT_TEST_UTIL_H_ */
