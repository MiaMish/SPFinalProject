CC = gcc
CPP = g++
OBJS = main.o SPImageProc.o SPPoint.o SPListElement.o SPList.o \
SPBPriorityQueue.o SPConfig.o SPConfigUtils.o \
SPFeaturesSerializer.o SPKDArray.o SPKDTree.o SPLogger.o
EXEC = SPCBIR
INCLUDEPATH=/usr/local/lib/opencv-3.1.0/include/
LIBPATH=/usr/local/lib/opencv-3.1.0/lib/
LIBS=-lopencv_xfeatures2d -lopencv_features2d \
-lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_core


CPP_COMP_FLAG = -std=c++11 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

C_COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

$(EXEC): $(OBJS)
	$(CPP) $(OBJS) -L$(LIBPATH) $(LIBS) -o $@
main.o: main.cpp SPImageProc.h SPConfig.h SPLogger.h SPConfigUtils.h \
 SPPoint.h SPFeaturesSerializer.h SPKDTree.h SPKDArray.h \
 SPBPriorityQueue.h SPListElement.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
SPImageProc.o: SPImageProc.cpp SPImageProc.h SPConfig.h SPLogger.h \
 SPConfigUtils.h SPPoint.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
SPPoint.o: SPPoint.c SPPoint.h 
	$(CC) $(C_COMP_FLAG) -c $*.c
SPList.o: SPList.c SPList.h SPListElement.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPListElement.o: SPListElement.c SPListElement.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPBPriorityQueue.o: SPBPriorityQueue.c SPBPriorityQueue.h SPListElement.h \
 SPList.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPConfig.o: SPConfig.c SPConfig.h SPLogger.h SPConfigUtils.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPConfigUtils.o: SPConfigUtils.c SPConfigUtils.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPFeaturesSerializer.o: SPFeaturesSerializer.c SPFeaturesSerializer.h \
 SPConfig.h SPLogger.h SPConfigUtils.h SPPoint.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPKDArray.o: SPKDArray.c SPKDArray.h SPPoint.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPKDTree.o: SPKDTree.c SPKDTree.h SPKDArray.h SPPoint.h \
 SPBPriorityQueue.h SPListElement.h SPConfigUtils.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPLogger.o: SPLogger.c SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c

TESTS_OBJS = unit_tests.o sp_config_unit_tests.o sp_config_utils_unit_tests.o \
sp_kd_array_unit_tests.o sp_kd_tree_unit_tests.o SPConfig.o SPLogger.o SPConfigUtils.o \
SPPoint.o SPKDArray.o SPKDTree.o SPBPriorityQueue.o SPListElement.o SPList.o
TESTS_DIR = ./unit_tests
TESTS_EXEC = sp_tests

$(TESTS_EXEC): $(TESTS_OBJS)
	$(CC) $(TESTS_OBJS) -o $@
unit_tests.o: $(TESTS_DIR)/unit_tests.c $(TESTS_DIR)/unit_tests.h
	$(CC) $(C_COMP_FLAG) -c $(TESTS_DIR)/$*.c
sp_config_unit_tests.o: $(TESTS_DIR)/sp_config_unit_tests.c SPConfig.h \
 SPLogger.h SPConfigUtils.h $(TESTS_DIR)/unit_test_util.h $(TESTS_DIR)/unit_tests.h
	$(CC) $(C_COMP_FLAG) -c $(TESTS_DIR)/$*.c
sp_config_utils_unit_tests.o: $(TESTS_DIR)/sp_config_utils_unit_tests.c SPConfig.h \
 SPLogger.h SPConfigUtils.h \
 $(TESTS_DIR)/unit_test_util.h $(TESTS_DIR)/unit_tests.h
	$(CC) $(C_COMP_FLAG) -c $(TESTS_DIR)/$*.c
sp_kd_array_unit_tests.o: $(TESTS_DIR)/sp_kd_array_unit_tests.c SPKDArray.h \
 SPPoint.h $(TESTS_DIR)/unit_test_util.h $(TESTS_DIR)/unit_tests.h
	$(CC) $(C_COMP_FLAG) -c $(TESTS_DIR)/$*.c
sp_kd_tree_unit_tests.o: $(TESTS_DIR)/sp_kd_tree_unit_tests.c SPKDTree.h \
 SPKDArray.h SPPoint.h SPBPriorityQueue.h SPListElement.h \
 SPConfigUtils.h SPKDArray.h $(TESTS_DIR)/unit_test_util.h $(TESTS_DIR)/unit_tests.h
	$(CC) $(C_COMP_FLAG) -c $(TESTS_DIR)/$*.c

clean:
	rm -f $(OBJS) $(EXEC) $(TESTS_OBJS) $(TESTS_EXEC)
