# Copyright (C) 2015, Harikan Dawn Associates.All Rights Reserved.
# Contributors include : Joe A Lyon.
#
#
#   make [all]  - makes everything.
#   make TARGET - makes the given target.
#   make clean  - removes all files generated by make.





GTEST_DIR = googletest
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)





USER_DIR = .

CPPFLAGS += -isystem $(GTEST_DIR)/include
CXXFLAGS += -pthread -std=c++11 -I $(GTEST_DIR) -I $(GTEST_DIR)/include ###-g -Wall -Wextra 

APPLICATION = HarikanDawnServer

cFiles :=$(wildcard *.cc)
cFilesWithoutMain := $(filter-out HarikanDawnServer.cc,$(filter-out HarikanDawnServer_unittest.cc,$(cFiles)))
cFilesWithoutMainOrUnitTests := $(filter-out %_unittest.cc,$(cFilesWithoutMain))

Objects := $(patsubst %.cc,%.o,$(cFiles))
ObjectsWithoutMain := $(patsubst %.cc,%.o,$(cFilesWithoutMain))
ObjectsWithoutMainOrUnitTests := $(patsubst %.cc,%.o,$(cFilesWithoutMainOrUnitTests))

unitTestCFiles := $(wildcard *_unittest.cc)
unitTestFiles := $(patsubst %unittest.cc,%unittest,$(unitTestCFiles))





all : $(APPLICATION) $(unitTestFiles)

clean :
	rm -f $(unitTestFiles) gtest.a gtest_main.a *.o



gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^



ObjectFiles : $(Objects) $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(cFiles)

$(unitTestFiles) : gtest_main.a ObjectFiles
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread -o $@ gtest_main.a $(ObjectsWithoutMainOrUnitTests)

HarikanDawnServer : ObjectFiles
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread -o $@  HarikanDawnServer.o  $(ObjectsWithoutMainOrUnitTests)
