// GDSProcessor.h

#ifndef GDSPROCESSOR_H
#define GDSPROCESSOR_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include "lib/Triangulation.h"
#include "libGDSII.h"

using namespace std;
using namespace libGDSII;

// Struct definitions for vertices and triangles
struct Vertex2D {
    double x, y;
};
struct Vertex3D {
    double x, y, z;
};
struct Triangle { 
    int x, y, z;
};

typedef vector<Vertex2D> Polygon2D;
typedef vector<Vertex3D> Polygon3D;
typedef vector<Triangle> TriangleList; 

struct Element2D { 
    Polygon2D polygon2D;
    TriangleList triangles; 
    bool clockwise;
};
struct Element3D { 
    Polygon3D polygon3D;
    TriangleList triangles; 
    bool clockwise;
};

typedef vector<Element2D> ElementList2D;
typedef vector<Element3D> ElementList3D;

// Triangulation struct definitions
struct CustomPoint2D {
    double data[2];
};

struct CustomEdge {
    pair<int, int> vertices;
};

// Function declarations
GDSIIData* readGDS(const char* gdsFileName);
map<int, PolygonList> extractPolygons(GDSIIData* gdsIIData);
map<int, ElementList2D> layerMapToElementList(map<int, PolygonList>& layerMap);
bool checkClockwise(Polygon2D polygon);
void triangulatePolygons(map<int, ElementList2D>& layerMap);
Polygon3D insertZ(const Polygon2D& polygon2D, double z);
map<int, ElementList3D> extrudePolygons(map<int, ElementList2D>& layerMap, double zMin, double zMax);
void writePLY(const string& filename, const map<int, ElementList3D>& extrudedLayerMap, int layerNumber);

#endif // GDSPROCESSOR_H
