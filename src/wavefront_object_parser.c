#include <stdlib.h>
#include <stdio.h>
#include "cutil/src/error.h"
#include "cutil/src/string.h"
#include "wavefront_object_parser.h"

static int parseVertex(struct WavefrontObject *obj, const char *line) {
    struct WavefrontObjectVertex vertex;
    vertex.w = 1.0;
    if(sscanf(
        line,
        "%lf %lf %lf %lf",
        &vertex.x,
        &vertex.y,
        &vertex.z,
        &vertex.w) >= 3) {
        return wavefrontObjectAddVertex(obj, &vertex);
    }
    return STATUS_PARSE_ERR;
}

static int parseUnwrap(struct WavefrontObject *obj, const char *line) {
    struct WavefrontObjectUnwrap unwrap;
    unwrap.w = 0.0;
    if(sscanf(line, "%lf %lf %lf", &unwrap.u, &unwrap.v, &unwrap.w) >= 2) {
        return wavefrontObjectAddUnwrap(obj, &unwrap);
    }
    return STATUS_PARSE_ERR;
}

static int parseNormal(struct WavefrontObject *obj, const char *line) {
    struct WavefrontObjectNormal normal;
    if(sscanf(line, "%lf %lf %lf", &normal.x, &normal.y, &normal.z) == 3) {
        return wavefrontObjectAddNormal(obj, &normal);
    }
    return STATUS_PARSE_ERR;
}

static int parsePoint(struct WavefrontObjectPoint *point, const char *input) {
    unsigned int indicies[3] = {0, 0, 0}; // Vertex, UV, Normal.
    short i = 0;
    const char *thisToken = input, *nextDelim = NULL, *nextToken = NULL;
    const char *lastDelim = NULL;
    while(tokenize(&thisToken, &nextDelim, &nextToken, "/")) {
        if(i>2) return STATUS_PARSE_ERR; // Garbage after indicies.
        if(thisToken==nextDelim) {// Token is the empty string.
            // Fail on missing vertex or dangling delimeter.
            if (i==0 || (lastDelim && !nextToken)) return STATUS_PARSE_ERR;
            indicies[i] = 0;
        }
        else if(strAfterInteger(thisToken) < nextDelim) return STATUS_PARSE_ERR; // Token is not an integer.
        else indicies[i] = atoi(thisToken);
        lastDelim = nextDelim;
        i++;
    }
    point->v = indicies[0];
    point->vt = indicies[1];
    point->vn = indicies[2];
    return STATUS_OK;
}

static int parseFace(struct WavefrontObject *obj, const char *line) {
    struct WavefrontObjectFace face;
    face.pointCount = 0;
    face.points = NULL;
    const char *thisToken = line, *nextDelim = NULL, *nextToken = NULL;
    while(tokenize(&thisToken, &nextDelim, &nextToken, ASCII_H_DELIMITERS)) {
        char *token = strCopyN(thisToken, nextDelim-thisToken);
        if(!token) return STATUS_ALLOC_ERR;

        struct WavefrontObjectPoint point;
        int result = parsePoint(&point, token);
        if(result == STATUS_OK) {
            result = wavefrontObjectFaceAddPoint(&face, &point);
        }

        free(token);
        if(result) return result;
    }
    return face.pointCount ? wavefrontObjectAddFace(obj, &face) : STATUS_PARSE_ERR;
}

static int parseMaterialLibrary(struct WavefrontObject *obj, const char *line) {
    int result = STATUS_PARSE_ERR;
    const char *thisToken = line, *nextDelim = NULL, *nextToken = NULL;
    // Remaining tokens are material library files.
    while(tokenize(&thisToken, &nextDelim, &nextToken, ASCII_H_DELIMITERS)) {
        if(thisToken==nextDelim) continue; // Ignore empty string tokens.
        char *token = strCopyN(thisToken, nextDelim-thisToken);
        if(!token) return STATUS_ALLOC_ERR;
        result = wavefrontObjectAddMaterialLibrary(obj, token);
        free(token);
        if(result) {
            return result;
        }
    };
    return STATUS_OK;
}

static int parseUseMaterial(struct WavefrontObject *obj, const char *line) {
    return wavefrontObjectAddMaterial(obj, line);
}

static int parseObject(struct WavefrontObject *obj, const char *line) {
    return wavefrontObjectAddObject(obj, line);
}

struct Parser {
    char name[8];
    int (*fn)(void *obj, const char *input);
};
struct Parser parsers[] = {
    {"v", (int (*)(void*, const char*))parseVertex},
    {"vt", (int (*)(void*, const char*))parseUnwrap},
    {"vn", (int (*)(void*, const char*))parseNormal},
    {"f", (int (*)(void*, const char*))parseFace},
    {"mtllib", (int (*)(void*, const char*))parseMaterialLibrary},
    {"usemtl", (int (*)(void*, const char*))parseUseMaterial},
    {"o", (int (*)(void*, const char*))parseObject},
    {"#", NULL}
};

int parseWavefrontObjectFromString(struct WavefrontObject *obj, char *input) {
    wavefrontObjectCompose(obj);

    const char *thisToken = input, *nextDelim = NULL, *nextToken = NULL;
    while(tokenize(&thisToken, &nextDelim, &nextToken, ASCII_V_DELIMITERS)) {
        char *line = strCopyN(thisToken, nextDelim-thisToken);
        if(!line) {
            wavefrontObjectRelease(obj);
            return STATUS_ALLOC_ERR;
        }
        int result = STATUS_OK;
        const char *tempLine = strAfterWhitespace(line);
        const char *thisToken = tempLine, *nextDelim = NULL, *nextToken = NULL;
        tokenize(&thisToken, &nextDelim, &nextToken, ASCII_H_DELIMITERS);
        for(int i = 0; i < sizeof(parsers)/sizeof(struct Parser); i++) {
            if((strStartsWith(thisToken, parsers[i].name) == nextDelim)) {
                const char *temp = strAfterWhitespace(nextDelim);
                result = parsers[i].fn ? parsers[i].fn(obj, temp) : STATUS_OK;
                break;
            }
        }
        free(line);
        if(result) {
            wavefrontObjectRelease(obj);
            return result;
        }
    }
    return STATUS_OK;
}
