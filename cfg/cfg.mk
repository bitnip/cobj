SOURCE= src/wavefront_object.c \
	src/wavefront_object_parser.c
TEST_SOURCE= \
	src/test.c \
	src/wavefront_object_parser_test.c
LIBRARIES=-lcutil -L ../cutil/bin
INCLUDES=-I../
