#include <stdlib.h>
#include "cutil/src/error.h"
#include "cutil/src/string.h"
#include "wavefront_object.h"

static struct WavefrontObjectObject* getObject(
        struct WavefrontObject* obj) {
    char defaultName[] = "";
    if (obj->objectCount == 0) {
        // Create a default object.
        if(wavefrontObjectAddObject(obj, defaultName)) return NULL;
    }
    unsigned int index = obj->currentObject;
    return obj->objects + index;
}

int wavefrontObjectCompose(struct WavefrontObject* obj) {
    memset(obj, 0, sizeof(struct WavefrontObject));
    obj->currentMaterial = -1;
    obj->currentObject = -1;
    return STATUS_OK;
}

int wavefrontObjectFaceAddPoint(
        struct WavefrontObjectFace* face,
        struct WavefrontObjectPoint* point) {
    struct WavefrontObjectPoint* temp = (struct WavefrontObjectPoint*) realloc(face->points,
        (face->pointCount + 1) * sizeof(struct WavefrontObjectPoint));
    if(temp == NULL) return STATUS_ALLOC_ERR;
    face->points = temp;
    face->points[face->pointCount++] = *point;
    return STATUS_OK;
}

void wavefrontObjectFaceFree(struct WavefrontObjectFace* face) {
    free(face->points);
}

void wavefrontObjectRelease(struct WavefrontObject* obj) {
    unsigned int materialLibraryIndex;
    for(materialLibraryIndex = 0;
        materialLibraryIndex < obj->materialLibraryCount;
        materialLibraryIndex++) {
        free(obj->materialLibraries[materialLibraryIndex]);
    }
    free(obj->materialLibraries);

    for(unsigned int i = 0; i < obj->objectCount; i++) {
        struct WavefrontObjectObject o = obj->objects[i];
        free(o.name);

        unsigned int faceIndex;
        for(faceIndex = 0;
            faceIndex < o.faceCount;
            faceIndex++) {
            wavefrontObjectFaceFree(o.faces + faceIndex);
        }
        free(o.faces);
    }
    unsigned int materialIndex;
    for(materialIndex = 0;
        materialIndex < obj->materialCount;
        materialIndex++) {
        free(obj->materials[materialIndex]);
    }
    free(obj->materials);

    free(obj->vertices);
    free(obj->unwraps);
    free(obj->normals);
}

int wavefrontObjectAddVertex(
        struct WavefrontObject* obj,
        struct WavefrontObjectVertex* vertex) {
    struct WavefrontObjectVertex* temp = (struct WavefrontObjectVertex*)realloc(
        obj->vertices,
        (obj->vertexCount + 1) * sizeof(struct WavefrontObjectVertex));
    if(temp == NULL) return STATUS_ALLOC_ERR;
    obj->vertices = temp;
    obj->vertices[obj->vertexCount++] = *vertex;
    return STATUS_OK;
}

int wavefrontObjectAddUnwrap(
        struct WavefrontObject* obj,
        struct WavefrontObjectUnwrap* unwrap) {
    struct WavefrontObjectUnwrap* temp = (struct WavefrontObjectUnwrap*)realloc(
        obj->unwraps,
        (obj->unwrapCount + 1) * sizeof(struct WavefrontObjectUnwrap));
    if(temp == NULL) return STATUS_ALLOC_ERR;
    obj->unwraps = temp;
    obj->unwraps[obj->unwrapCount++] = *unwrap;
    return STATUS_OK;
}

int wavefrontObjectAddNormal(
        struct WavefrontObject* obj,
        struct WavefrontObjectNormal* normal) {
    struct WavefrontObjectNormal* temp = (struct WavefrontObjectNormal*)realloc(
        obj->normals,
        (obj->normalCount + 1) * sizeof(struct WavefrontObjectNormal));
    if(temp == NULL) return STATUS_ALLOC_ERR;
    obj->normals = temp;
    obj->normals[obj->normalCount++] = *normal;
    return STATUS_OK;
}

int wavefrontObjectAddFace(
      struct WavefrontObject* obj,
      struct WavefrontObjectFace* face) {
    struct WavefrontObjectObject* o = getObject(obj);
    if(o == NULL) return STATUS_ALLOC_ERR;
    face->material = obj->currentMaterial;

    struct WavefrontObjectFace* temp = (struct WavefrontObjectFace*)realloc(
        o->faces,
        (o->faceCount + 1) * sizeof(struct WavefrontObjectFace));
    if(temp == NULL) return STATUS_ALLOC_ERR;
    o->faces = temp;
    o->faces[o->faceCount++] = *face;
    return STATUS_OK;
}

int wavefrontObjectAddMaterialLibrary(
      struct WavefrontObject* obj,
      const char* materialLibrary) {
    char* temp = strCopy(materialLibrary);
    if(temp == NULL) {
        return STATUS_ALLOC_ERR;
    }

    char** tempMtls = (char**)realloc(obj->materialLibraries,
        (obj->materialLibraryCount + 1) * sizeof(char*));
    if(tempMtls == NULL) {
        free(temp);
        return STATUS_ALLOC_ERR;
    }
    obj->materialLibraries = tempMtls;
    obj->materialLibraries[obj->materialLibraryCount++] = temp;
    return STATUS_OK;
}

int wavefrontObjectAddMaterial(
      struct WavefrontObject* obj,
      const char* material) {
    for (unsigned int i = 0; i < obj->materialCount; i++) {
        if (strcmp(material, obj->materials[i]) == 0) {
            obj->currentMaterial = i;
            return STATUS_OK;
        }
    }

    char* temp = strCopy(material);
    if(temp == NULL) return STATUS_ALLOC_ERR;

    char** tempMtls = (char**)realloc(
        obj->materials,
        (obj->materialCount + 1) * sizeof(char*));
    if(tempMtls == NULL) {
        free(temp);
        return STATUS_ALLOC_ERR;
    }
    obj->currentMaterial = obj->materialCount;
    obj->materials = tempMtls;
    obj->materials[obj->materialCount++] = temp;

    return STATUS_OK;
}

int wavefrontObjectAddObject(
      struct WavefrontObject* obj,
      const char* name) {
    for(unsigned int i = 0; i < obj->objectCount; i++) {
        if(strcmp(name, obj->objects[i].name) == 0) {
            obj->currentObject = i;
            return STATUS_OK;
        }
    }

    char* temp = strCopy(name);
    if(temp == NULL) {
        return STATUS_ALLOC_ERR;
    }

    struct WavefrontObjectObject* tempObj;
    tempObj = (struct WavefrontObjectObject*)realloc(
        obj->objects,
        (obj->objectCount + 1) * sizeof(struct WavefrontObjectObject));
    if (tempObj == NULL) {
        free(temp);
        return STATUS_ALLOC_ERR;
    }
    obj->objects = tempObj;

    obj->currentObject = obj->objectCount++;
    struct WavefrontObjectObject* o = obj->objects + obj->currentObject;
    o->name = temp;
    o->lines = 0;
    o->faces = NULL;
    o->faceCount = 0;
    o->lineCount = 0;
    return STATUS_OK;
}