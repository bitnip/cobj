#include <stddef.h>
#include "wavefront_object_parser.h"
#include <string.h>
#include "cutil/src/assertion.h"

/* Private Functions */
int parsePoint(struct WavefrontObjectPoint* point, char* input);
int parseFace(struct WavefrontObjectFace* face, char* line);
int parseVertex(struct WavefrontObjectVertex* vertex, char* input);
int parseUnwrap(struct WavefrontObjectUnwrap* unwrap, char* line);
int parseNormal(struct WavefrontObjectNormal* normal, char* line);
int parseLine(struct WavefrontObject* obj, char* line);
int parseMaterialLibrary(struct WavefrontObject* obj, char* line);
void wavefrontObjectFaceFree(struct WavefrontObjectFace* face);

/* Wavefront Obj Point Test Cases */
void canParseEmptyString()
{
    char input[] = "";
    struct WavefrontObjectPoint point;
    int result = parsePoint(&point, input);
    assertFalse(result);
}

void canParseVertexOnlyPoint()
{
    char input[] = "10";
    struct WavefrontObjectPoint point;
    int result = parsePoint(&point, input);
    assertTrue(result);
    assertIntegersEqual(point.v, 10);
    assertIntegersEqual(point.vt, 0);
    assertIntegersEqual(point.vn, 0);
}

void canParseVertexAndTextureOnlyPoint()
{
    char input[] = "10/9";
    struct WavefrontObjectPoint point;
    int result = parsePoint(&point, input);
    assertTrue(result);
    assertIntegersEqual(point.v, 10);
    assertIntegersEqual(point.vt, 9);
    assertIntegersEqual(point.vn, 0);
}

void canParseVertexTextureNormalPoint()
{
    char input[] = "10/9/8";
    struct WavefrontObjectPoint point;
    int result = parsePoint(&point, input);
    assertTrue(result);
    assertIntegersEqual(point.v, 10);
    assertIntegersEqual(point.vt, 9);
    assertIntegersEqual(point.vn, 8);
}

void canParseVertexNormalPoint()
{
    char input[] = "10//8";
    struct WavefrontObjectPoint point;
    int result = parsePoint(&point, input);
    assertTrue(result);
    assertIntegersEqual(point.v, 10);
    assertIntegersEqual(point.vt, 0);
    assertIntegersEqual(point.vn, 8);
}

void garbageAfterVertexIndexFails()
{
    char input[] = "10j/9/8";
    struct WavefrontObjectPoint point;
    int result = parsePoint(&point, input);
    assertFalse(result);
}

void garbageAfterTextureIndexFails()
{
    char input[] = "10/9j/8";
    struct WavefrontObjectPoint point;
    int result = parsePoint(&point, input);
    assertFalse(result);
}

void garbageAfterNormalIndexFails()
{
    char input[] = "10/9/8j";
    struct WavefrontObjectPoint point;
    int result = parsePoint(&point, input);
    assertFalse(result);
}

void missingVertexFails()
{
    char input[] = "/1/10";
    struct WavefrontObjectPoint point;
    int result = parsePoint(&point, input);
    assertFalse(result);
}

void missingTextureFails()
{
    char input[] = "1/";
    struct WavefrontObjectPoint point;
    int result = parsePoint(&point, input);
    assertFalse(result);
}

void missingNormalFails()
{
    char input[] = "1/2/";
    struct WavefrontObjectPoint point;
    int result = parsePoint(&point, input);
    assertFalse(result);
}

/* Wavefront Obj Face Test Cases */
void canParseGenericFaceWithSpaces()
{
    char input[] = "f 1/2/3 4/5/6 7/8/9";
    struct WavefrontObjectFace face;
    face.points = NULL;
    face.pointCount = 0;
    int result = parseFace(&face, input);
    assertTrue(result);
    assertIntegersEqual(face.pointCount, 3);
    wavefrontObjectFaceFree(&face);
}

