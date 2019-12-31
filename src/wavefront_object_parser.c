#include "cutil/src/string.h"

#include "wavefront_object_parser.h"

static char parseComment(char* line)
{
    char* trimmed = afterWhitespace(line);
    return *trimmed == '#';
}

int parseVertex(struct WavefrontObjectVertex* vertex, char* line)
{
    line = afterWhitespace(line);
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

int parseUnwrap(struct WavefrontObjectUnwrap* unwrap, char* line)
{
    line = afterWhitespace(line);

    int itemsRead = sscanf(line, "vt %lf %lf %lf", &unwrap->u, &unwrap->v, &unwrap->w);
    if(itemsRead < 2)
        return 0;
    if(itemsRead == 2)
        unwrap->w = 0.0;
    return 1;
}

int parseNormal(struct WavefrontObjectNormal* normal, char* line)
{
    line = afterWhitespace(line);
    int itemsRead = sscanf(line, "vn %lf %lf %lf", &normal->x, &normal->y, &normal->z);
    if(itemsRead != 3)
        return 0;
    return 1;
}

int parsePoint(struct WavefrontObjectPoint* point, char* input)
{
    unsigned int indicies[3] = {0}; // Vertex, UV, Normal.
    short i = 0;
    char *thisToken = input, *nextDelim = NULL, *nextToken = NULL;
    char *lastDelim = NULL;
    while (tokenize(&thisToken, &nextDelim, &nextToken, "/")) {
        if (i>2) return 0; // Garbage after indicies.
        if (thisToken==nextDelim) {// Token is the empty string.
            // Fail on missing vertex or dangling delimeter.
            if (i==0 || (lastDelim && !nextToken)) return 0;
            indicies[i] = 0;
        }
        else if (afterInteger(thisToken) < nextDelim) return 0; // Token is not an integer.
        else indicies[i] = atoi(thisToken);
        lastDelim = nextDelim;
        i++;
    }
    point->v = indicies[0];
    point->vt = indicies[1];
    point->vn = indicies[2];
    return 1;
}

int parseFace(struct WavefrontObjectFace* face, char* line)
{
    line = afterWhitespace(line);
    if(*(line++) != 'f') return 0; // Not a face definition.
    struct WavefrontObjectPoint point;
    char *thisToken = line, *nextDelim = NULL, *nextToken = NULL;
    while (tokenize(&thisToken, &nextDelim, &nextToken, ASCII_H_DELIMITERS)) {
        *nextDelim = '\0'; // Terminate the token at its end delimiter.
        if(parsePoint(&point, thisToken)) wavefrontObjectFaceAddPoint(face, &point);
    }
    return 1;
}

int parseMaterialLibrary(struct WavefrontObject* obj, char* line)
{
    line = afterWhitespace(line);
    char *thisToken = line, *nextDelim = NULL, *nextToken = NULL;
    tokenize(&thisToken, &nextDelim, &nextToken, ASCII_H_DELIMITERS);
    if (strStartsWith(thisToken, "mtllib") != nextDelim) return 0; // Garbage after map_kd.
    // Remaining tokens are material library files.
    line = afterWhitespace(line);
    while (tokenize(&thisToken, &nextDelim, &nextToken, ASCII_H_DELIMITERS)) {
        *nextDelim='\0';
        wavefrontObjectAddMaterialLibrary(obj, thisToken);
    };
    return 1;
}

int parseUseMaterial(struct WavefrontObject* obj, char* line)
{
    line = afterWhitespace(line);
    char *thisToken = line, *nextDelim = NULL, *nextToken = NULL;
    tokenize(&thisToken, &nextDelim, &nextToken, ASCII_H_DELIMITERS);
    if (strStartsWith(thisToken, "usemtl") != nextDelim) return 0; // Garbage after map_kd.
    wavefrontObjectAddMaterial(obj, nextToken);
    return 1;
}

int parseObject(struct WavefrontObject* obj, char* line)
{
    line = afterWhitespace(line);
    if (*line++ != 'o') return 0;
    line = afterWhitespace(line);
    wavefrontObjectAddObject(obj, line);
    return 1;
}

int parseLine(struct WavefrontObject* obj, char* line)
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

struct WavefrontObject* parseWavefrontObjectFromString(char* input)
{
    if (!input) return NULL;
    struct WavefrontObject* obj = wavefrontObjectCreate();

    char *thisToken = input, *nextDelim = NULL, *nextToken = NULL;
    while (tokenize(&thisToken, &nextDelim, &nextToken, ASCII_V_DELIMITERS)) {
        *nextDelim='\0';
        parseLine(obj, thisToken);
    }
    return obj;
}
