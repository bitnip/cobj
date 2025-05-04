SOURCE= src/wavefront_object.c \
	src/wavefront_object_parser.c
TEST_SOURCE= \
	src/test.c \
	src/wavefront_object_parser_test.c
LIBRARIES=-lcutil -L ../cutil/bin
INCLUDES=-I../

COVERAGE_CC=gcc
ifeq ($(shell uname -s),Darwin)
	CC=gcc
endif
ifeq ($(shell uname -s),Linux)
	CC=gcc
endif
ifeq ($(OS),Windows_NT)
	CC=x86_64-w64-mingw32-gcc
endif