void canParseGenericFaceWithTabs()
{
    char input[] = "f\t1/2/3\t4/5/6\t7/8/9";
    struct WavefrontObjectFace face;
    face.points = NULL;
    face.pointCount = 0;
    int result = parseFace(&face, input);
    assertTrue(result);
    assertIntegersEqual(face.pointCount, 3);
    wavefrontObjectFaceFree(&face);
}

void faceFailToParseEmptyString()
{
    char input[] = "";
    struct WavefrontObjectFace face;
    face.points = NULL;
    face.pointCount = 0;
    int result = parseFace(&face, input);
    assertFalse(result);
    assertIntegersEqual(face.pointCount, 0);
}

void faceFailWithMissingF()
{
    char input[] = "\t1/2/3\t4/5/6\t7/8/9";
    struct WavefrontObjectFace face;
    face.points = NULL;
    face.pointCount = 0;
    int result = parseFace(&face, input);
    assertFalse(result);
    assertIntegersEqual(face.pointCount, 0);
    wavefrontObjectFaceFree(&face);
}

void faceMissingPoints()
{
    char input[] = "f";
    struct WavefrontObjectFace face;
    face.points = NULL;
    face.pointCount = 0;
    int result = parseFace(&face, input);
    assertTrue(result);
    assertIntegersEqual(face.pointCount, 0);
    wavefrontObjectFaceFree(&face);
}

/* Wavefront Obj Vertex Test Cases */
void vertexParseFourCoordinates()
{
    char input[] = "v 1.234 0.123 0.321 0.5";
    struct WavefrontObjectVertex vertex;
    vertex.x = 0.0;
    vertex.y = 0.0;
    vertex.z = 0.0;
    vertex.w = 0.0;
    int result = parseVertex(&vertex, input);
    assertTrue(result);
    assertFloatsEqual(vertex.w, 0.5);
    assertFloatsEqual(vertex.x, 1.234);
    assertFloatsEqual(vertex.y, 0.123);
    assertFloatsEqual(vertex.z, 0.321);
}

void vertexParseThreeCoordinates()
{
    char input[] = "v 1.234 0.123 0.321";
    struct WavefrontObjectVertex vertex;
    vertex.x = 0.0;
    vertex.y = 0.0;
    vertex.z = 0.0;
    vertex.w = 0.0;
    int result = parseVertex(&vertex, input);
    assertTrue(result);
    assertFloatsEqual(vertex.w, 1.000);
    assertFloatsEqual(vertex.x, 1.234);
    assertFloatsEqual(vertex.y, 0.123);
    assertFloatsEqual(vertex.z, 0.321);
}

void vertexParseFailsTwoCoordinates()
{
    char input[] = "v 1.234 0.123";
    struct WavefrontObjectVertex vertex;
    vertex.x = 0.0;
    vertex.y = 0.0;
    vertex.z = 0.0;
    vertex.w = 0.0;
    int result = parseVertex(&vertex, input);
    assertFalse(result);
}

void vertexParseFailsOneCoordinate()
{
    char input[] = "v 1.234";
    struct WavefrontObjectVertex vertex;
    vertex.x = 0.0;
    vertex.y = 0.0;
    vertex.z = 0.0;
    vertex.w = 0.0;
    int result = parseVertex(&vertex, input);
    assertFalse(result);
}

void vertexParseFailsNoCoordinates()
{
    char input[] = "v";
    struct WavefrontObjectVertex vertex;
    vertex.x = 0.0;
    vertex.y = 0.0;
    vertex.z = 0.0;
    vertex.w = 0.0;
    int result = parseVertex(&vertex, input);
    assertFalse(result);
}

void vertexParseFailsEmptyString()
{
    char input[] = "";
    struct WavefrontObjectVertex vertex;
    vertex.x = 0.0;
    vertex.y = 0.0;
    vertex.z = 0.0;
    vertex.w = 0.0;
    int result = parseVertex(&vertex, input);
    assertFalse(result);
}

