#include <stdio.h>
#include <stdlib.h>
#include "cutil/src/string.h"

#include "wavefront_object.h"

static struct WavefrontObjectObject* getObject(
      struct WavefrontObject* obj){
    char defaultName[] = "";
    unsigned int index = obj->currentObject;
    if (obj->objectCount == 0) {
        // Create a default object.
        index = wavefrontObjectAddObject(obj, defaultName);
    }
    return obj->objects + index;
}

struct WavefrontObject* wavefrontObjectCreate() {
    struct WavefrontObject* wavefrontObject;
    wavefrontObject = (struct WavefrontObject *) malloc(sizeof(struct WavefrontObject));

    wavefrontObject->materialLibraries = NULL;
    wavefrontObject->objects = NULL;

    wavefrontObject->materialLibraryCount = 0;
    wavefrontObject->objectCount = 0;

    wavefrontObject->currentMaterial = -1;
    wavefrontObject->currentObject = -1;

    wavefrontObject->vertices = NULL;
    wavefrontObject->unwraps = NULL;
    wavefrontObject->normals = NULL;
    wavefrontObject->vertexCount = 0;
    wavefrontObject->unwrapCount = 0;
    wavefrontObject->normalCount = 0;

    return wavefrontObject;
}

void wavefrontObjectFaceAddPoint(
      struct WavefrontObjectFace* face,
      struct WavefrontObjectPoint* point) {
    face->points = (struct WavefrontObjectPoint *) realloc(face->points,
        ++face->pointCount * sizeof(struct WavefrontObjectPoint));
    face->points[face->pointCount - 1] = *point;
}

void wavefrontObjectFaceFree(struct WavefrontObjectFace* face) {
    free(face->points);
}

void wavefrontObjectFree(struct WavefrontObject* obj) {
    unsigned int materialLibraryIndex;
    for ( materialLibraryIndex = 0;
          materialLibraryIndex < obj->materialLibraryCount;
          materialLibraryIndex++) {
        free(obj->materialLibraries[materialLibraryIndex]);
    }
    free(obj->materialLibraries);
    obj->materialLibraryCount = 0;

    for (unsigned int i = 0; i < obj->objectCount; i++) {
        struct WavefrontObjectObject o = obj->objects[i];
        free(o.name);
        unsigned int materialIndex;
        for ( materialIndex = 0;
              materialIndex < o.materialCount;
              materialIndex++) {
            free(o.materials[materialIndex]);
        }
        free(o.materials);

        unsigned int faceIndex;
        for ( faceIndex = 0;
              faceIndex < o.faceCount;
              faceIndex++) {
            wavefrontObjectFaceFree(o.faces + faceIndex);
        }
        free(o.faces);
    }
    obj->objectCount = 0;

    free(obj->vertices);
    free(obj->unwraps);
    free(obj->normals);
    obj->vertexCount = 0;
    obj->unwrapCount = 0;
    obj->normalCount = 0;
    free(obj);
}

void wavefrontObjectAddVertex(
      struct WavefrontObject* obj,
      struct WavefrontObjectVertex* vertex) {
    obj->vertices = (struct WavefrontObjectVertex*) realloc(
        obj->vertices,
        ++obj->vertexCount * sizeof(struct WavefrontObjectVertex));
    obj->vertices[obj->vertexCount - 1] = *vertex;
}

void wavefrontObjectAddUnwrap(
      struct WavefrontObject* obj,
      struct WavefrontObjectUnwrap* unwrap) {
    obj->unwraps = (struct WavefrontObjectUnwrap*) realloc(
        obj->unwraps,
        ++obj->unwrapCount * sizeof(struct WavefrontObjectUnwrap));
    obj->unwraps[obj->unwrapCount - 1] = *unwrap;
}

void wavefrontObjectAddNormal(
      struct WavefrontObject* obj,
      struct WavefrontObjectNormal* normal) {
    obj->normals = (struct WavefrontObjectNormal*) realloc(
        obj->normals,
        ++obj->normalCount * sizeof(struct WavefrontObjectNormal));
    obj->normals[obj->normalCount - 1] = *normal;
}

void wavefrontObjectAddFace(
      struct WavefrontObject* obj,
      struct WavefrontObjectFace* face) {
    struct WavefrontObjectObject* o = getObject(obj);
    face->material = obj->currentMaterial;
    o->faces = (struct WavefrontObjectFace *) realloc(
        o->faces,
        ++o->faceCount * sizeof(struct WavefrontObjectFace));
    o->faces[o->faceCount - 1] = *face;
}

void wavefrontObjectAddMaterialLibrary(
      struct WavefrontObject* obj,
      const char* materialLibrary) {
    obj->materialLibraries = (char **) realloc(obj->materialLibraries,
        ++obj->materialLibraryCount * sizeof(char*));
    obj->materialLibraries[obj->materialLibraryCount - 1] = strCopy(materialLibrary);
}

int wavefrontObjectAddMaterial(
      struct WavefrontObject* obj,
      const char* material) {
    struct WavefrontObjectObject* o = getObject(obj);
    for (unsigned int i = 0; i < o->materialCount; i++) {
        if (strcmp(material, o->materials[i]) == 0) {
            return obj->currentMaterial = i;
        }
    }

    o->materials = (char **) realloc(
        o->materials,
        ++o->materialCount * sizeof(char*));
    o->materials[o->materialCount - 1] = strCopy(material);

    return obj->currentMaterial = o->materialCount - 1;
}

int wavefrontObjectAddObject(
      struct WavefrontObject* obj,
      const char* name) {
    for (unsigned int i = 0; i < obj->objectCount; i++) {
        if(strcmp(name, obj->objects[i].name) == 0) {
            obj->currentObject = i;
            return i;
        }
    }

    obj->currentObject = obj->objectCount++;
    struct WavefrontObjectObject* temp;
    temp = (struct WavefrontObjectObject*)realloc(
        obj->objects,
        obj->objectCount * sizeof(struct WavefrontObjectObject));
    /* TODO: Handle realloc failure.
    if (temp == NULL) {
        obj->objectCount--;
        return -1; // Failed to resize buffer.
    }*/
    obj->objects = temp;

    struct WavefrontObjectObject* o = obj->objects + obj->currentObject;
    o->name = strCopy(name);
    o->faces = NULL;
    o->materials = NULL;
    o->faceCount = 0;
    o->materialCount = 0;
    return obj->currentObject;
}