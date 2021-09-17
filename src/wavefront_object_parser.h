#ifndef __WAVEFRONT_OBJECT_PARSER_H
#define __WAVEFRONT_OBJECT_PARSER_H
#ifdef __cplusplus
extern "C"{
#endif

#include "wavefront_object.h"

struct WavefrontObject* parseWavefrontObjectFromString(char* input);

#ifdef __cplusplus
}
#endif
#endif