/* Wavefront Obj Unwrap Test Cases */
void unwrapParseThreeCoordinates()
{
    char input[] = "vt 1.234 0.123 0.321";
    struct WavefrontObjectUnwrap unwrap;
    unwrap.u = 0.0;
    unwrap.v = 0.0;
    unwrap.w = 0.0;
    int result = parseUnwrap(&unwrap, input);
    assertTrue(result);
    assertFloatsEqual(unwrap.u, 1.234);
    assertFloatsEqual(unwrap.v, 0.123);
    assertFloatsEqual(unwrap.w, 0.321);
}

void unwrapParseTwoCoordinates()
{
    char input[] = "vt 1.234 0.123";
    struct WavefrontObjectUnwrap unwrap;
    unwrap.u = 0.0;
    unwrap.v = 0.0;
    unwrap.w = 0.0;
    int result = parseUnwrap(&unwrap, input);
    assertTrue(result);
}

void unwrapParseFailsOneCoordinate()
{
    char input[] = "vt 1.234";
    struct WavefrontObjectUnwrap unwrap;
    unwrap.u = 0.0;
    unwrap.v = 0.0;
    unwrap.w = 0.0;
    int result = parseUnwrap(&unwrap, input);
    assertFalse(result);
}

void unwrapParseFailsNoCoordinates()
{
    char input[] = "vt";
    struct WavefrontObjectUnwrap unwrap;
    unwrap.u = 0.0;
    unwrap.v = 0.0;
    unwrap.w = 0.0;
    int result = parseUnwrap(&unwrap, input);
    assertFalse(result);
}

void unwrapParseFailsEmptyString()
{
    char input[] = "";
    struct WavefrontObjectUnwrap unwrap;
    unwrap.u = 0.0;
    unwrap.v = 0.0;
    unwrap.w = 0.0;
    int result = parseUnwrap(&unwrap, input);
    assertFalse(result);
}

/* Wavefront Obj Normal Test Cases */
void normalParseThreeCoordinates()
{
    char input[] = "vn 1.234 0.123 0.321";
    struct WavefrontObjectNormal normal;
    normal.x = 0.0;
    normal.y = 0.0;
    normal.z = 0.0;
    int result = parseNormal(&normal, input);
    assertTrue(result);
    assertFloatsEqual(normal.x, 1.234);
    assertFloatsEqual(normal.y, 0.123);
    assertFloatsEqual(normal.z, 0.321);
}

void normalParseFailsTwoCoordinates()
{
    char input[] = "vn 1.234 0.123";
    struct WavefrontObjectNormal normal;
    normal.x = 0.0;
    normal.y = 0.0;
    normal.z = 0.0;
    int result = parseNormal(&normal, input);
    assertFalse(result);
}

void normalParseFailsOneCoordinate()
{
    char input[] = "vn 1.234";
    struct WavefrontObjectNormal normal;
    normal.x = 0.0;
    normal.y = 0.0;
    normal.z = 0.0;
    int result = parseNormal(&normal, input);
    assertFalse(result);
}

void normalParseFailsNoCoordinates()
{
    char input[] = "vn";
    struct WavefrontObjectNormal normal;
    normal.x = 0.0;
    normal.y = 0.0;
    normal.z = 0.0;
    int result = parseNormal(&normal, input);
    assertFalse(result);
}

void normalParseFailsEmptyString()
{
    char input[] = "";
    struct WavefrontObjectNormal normal;
    normal.x = 0.0;
    normal.y = 0.0;
    normal.z = 0.0;
    int result = parseNormal(&normal, input);
    assertFalse(result);
}

/* Wavefront Obj Material Library Test Cases */

void materialLibraryParseFailsEmptyString()
{
    char input[] = "";
    struct WavefrontObject* obj = wavefrontObjectCreate();

    int result = parseMaterialLibrary(obj, input);
    assertFalse(result);

    wavefrontObjectFree(obj);
}

