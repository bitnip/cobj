#include "wavefront_object_parser.h"
#include "cutil/src/error.h"
#include "cutil/src/assertion.h"

void canParseEmptyString() {
    char input[] = "";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    wavefrontObjectRelease(&wObj);
}

/* Wavefront Obj Point Test Cases */

void canParseLine() {
    char input[] = "l 1 2 3";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(wObj.objectCount, 1);
    assertIntegersEqual(wObj.objects->faces->points->v, 1);
    assertIntegersEqual(wObj.objects->faces->points->vt, 0);
    assertIntegersEqual(wObj.objects->faces->points->vn, 0);
    wavefrontObjectRelease(&wObj);
}

void canParseVertexOnlyPoint() {
    char input[] = "f 10 10 10";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(wObj.objectCount, 1);
    assertIntegersEqual(wObj.objects->faces->points->v, 10);
    assertIntegersEqual(wObj.objects->faces->points->vt, 0);
    assertIntegersEqual(wObj.objects->faces->points->vn, 0);
    wavefrontObjectRelease(&wObj);
}

void canParseVertexAndTextureOnlyPoint() {
    char input[] = "f 10/9 10/9 10/9";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(wObj.objectCount, 1);
    assertIntegersEqual(wObj.objects->faces->points->v, 10);
    assertIntegersEqual(wObj.objects->faces->points->vt, 9);
    assertIntegersEqual(wObj.objects->faces->points->vn, 0);
    wavefrontObjectRelease(&wObj);
}

void canParseVertexTextureNormalPoint() {
    char input[] = "f 10/9/8 10/9/8 10/9/8";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(wObj.objects->faces->points->v, 10);
    assertIntegersEqual(wObj.objects->faces->points->vt, 9);
    assertIntegersEqual(wObj.objects->faces->points->vn, 8);
    wavefrontObjectRelease(&wObj);
}

void canParseVertexNormalPoint() {
    char input[] = " f 10//8 10//8 10//8";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(wObj.objectCount, 1);
    assertIntegersEqual(wObj.objects->faces->points->v, 10);
    assertIntegersEqual(wObj.objects->faces->points->vt, 0);
    assertIntegersEqual(wObj.objects->faces->points->vn, 8);
    wavefrontObjectRelease(&wObj);
}

