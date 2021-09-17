#ifndef __WAVEFRONT_OBJECT_H
#define __WAVEFRONT_OBJECT_H
#ifdef __cplusplus
extern "C"{
#endif

struct WavefrontObjectVertex {
    double w, x, y, z;
};

struct WavefrontObjectUnwrap {
    double u, v, w;
};

struct WavefrontObjectNormal {
    double x, y, z;
};

struct WavefrontObjectPoint {
    int v, vt, vn;
};

struct WavefrontObjectFace {
    struct WavefrontObjectPoint* points;
    unsigned int pointCount;
    unsigned int material;
};

struct WavefrontObjectObject {
    char* name;
    struct WavefrontObjectVertex* vertices;
    struct WavefrontObjectUnwrap* unwraps;
    struct WavefrontObjectNormal* normals;
    struct WavefrontObjectFace* faces;
    char** materials;
    unsigned int vertexCount;
    unsigned int unwrapCount;
    unsigned int normalCount;
    unsigned int faceCount;
    unsigned int materialCount;
};

struct WavefrontObject {
    char** materialLibraries;
    struct WavefrontObjectObject* objects;
    unsigned int materialLibraryCount;
    unsigned int objectCount;
    int currentMaterial;
    int currentObject;
};

struct WavefrontObject* wavefrontObjectCreate();
void wavefrontObjectFaceAddPoint(struct WavefrontObjectFace* face, struct WavefrontObjectPoint* point);
void wavefrontObjectFaceFree(struct WavefrontObjectFace* face);
void wavefrontObjectFree(struct WavefrontObject* obj);
void wavefrontObjectAddVertex(struct WavefrontObject* obj, struct WavefrontObjectVertex* vertex);
void wavefrontObjectAddUnwrap(struct WavefrontObject* obj, struct WavefrontObjectUnwrap* unwrap);
void wavefrontObjectAddNormal(struct WavefrontObject* obj, struct WavefrontObjectNormal* normal);
void wavefrontObjectAddFace(struct WavefrontObject* obj, struct WavefrontObjectFace* face);
void wavefrontObjectAddMaterialLibrary(struct WavefrontObject* obj, char* materialLibrary);
int wavefrontObjectAddMaterial(struct WavefrontObject* obj, char* material);
int wavefrontObjectAddObject(struct WavefrontObject* obj, char* object);

#ifdef __cplusplus
}
#endif
#endif