void materialLibraryParseFailsMissingSeparator()
{
    char input[] = "mtlliblib.mtl";
    struct WavefrontObject* obj = wavefrontObjectCreate();

    int result = parseMaterialLibrary(obj, input);
    assertFalse(result);

    wavefrontObjectFree(obj);
}

void materialLibraryParsePassesNoLibraries()
{
    char input[] = "mtllib";
    struct WavefrontObject* obj = wavefrontObjectCreate();

    int result = parseMaterialLibrary(obj, input);
    assertTrue(result);

    wavefrontObjectFree(obj);
}

void materialLibraryParsePassesOneLibrary()
{
    char input[] = "mtllib lib.mtl";
    struct WavefrontObject* obj = wavefrontObjectCreate();

    int result = parseMaterialLibrary(obj, input);
    assertTrue(result);
    assertIntegersEqual(obj->materialLibraryCount, 1);
    assertStringsEqual(obj->materialLibraries[0], "lib.mtl");

    wavefrontObjectFree(obj);
}

void materialLibraryParsePassesMultipleLibraries()
{
    char input[] = "mtllib lib1.mtl lib2.mtl lib3.mtl";
    struct WavefrontObject* obj = wavefrontObjectCreate();

    int result = parseMaterialLibrary(obj, input);
    assertTrue(result);
    assertIntegersEqual(obj->materialLibraryCount, 3);
    assertStringsEqual(obj->materialLibraries[0], "lib1.mtl");
    assertStringsEqual(obj->materialLibraries[1], "lib2.mtl");
    assertStringsEqual(obj->materialLibraries[2], "lib3.mtl");

    wavefrontObjectFree(obj);
}

/* Wavefront Obj Parse Line Test Cases */
void parseLineIgnoresComments()
{
    char input[] = "#Line Comment";
    struct WavefrontObject* obj = wavefrontObjectCreate();

    int result = parseLine(obj, input);
    assertTrue(result);
    assertIntegersEqual(obj->objectCount, 0);

    wavefrontObjectFree(obj);
}

void parseLineParsesVertex()
{
    char input[] = "v 1.234 0.123 0.321";
    struct WavefrontObject* obj = wavefrontObjectCreate();

    int result = parseLine(obj, input);
    assertTrue(result);
    assertIntegersEqual(obj->objectCount, 0);
    assertIntegersEqual(obj->vertexCount, 1);
    assertIntegersEqual(obj->unwrapCount, 0);
    assertIntegersEqual(obj->normalCount, 0);
    //struct WavefrontObjectObject* o = obj->objects;
    //assertIntegersEqual(o->faceCount, 0);

    wavefrontObjectFree(obj);
}

void parseLineParsesUnwrap()
{
    char input[] = "vt 1.234 0.123 0.321";
    struct WavefrontObject* obj = wavefrontObjectCreate();

    int result = parseLine(obj, input);
    assertTrue(result);
    assertIntegersEqual(obj->objectCount, 0);
    assertIntegersEqual(obj->vertexCount, 0);
    assertIntegersEqual(obj->unwrapCount, 1);
    assertIntegersEqual(obj->normalCount, 0);
    //struct WavefrontObjectObject* o = obj->objects;
    //assertIntegersEqual(o->faceCount, 0);

    wavefrontObjectFree(obj);
}

void parseLineParsesNormal()
{
    char input[] = "vn 1.234 0.123 0.321";
    struct WavefrontObject* obj = wavefrontObjectCreate();

    int result = parseLine(obj, input);
    assertTrue(result);
    assertIntegersEqual(obj->objectCount, 0);
    assertIntegersEqual(obj->vertexCount, 0);
    assertIntegersEqual(obj->unwrapCount, 0);
    assertIntegersEqual(obj->normalCount, 1);
    //struct WavefrontObjectObject* o = obj->objects;
    //assertIntegersEqual(o->faceCount, 0);

    wavefrontObjectFree(obj);
}

