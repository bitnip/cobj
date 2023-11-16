#ifndef __WAVEFRONT_OBJECT_PARSER_H
#define __WAVEFRONT_OBJECT_PARSER_H
#ifdef __cplusplus
extern "C"{
#endif

#include "wavefront_object.h"

int parseWavefrontObjectFromString(struct WavefrontObject *obj, char *input);

#ifdef __cplusplus
}
#endif
#endif