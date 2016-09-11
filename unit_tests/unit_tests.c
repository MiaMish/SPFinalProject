#include "unit_tests.h"
#include <stdio.h>

int main() {
	setvbuf (stdout, NULL, _IONBF, BUFSIZ);

	printf("Running config tests\n");
	sp_config_unit_tests();

	printf("Running config utils tests\n");
	//sp_config_utils_unit_tests();

	printf("Running kdarray tests\n");
	sp_kd_array_unit_tests();

	printf("Running kdtree tests\n");
	sp_kd_tree_unit_tests();

	printf("Done!\n");

	return 0;
}