void parseLineParsesFace()
{
    char input[] = "f 1/2/3 4/5/6 7/8/9";
    struct WavefrontObject* obj = wavefrontObjectCreate();

    int result = parseLine(obj, input);
    assertTrue(result);
    assertIntegersEqual(obj->objectCount, 1);
    struct WavefrontObjectObject* o = obj->objects;
    assertIntegersEqual(obj->vertexCount, 0);
    assertIntegersEqual(obj->unwrapCount, 0);
    assertIntegersEqual(obj->normalCount, 0);
    assertIntegersEqual(o->faceCount, 1);

    wavefrontObjectFree(obj);
}

/* Wavefront Obj Parse object test cases */

void parseObjectTest() {
    char input[] = "\
    o test_object\n";
    struct WavefrontObject* obj = parseWavefrontObjectFromString(input);
    assertIntegersEqual(obj->objectCount, 1);
    struct WavefrontObjectObject* o = obj->objects;
    assertStringsEqual(o->name, "test_object");
    wavefrontObjectFree(obj);

    char input2[] = "\
    o test_object\n\
    o test_object\n";
    obj = parseWavefrontObjectFromString(input2);
    assertIntegersEqual(obj->objectCount, 1);
    o = obj->objects;
    assertStringsEqual(o->name, "test_object");
    wavefrontObjectFree(obj);

    char input3[] = "\
    o test_object1\n\
    o test_object2\n";
    obj = parseWavefrontObjectFromString(input3);
    assertIntegersEqual(obj->objectCount, 2);
    o = obj->objects;
    assertStringsEqual(o->name, "test_object1");
    wavefrontObjectFree(obj);
}

/* Wavefront Obj Parse usemtl Test Cases */

void parseUseMaterialTest() {
    char input[] = "\
    usemtl test_material\n";
    struct WavefrontObject* obj = parseWavefrontObjectFromString(input);
    assertIntegersEqual(obj->objectCount, 1);
    struct WavefrontObjectObject* o = obj->objects;
    assertIntegersEqual(o->materialCount, 1);
    assertStringsEqual(o->materials[0], "test_material");
    wavefrontObjectFree(obj);

    char input2[] = "\
    usemtl test_material\n\
    usemtl test_material\n";
    obj = parseWavefrontObjectFromString(input2);
    assertIntegersEqual(obj->objectCount, 1);
    o = obj->objects;
    assertIntegersEqual(o->materialCount, 1);
    assertStringsEqual(o->materials[0], "test_material");
    wavefrontObjectFree(obj);

    char input3[] = "\
    usemtl test_material1\n\
    usemtl test_material2\n";
    obj = parseWavefrontObjectFromString(input3);
    assertIntegersEqual(obj->objectCount, 1);
    o = obj->objects;
    assertIntegersEqual(o->materialCount, 2);
    assertStringsEqual(o->materials[0], "test_material1");
    assertStringsEqual(o->materials[1], "test_material2");
    wavefrontObjectFree(obj);

}

/* Wavefront Obj Parse From String Test Cases */

