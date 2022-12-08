#include "cutil/src/string.h"

#include "wavefront_object_parser.h"

static char parseComment(const char* line)
{
    const char* trimmed = strAfterWhitespace(line);
    return *trimmed == '#';
}

int parseVertex(struct WavefrontObjectVertex* vertex, const char* line)
{
    line = strAfterWhitespace(line);
    int itemsRead = sscanf(
        line,
        "v %lf %lf %lf %lf",
        &vertex->x,
        &vertex->y,
        &vertex->z,
        &vertex->w);
    if(itemsRead < 3) return 0;
    if(itemsRead == 3) vertex->w = 1.0;
    return 1;
}

int parseUnwrap(struct WavefrontObjectUnwrap* unwrap, const char* line)
{
    line = strAfterWhitespace(line);

    int itemsRead = sscanf(line, "vt %lf %lf %lf", &unwrap->u, &unwrap->v, &unwrap->w);
    if(itemsRead < 2)
        return 0;
    if(itemsRead == 2)
        unwrap->w = 0.0;
    return 1;
}

int parseNormal(struct WavefrontObjectNormal* normal, const char* line)
{
    line = strAfterWhitespace(line);
    int itemsRead = sscanf(line, "vn %lf %lf %lf", &normal->x, &normal->y, &normal->z);
    if(itemsRead != 3)
        return 0;
    return 1;
}

int parsePoint(struct WavefrontObjectPoint* point, const char* input)
{
    unsigned int indicies[3] = {0}; // Vertex, UV, Normal.
    short i = 0;
    const char *thisToken = input, *nextDelim = NULL, *nextToken = NULL;
    const char *lastDelim = NULL;
    while (tokenize(&thisToken, &nextDelim, &nextToken, "/")) {
        if (i>2) return 0; // Garbage after indicies.
        if (thisToken==nextDelim) {// Token is the empty string.
            // Fail on missing vertex or dangling delimeter.
            if (i==0 || (lastDelim && !nextToken)) return 0;
            indicies[i] = 0;
        }
        else if (strAfterInteger(thisToken) < nextDelim) return 0; // Token is not an integer.
        else indicies[i] = atoi(thisToken);
        lastDelim = nextDelim;
        i++;
    }
    point->v = indicies[0];
    point->vt = indicies[1];
    point->vn = indicies[2];
    return 1;
}

int parseFace(struct WavefrontObjectFace* face, const char* line)
{
    line = strAfterWhitespace(line);
    if(*(line++) != 'f') return 0; // Not a face definition.
    struct WavefrontObjectPoint point;
    const char *thisToken = line, *nextDelim = NULL, *nextToken = NULL;
    while (tokenize(&thisToken, &nextDelim, &nextToken, ASCII_H_DELIMITERS)) {
        char* token = strCopyN(thisToken, nextDelim-thisToken);
        if(!token) return 0;
        if(parsePoint(&point, token)) wavefrontObjectFaceAddPoint(face, &point);
        free(token);
    }
    return 1;
}

int parseMaterialLibrary(struct WavefrontObject* obj, const char* line)
{
    line = strAfterWhitespace(line);
    const char *thisToken = line, *nextDelim = NULL, *nextToken = NULL;
    tokenize(&thisToken, &nextDelim, &nextToken, ASCII_H_DELIMITERS);
    if (strStartsWith(thisToken, "mtllib") != nextDelim) return 0; // Garbage after map_kd.
    // Remaining tokens are material library files.
    line = strAfterWhitespace(line);
    while (tokenize(&thisToken, &nextDelim, &nextToken, ASCII_H_DELIMITERS)) {
        char* token = strCopyN(thisToken, nextDelim-thisToken);
        if(!token) return 0;
        wavefrontObjectAddMaterialLibrary(obj, token);
        free(token);
    };
    return 1;
}

int parseUseMaterial(struct WavefrontObject* obj, const char* line)
{
    line = strAfterWhitespace(line);
    const char *thisToken = line, *nextDelim = NULL, *nextToken = NULL;
    tokenize(&thisToken, &nextDelim, &nextToken, ASCII_H_DELIMITERS);
    if (strStartsWith(thisToken, "usemtl") != nextDelim) return 0; // Garbage after map_kd.
    wavefrontObjectAddMaterial(obj, nextToken);
    return 1;
}

int parseObject(struct WavefrontObject* obj, const char* line)
{
    line = strAfterWhitespace(line);
    if (*line++ != 'o') return 0;
    line = strAfterWhitespace(line);
    wavefrontObjectAddObject(obj, line);
    return 1;
}

int parseLine(struct WavefrontObject* obj, const char* line)
{
    if (obj == NULL || line == NULL) return 0;

    struct WavefrontObjectVertex vertex;
    struct WavefrontObjectUnwrap unwrap;
    struct WavefrontObjectNormal normal;
    struct WavefrontObjectFace face;
    face.pointCount = 0;
    face.points = NULL;

    if (parseVertex(&vertex, line))      wavefrontObjectAddVertex(obj, &vertex);
    else if (parseUnwrap(&unwrap, line)) wavefrontObjectAddUnwrap(obj, &unwrap);
    else if (parseNormal(&normal, line)) wavefrontObjectAddNormal(obj, &normal);
    else if (parseFace(&face, line))     wavefrontObjectAddFace(obj, &face);
    else if (parseMaterialLibrary(obj, line));
    else if (parseUseMaterial(obj, line));
    else if (parseObject(obj, line));
    else if (parseComment(line));
    else return 0;

    return 1;
}

struct WavefrontObject* parseWavefrontObjectFromString(char* input) {
    if (!input) return NULL;
    struct WavefrontObject* obj = wavefrontObjectCreate();

    const char *thisToken = input, *nextDelim = NULL, *nextToken = NULL;
    while (tokenize(&thisToken, &nextDelim, &nextToken, ASCII_V_DELIMITERS)) {
        char* line = strCopyN(thisToken, nextDelim-thisToken);
        if(!line) return NULL;
        parseLine(obj, line);
        free(line);
    }
    return obj;
}
