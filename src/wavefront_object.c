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

        free(o.vertices);
        free(o.unwraps);
        free(o.normals);
        o.vertexCount = 0;
        o.unwrapCount = 0;
        o.normalCount = 0;
    }
    obj->objectCount = 0;

    free(obj);
}

void wavefrontObjectAddVertex(
      struct WavefrontObject* obj,
      struct WavefrontObjectVertex* vertex) {
    struct WavefrontObjectObject* o = getObject(obj);
    o->vertices = (struct WavefrontObjectVertex*) realloc(
        o->vertices,
        ++o->vertexCount * sizeof(struct WavefrontObjectVertex));
    o->vertices[o->vertexCount - 1] = *vertex;
}

void wavefrontObjectAddUnwrap(
      struct WavefrontObject* obj,
      struct WavefrontObjectUnwrap* unwrap) {
    struct WavefrontObjectObject* o = getObject(obj);
    o->unwraps = (struct WavefrontObjectUnwrap*) realloc(
        o->unwraps,
        ++o->unwrapCount * sizeof(struct WavefrontObjectUnwrap));
    o->unwraps[o->unwrapCount - 1] = *unwrap;
}

void wavefrontObjectAddNormal(
      struct WavefrontObject* obj,
      struct WavefrontObjectNormal* normal) {
    struct WavefrontObjectObject* o = getObject(obj);
    o->normals = (struct WavefrontObjectNormal*) realloc(
        o->normals,
        ++o->normalCount * sizeof(struct WavefrontObjectNormal));
    o->normals[o->normalCount - 1] = *normal;
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
      char* materialLibrary) {
    obj->materialLibraries = (char **) realloc(obj->materialLibraries,
        ++obj->materialLibraryCount * sizeof(char*));
    obj->materialLibraries[obj->materialLibraryCount - 1] = strCopy(materialLibrary);
}

int wavefrontObjectAddMaterial(
      struct WavefrontObject* obj,
      char* material) {
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
      char* name) {
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
    o->vertices = NULL;
    o->unwraps = NULL;
    o->normals = NULL;
    o->faces = NULL;
    o->materials = NULL;
    o->vertexCount = 0;
    o->unwrapCount = 0;
    o->normalCount = 0;
    o->faceCount = 0;
    o->materialCount = 0;
    return obj->currentObject;
}