void normalWavefrontObjectFromString()
{
    char input[] = "\
    o test_object\n\
    v 1.00 2.00 3.00\n\
    v 4.00 5.00 6.00\n\
    v 7.00 8.00 9.00\n\
    vt 0.1 0.2 0.3\n\
    vt 0.4 0.5 0.6\n\
    vt 0.7 0.8 0.9\n\
    vn 0.1 0.2 0.3\n\
    vn 0.4 0.5 0.6\n\
    vn 0.7 0.8 0.9\n\
    usemtl test_material\n\
    f 1/1/1 2/2/2 3/3/3\n";

    struct WavefrontObject* obj = parseWavefrontObjectFromString(input);

    assertIntegersEqual(obj->objectCount, 1);
    struct WavefrontObjectObject* o = obj->objects;

    assertStringsEqual(o->name, "test_object");

    assertIntegersEqual(obj->vertexCount, 3);
    assertFloatsEqual(obj->vertices[0].x, 1.00);
    assertFloatsEqual(obj->vertices[0].y, 2.00);
    assertFloatsEqual(obj->vertices[0].z, 3.00);
    assertFloatsEqual(obj->vertices[1].x, 4.00);
    assertFloatsEqual(obj->vertices[1].y, 5.00);
    assertFloatsEqual(obj->vertices[1].z, 6.00);
    assertFloatsEqual(obj->vertices[2].x, 7.00);
    assertFloatsEqual(obj->vertices[2].y, 8.00);
    assertFloatsEqual(obj->vertices[2].z, 9.00);

    assertIntegersEqual(obj->unwrapCount, 3);
    assertFloatsEqual(obj->unwraps[0].u, 0.1);
    assertFloatsEqual(obj->unwraps[0].v, 0.2);
    assertFloatsEqual(obj->unwraps[0].w, 0.3);
    assertFloatsEqual(obj->unwraps[1].u, 0.4);
    assertFloatsEqual(obj->unwraps[1].v, 0.5);
    assertFloatsEqual(obj->unwraps[1].w, 0.6);
    assertFloatsEqual(obj->unwraps[2].u, 0.7);
    assertFloatsEqual(obj->unwraps[2].v, 0.8);
    assertFloatsEqual(obj->unwraps[2].w, 0.9);

    assertIntegersEqual(obj->normalCount, 3);
    assertFloatsEqual(obj->normals[0].x, 0.1);
    assertFloatsEqual(obj->normals[0].y, 0.2);
    assertFloatsEqual(obj->normals[0].z, 0.3);
    assertFloatsEqual(obj->normals[1].x, 0.4);
    assertFloatsEqual(obj->normals[1].y, 0.5);
    assertFloatsEqual(obj->normals[1].z, 0.6);
    assertFloatsEqual(obj->normals[2].x, 0.7);
    assertFloatsEqual(obj->normals[2].y, 0.8);
    assertFloatsEqual(obj->normals[2].z, 0.9);

    assertIntegersEqual(o->faceCount, 1);
    assertIntegersEqual(o->materialCount, 1);
    assertStringsEqual(o->materials[0], "test_material");

    wavefrontObjectFree(obj);
}

void wavefrontObjectParserTest()
{
    canParseEmptyString();
    canParseVertexOnlyPoint();
    canParseVertexAndTextureOnlyPoint();
    canParseVertexTextureNormalPoint();
    canParseVertexNormalPoint();
    garbageAfterVertexIndexFails();
    garbageAfterTextureIndexFails();
    garbageAfterNormalIndexFails();
    missingVertexFails();
    missingTextureFails();
    missingNormalFails();
    canParseGenericFaceWithSpaces();
    canParseGenericFaceWithTabs();
    faceFailToParseEmptyString();
    faceFailWithMissingF();
    faceMissingPoints();


    vertexParseFourCoordinates();
    vertexParseThreeCoordinates();
    vertexParseFailsTwoCoordinates();
    vertexParseFailsOneCoordinate();
    vertexParseFailsNoCoordinates();
    vertexParseFailsEmptyString();

    unwrapParseThreeCoordinates();
    unwrapParseTwoCoordinates();
    unwrapParseFailsOneCoordinate();
    unwrapParseFailsNoCoordinates();
    unwrapParseFailsEmptyString();

    normalParseThreeCoordinates();
    normalParseFailsTwoCoordinates();
    normalParseFailsOneCoordinate();
    normalParseFailsNoCoordinates();
    normalParseFailsEmptyString();

    materialLibraryParseFailsEmptyString();
    materialLibraryParseFailsMissingSeparator();
    materialLibraryParsePassesNoLibraries();
    materialLibraryParsePassesOneLibrary();
    materialLibraryParsePassesMultipleLibraries();

    parseLineIgnoresComments();
    parseLineParsesVertex();
    parseLineParsesUnwrap();
    parseLineParsesNormal();
    parseLineParsesFace();

    parseObjectTest();
    parseUseMaterialTest();

    normalWavefrontObjectFromString();
}
