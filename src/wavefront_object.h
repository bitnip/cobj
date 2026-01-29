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
    struct WavefrontObjectPoint *points;
    unsigned int pointCount;
    unsigned int material;
};

struct WavefrontObjectObject {
    char *name;
    struct WavefrontObjectFace *faces;
    unsigned int faceCount;
};

struct WavefrontObject {
    char **materialLibraries;
    char **materials;
    struct WavefrontObjectObject *objects;
    struct WavefrontObjectVertex *vertices;
    struct WavefrontObjectUnwrap *unwraps;
    struct WavefrontObjectNormal *normals;
    unsigned int materialLibraryCount;
    unsigned int vertexCount;
    unsigned int unwrapCount;
    unsigned int normalCount;
    unsigned int objectCount;
    unsigned int materialCount;
    int currentMaterial;
    int currentObject;
};

int wavefrontObjectCompose(struct WavefrontObject *obj);
int wavefrontObjectFaceAddPoint(struct WavefrontObjectFace *face, struct WavefrontObjectPoint *point);
void wavefrontObjectFaceFree(struct WavefrontObjectFace *face);
void wavefrontObjectRelease(struct WavefrontObject *obj);
int wavefrontObjectAddVertex(struct WavefrontObject *obj, struct WavefrontObjectVertex *vertex);
int wavefrontObjectAddUnwrap(struct WavefrontObject *obj, struct WavefrontObjectUnwrap *unwrap);
int wavefrontObjectAddNormal(struct WavefrontObject *obj, struct WavefrontObjectNormal *normal);
int wavefrontObjectAddFace(struct WavefrontObject *obj, struct WavefrontObjectFace *face);
int wavefrontObjectAddMaterialLibrary(struct WavefrontObject *obj, const char *materialLibrary);
int wavefrontObjectAddMaterial(struct WavefrontObject *obj, const char *material);
int wavefrontObjectAddObject(struct WavefrontObject *obj, const char *object);

#ifdef __cplusplus
}
#endif
#endif