void garbageAfterVertexIndexFails() {
    char input[] = "f 10j/9/8 10j/9/8 10j/9/8";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

void garbageAfterTextureIndexFails() {
    char input[] = "f 10/9j/8 10/9j/8 10/9j/8";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

void garbageAfterNormalIndexFails() {
    char input[] = "f 10/9/8j 10/9/8j 10/9/8j";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

void missingVertexFails() {
    char input[] = "f /1/10 /1/10 /1/10";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

void missingTextureFails() {
    char input[] = "f 1/ 1/ 1/";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

void missingNormalFails() {
    char input[] = "f 1/2/ 1/2/ 1/2/";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

/* Wavefront Obj Face Test Cases */
void canParseGenericFaceWithSpaces() {
    char input[] = "f 1/2/3 4/5/6 7/8/9";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    struct WavefrontObjectObject *obj = wObj.objects;
    struct WavefrontObjectFace *face = obj->faces;
    assertIntegersEqual(face->pointCount, 3);
    wavefrontObjectRelease(&wObj);
}

void canParseGenericFaceWithTabs() {
    char input[] = "f\t1/2/3\t4/5/6\t7/8/9";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    struct WavefrontObjectObject *obj = wObj.objects;
    struct WavefrontObjectFace *face = obj->faces;
    assertIntegersEqual(face->pointCount, 3);
    wavefrontObjectRelease(&wObj);
}

void faceFailToParseEmptyString() {
    char input[] = "f";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

void faceMissingPoints() {
    char input[] = "f ";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

/* Wavefront Obj Vertex Test Cases */
void vertexParseFourCoordinates() {
    char input[] = "v 1.234 0.123 0.321 0.5";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);

    assertFloatsEqual(wObj.vertices->w, 0.5);
    assertFloatsEqual(wObj.vertices->x, 1.234);
    assertFloatsEqual(wObj.vertices->y, 0.123);
    assertFloatsEqual(wObj.vertices->z, 0.321);
    
    wavefrontObjectRelease(&wObj);
}

void vertexParseThreeCoordinates() {
    char input[] = "v 1.234 0.123 0.321";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);

    assertFloatsEqual(wObj.vertices->w, 1.000);
    assertFloatsEqual(wObj.vertices->x, 1.234);
    assertFloatsEqual(wObj.vertices->y, 0.123);
    assertFloatsEqual(wObj.vertices->z, 0.321);

    wavefrontObjectRelease(&wObj);
}

void vertexParseFailsTwoCoordinates() {
    char input[] = "v 1.234 0.123";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

void vertexParseFailsOneCoordinate() {
    char input[] = "v 1.234";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

void vertexParseFailsNoCoordinates() {
    char input[] = "v ";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

void vertexParseFailsEmptyString() {
    char input[] = "v";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

/* Wavefront Obj Unwrap Test Cases */
void unwrapParseThreeCoordinates() {
    char input[] = "vt 1.234 0.123 0.321";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    assertFloatsEqual(wObj.unwraps->u, 1.234);
    assertFloatsEqual(wObj.unwraps->v, 0.123);
    assertFloatsEqual(wObj.unwraps->w, 0.321);
    wavefrontObjectRelease(&wObj);
}

void unwrapParseTwoCoordinates() {
    char input[] = "vt 1.234 0.123";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    wavefrontObjectRelease(&wObj);
}

void unwrapParseFailsOneCoordinate() {
    char input[] = "vt 1.234";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

void unwrapParseFailsNoCoordinates() {
    char input[] = "vt ";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

void unwrapParseFailsEmptyString() {
    char input[] = "vt";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

/* Wavefront Obj Normal Test Cases */
void normalParseThreeCoordinates() {
    char input[] = "vn 1.234 0.123 0.321";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    assertFloatsEqual(wObj.normals->x, 1.234);
    assertFloatsEqual(wObj.normals->y, 0.123);
    assertFloatsEqual(wObj.normals->z, 0.321);
    wavefrontObjectRelease(&wObj);
}

void normalParseFailsTwoCoordinates() {
    char input[] = "vn 1.234 0.123";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

void normalParseFailsOneCoordinate() {
    char input[] = "vn 1.234";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

void normalParseFailsNoCoordinates() {
    char input[] = "vn ";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

void normalParseFailsEmptyString() {
    char input[] = "vn";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

/* Wavefront Obj Material Library Test Cases */

void materialLibraryParsePassesEmptyString() {
    char input[] = "mtllib";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(wObj.materialLibraryCount, 0);
}

void materialLibraryParsePassesNoLibraries() {
    char input[] = "mtllib ";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(wObj.materialLibraryCount, 0);
    wavefrontObjectRelease(&wObj);
}

void materialLibraryParsePassesOneLibrary() {
    char input[] = "mtllib lib.mtl";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(wObj.materialLibraryCount, 1);
    assertStringsEqual(wObj.materialLibraries[0], "lib.mtl");
    wavefrontObjectRelease(&wObj);
}

void materialLibraryParsePassesMultipleLibraries() {
    char input[] = "mtllib lib1.mtl lib2.mtl lib3.mtl";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(wObj.materialLibraryCount, 3);
    assertStringsEqual(wObj.materialLibraries[0], "lib1.mtl");
    assertStringsEqual(wObj.materialLibraries[1], "lib2.mtl");
    assertStringsEqual(wObj.materialLibraries[2], "lib3.mtl");
    wavefrontObjectRelease(&wObj);
}

/* Wavefront Obj Parse Line Test Cases */

void parseLineIgnoresComments() {
    char input[] = "#Line Comment";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(wObj.objectCount, 0);
    wavefrontObjectRelease(&wObj);
}

void parseLineParsesVertex() {
    char input[] = "v 1.234 0.123 0.321";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(wObj.objectCount, 0);
    assertIntegersEqual(wObj.vertexCount, 1);
    assertIntegersEqual(wObj.unwrapCount, 0);
    assertIntegersEqual(wObj.normalCount, 0);
    wavefrontObjectRelease(&wObj);
}

void parseLineParsesUnwrap() {
    char input[] = "vt 1.234 0.123 0.321";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(wObj.objectCount, 0);
    assertIntegersEqual(wObj.vertexCount, 0);
    assertIntegersEqual(wObj.unwrapCount, 1);
    assertIntegersEqual(wObj.normalCount, 0);
    wavefrontObjectRelease(&wObj);
}

void parseLineParsesNormal() {
    char input[] = "vn 1.234 0.123 0.321";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(wObj.objectCount, 0);
    assertIntegersEqual(wObj.vertexCount, 0);
    assertIntegersEqual(wObj.unwrapCount, 0);
    assertIntegersEqual(wObj.normalCount, 1);
    wavefrontObjectRelease(&wObj);
}

void parseLineParsesFace() {
    char input[] = "f 1/2/3 4/5/6 7/8/9";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(wObj.objectCount, 1);
    struct WavefrontObjectObject *o = wObj.objects;
    assertIntegersEqual(wObj.vertexCount, 0);
    assertIntegersEqual(wObj.unwrapCount, 0);
    assertIntegersEqual(wObj.normalCount, 0);
    assertIntegersEqual(o->faceCount, 1);
    wavefrontObjectRelease(&wObj);
}

/* Wavefront Obj Parse object test cases */

void parseObjectTest() {
    char input[] = "\
    o test_object\n";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(wObj.objectCount, 1);
    struct WavefrontObjectObject *o = wObj.objects;
    assertStringsEqual(o->name, "test_object");
    wavefrontObjectRelease(&wObj);

    char input2[] = "\
    o test_object\n\
    o test_object\n";
    result = parseWavefrontObjectFromString(&wObj, input2);
    assertIntegersEqual(result, STATUS_OK);
    o = wObj.objects;
    assertStringsEqual(o->name, "test_object");
    wavefrontObjectRelease(&wObj);

    char input3[] = "\
    o test_object1\n\
    o test_object2\n";
    result = parseWavefrontObjectFromString(&wObj, input3);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(wObj.objectCount, 2);
    o = wObj.objects;
    assertStringsEqual(o->name, "test_object1");
    wavefrontObjectRelease(&wObj);
}

/* Wavefront Obj Parse usemtl Test Cases */

void parseUseMaterialTest() {
    char input[] = "\
    usemtl test_material\n";
    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(wObj.materialCount, 1);
    assertStringsEqual(wObj.materials[0], "test_material");
    wavefrontObjectRelease(&wObj);

    char input2[] = "\
    usemtl test_material\n\
    usemtl test_material\n";
    result = parseWavefrontObjectFromString(&wObj, input2);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(wObj.materialCount, 1);
    assertStringsEqual(wObj.materials[0], "test_material");
    wavefrontObjectRelease(&wObj);

    char input3[] = "\
    usemtl test_material1\n\
    usemtl test_material2\n";
    result = parseWavefrontObjectFromString(&wObj, input3);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(wObj.materialCount, 2);
    assertStringsEqual(wObj.materials[0], "test_material1");
    assertStringsEqual(wObj.materials[1], "test_material2");
    wavefrontObjectRelease(&wObj);

}

/* Wavefront Obj Parse From String Test Cases */
void normalWavefrontObjectFromString() {
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

    struct WavefrontObject wObj;
    int result = parseWavefrontObjectFromString(&wObj, input);
    assertIntegersEqual(result, STATUS_OK);

    assertIntegersEqual(wObj.objectCount, 1);
    struct WavefrontObjectObject *o = wObj.objects;

    assertStringsEqual(o->name, "test_object");

    assertIntegersEqual(wObj.vertexCount, 3);
    assertFloatsEqual(wObj.vertices[0].x, 1.00);
    assertFloatsEqual(wObj.vertices[0].y, 2.00);
    assertFloatsEqual(wObj.vertices[0].z, 3.00);
    assertFloatsEqual(wObj.vertices[1].x, 4.00);
    assertFloatsEqual(wObj.vertices[1].y, 5.00);
    assertFloatsEqual(wObj.vertices[1].z, 6.00);
    assertFloatsEqual(wObj.vertices[2].x, 7.00);
    assertFloatsEqual(wObj.vertices[2].y, 8.00);
    assertFloatsEqual(wObj.vertices[2].z, 9.00);

    assertIntegersEqual(wObj.unwrapCount, 3);
    assertFloatsEqual(wObj.unwraps[0].u, 0.1);
    assertFloatsEqual(wObj.unwraps[0].v, 0.2);
    assertFloatsEqual(wObj.unwraps[0].w, 0.3);
    assertFloatsEqual(wObj.unwraps[1].u, 0.4);
    assertFloatsEqual(wObj.unwraps[1].v, 0.5);
    assertFloatsEqual(wObj.unwraps[1].w, 0.6);
    assertFloatsEqual(wObj.unwraps[2].u, 0.7);
    assertFloatsEqual(wObj.unwraps[2].v, 0.8);
    assertFloatsEqual(wObj.unwraps[2].w, 0.9);

    assertIntegersEqual(wObj.normalCount, 3);
    assertFloatsEqual(wObj.normals[0].x, 0.1);
    assertFloatsEqual(wObj.normals[0].y, 0.2);
    assertFloatsEqual(wObj.normals[0].z, 0.3);
    assertFloatsEqual(wObj.normals[1].x, 0.4);
    assertFloatsEqual(wObj.normals[1].y, 0.5);
    assertFloatsEqual(wObj.normals[1].z, 0.6);
    assertFloatsEqual(wObj.normals[2].x, 0.7);
    assertFloatsEqual(wObj.normals[2].y, 0.8);
    assertFloatsEqual(wObj.normals[2].z, 0.9);

    assertIntegersEqual(o->faceCount, 1);
    assertIntegersEqual(wObj.materialCount, 1);
    assertStringsEqual(wObj.materials[0], "test_material");

    wavefrontObjectRelease(&wObj);
}

void wavefrontObjectParserTest() {
    canParseEmptyString();
    canParseLine();
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

    materialLibraryParsePassesEmptyString();
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
