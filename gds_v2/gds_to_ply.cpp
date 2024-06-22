#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <algorithm>
#include "include/Triangulation.h"
#include "libGDSII.h"
#include "include/CDT.h"

using namespace std;
using namespace libGDSII;

struct Vertex2D {
    double x, y;
};
struct Vertex3D {
    double x, y, z;
};
struct Triangle { 
    int x, y, z;
};

typedef vector<Vertex2D> Polygon2D; // Polygon2D is a vector of vertices Vertex2D{x, y}
typedef vector<Vertex3D> Polygon3D;// Polygon3D is a vector of vertices Vertex3D{x, y, z}
typedef vector<Triangle> TriangleList; 

struct Element2D { // Element is a struct of the polygon, its triangulation, and whether it is clockwise
    Polygon2D polygon2D;
    TriangleList triangles; 
    bool clockwise;
};
struct Element3D { // Element is a struct of the polygon, its triangulation, and whether it is clockwise
    Polygon3D polygon3D;
    TriangleList triangles; 
    bool clockwise;
};
typedef vector<Element2D> ElementList2D;
typedef vector<Element3D> ElementList3D;

// Takes input GDS file and returns its data
GDSIIData* readGDS(const char* gdsFileName) {
    GDSIIData *gdsIIData = new GDSIIData(gdsFileName); 
    if (gdsIIData->ErrMsg) {
        printf("error: %s (aborting)\n", gdsIIData->ErrMsg->c_str());
        exit(1);
    }
    return gdsIIData;
}


/* Extracts polygons from GDSData. Returns a map from layer number to a polygon list. 
    layerMap = {
        6 : [[x1, y1, x2, y2, ... ], [x1, y1, x2, y2, ...], ...]
        42 : [[x1, y1, x2, y2, ... ], [x1, y1, x2, y2, ...], ...]
        ...
    }
where each element is a mapping from the layer number to a polygonList.
    polygonList = [[x1, y1, x2, y2, ... ], [x1, y1, x2, y2, ...], ...]
Each polygon in a polygonList is an array of points.
*/ 
map<int, PolygonList> extractPolygons(GDSIIData* gdsIIData) { 
    map<int, PolygonList> layerMap;
    vector<int> layers = gdsIIData->GetLayers();
    for (int nl = 0; nl < layers.size(); nl++) {
        PolygonList layerNLPolygons = gdsIIData->GetPolygons(layers[nl]);
        layerMap[layers[nl]] = layerNLPolygons;
    }
    return layerMap;
}

/* Converts polygons in layerMap to Vertex2D representation.
 * A layerMap is the following:
 * layerMap2D = {
 *  6 : [ Element1, Element2, ... ]
 * }
 * Each Element is a struct of {PolygonList2D, Triangles, clockwise}.
 * Element = {[V1, V2, ...], [T1, T2, ...], true/false}
 * */
map<int, ElementList2D> layerMapToElementList(map<int, PolygonList>& layerMap) {

    map<int, ElementList2D> layerMap2D;

    map<int, PolygonList>::iterator it;
    for (it = layerMap.begin(); it != layerMap.end(); it++) {
        int layerNumber = it->first;
        PolygonList polygons = it->second;
        ElementList2D elementList;
        for (const auto& polygon : polygons) {

            Polygon2D polygon2D;
            TriangleList triangles;
            bool clockwise = false;
            Element2D element;

            for (int i = 0; i < polygon.size(); i+=2) {
                double x = polygon[i];
                double y = polygon[i+1];
                polygon2D.push_back(Vertex2D{x, y});
            }
            element = Element2D{polygon2D, triangles, clockwise};
            elementList.push_back(element);
        }
        layerMap2D[layerNumber] = elementList;
    }

    return layerMap2D;
}

void printLayerMap(const map<int, ElementList2D>& layerMap) {
    cout << "layers = {" << endl;
    for (auto it = layerMap.begin(); it != layerMap.end(); ++it) {
        int layerNumber = it->first;
        const ElementList2D& elements = it->second;

        cout << "  " << layerNumber << " : [" << endl;
        for (const auto& element : elements) {
            cout << "    ([";

            // Output the polygon vertices
            for (size_t i = 0; i < element.polygon2D.size(); ++i) {
                const Vertex2D& vertex = element.polygon2D[i];
                cout << "[" << vertex.x << ", " << vertex.y << "]";
                if (i != element.polygon2D.size() - 1) {
                    cout << ", ";
                }
            }
            cout << "], ";

            // Output the triangles
            cout << "[";
            for (size_t i = 0; i < element.triangles.size(); ++i) {
                const Triangle& triplet = element.triangles[i];
                cout << "[" << triplet.x << ", " << triplet.y << ", " << triplet.z << "]";
                if (i != element.triangles.size() - 1) {
                    cout << ", ";
                }
            }
            cout << "], ";

            // Output the clockwise flag
            cout << (element.clockwise ? "true" : "false") << ")";
            if (&element != &elements.back()) {
                cout << ", ";
            }
            cout << endl;
        }
        cout << "  ]";
        if (next(it) != layerMap.end()) {
            cout << ", ";
        }
        cout << endl;
    }
    cout << "}" << endl;

}


/* Checks clockwise configuration of points in polygon using method linked here:
 * https://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order
 * Not sure if this is necessary for PLY files, but generally useful */
bool checkClockwise(Polygon2D polygon) {
    double area = 0;
    for (int i = 0; i < polygon.size(); i++) {
        Vertex2D v1 = polygon[i];
        Vertex2D v2 = polygon[(i+1) % polygon.size()];
        area += (v2.x - v1.x) * (v2.y + v1.y);
    }
    return area > 0;
}

struct CustomPoint2D
{
    double data[2];
};
 
struct CustomEdge
{
    pair<int, int> vertices;
};

/* Constrained Delaunay triangulation of polygons. Constraints are the outside edges of the polygon.*/
void triangulatePolygons(map<int, ElementList2D>& layerMap) {

    for (auto& layerPair : layerMap) {

        ElementList2D& elements = layerPair.second;
        for (Element2D& element : elements) {
            Polygon2D& polygon2D = element.polygon2D;
            TriangleList& triangles = element.triangles;

            if (checkClockwise(polygon2D)) {
                element.clockwise = true;
            }

            int totalPolygonPoints = polygon2D.size();

            // Convert vertices to CDT Points
            vector<CustomPoint2D> points; 
            for (const auto& vertex : polygon2D) {
                CustomPoint2D point;
                point.data[0] = vertex.x;
                point.data[1] = vertex.y;
                points.push_back(point);
            }

            vector<pair<int, int>> boundarySegments(totalPolygonPoints);

            // Create edges for the polygon
            for (int j = 0; j < totalPolygonPoints; j++) {
                boundarySegments[j] = make_pair(j, (j + 1) % totalPolygonPoints);
            }

            // Convert edges to Delaunay segments
            vector<CustomEdge> edges;
            for (const auto& edge : boundarySegments) {
                CustomEdge customEdge;
                customEdge.vertices.first = edge.first;
                customEdge.vertices.second = edge.second;
                edges.push_back(customEdge); 
            }

            CDT::Triangulation<double> cdt(CDT::VertexInsertionOrder::AsProvided);
            auto test = CDT::VertexInsertionOrder::AsProvided;
            cdt.insertVertices(
                points.begin(),
                points.end(),
                [](const CustomPoint2D& p) { return p.data[0]; },
                [](const CustomPoint2D& p) { return p.data[1]; }
            );

            cdt.insertEdges(
                edges.begin(),
                edges.end(),
                [](const CustomEdge& e) { return e.vertices.first; },
                [](const CustomEdge& e) { return e.vertices.second; }
            );

            cdt.eraseOuterTrianglesAndHoles();

            TriangleList newTriangles;
            for (const auto& tri : cdt.triangles) {
                int a = tri.vertices[0];
                int b = tri.vertices[1];
                int c = tri.vertices[2];
                newTriangles.push_back({a, b, c});
            }
            triangles = newTriangles;
        }
    }
}

Polygon3D insertZ(const Polygon2D& polygon2D, double z) {
    Polygon3D result;
    for (const auto& point : polygon2D) {
        result.push_back({point.x, point.y, z});
    }
    return result;
}

Polygon3D roll(const Polygon3D& polygon3D) {
    Polygon3D result = polygon3D;
    rotate(result.begin(), result.begin() + 1, result.end());
    return result;
}


map<int, ElementList3D> extrudePolygons(map<int, ElementList2D>& layerMap, double zMin, double zMax) {
    map<int, ElementList3D> extrudedLayerMap;
    map<int, ElementList2D>::iterator it;
    for (it = layerMap.begin(); it != layerMap.end(); it++) {
        int layerNumber = it->first;
        ElementList2D& elementList2D = it->second;
        ElementList3D elementList3D;
        for (int i = 0; i < elementList2D.size(); i++) {
            Polygon2D polygon2D = elementList2D[i].polygon2D;
            TriangleList triangle = elementList2D[i].triangles;
            
            Element3D element3DMin;
            Element3D element3DMax;

            Polygon3D polygon3DMin = insertZ(polygon2D, zMin);
            Polygon3D polygon3DMax = insertZ(polygon2D, zMax);

            element3DMin.polygon3D = polygon3DMin; // points_i_min
            element3DMax.polygon3D = polygon3DMax; // points_i_max
            element3DMin.triangles = triangle;
            element3DMax.triangles = triangle;
            element3DMin.clockwise = false;
            element3DMax.clockwise = false;
            elementList3D.push_back(element3DMin);
            elementList3D.push_back(element3DMax);
        }
        extrudedLayerMap[layerNumber] = elementList3D;
    }
    return extrudedLayerMap;
}

void writePLY(const string& filename, const map<int, ElementList3D>& extrudedLayerMap, int layerNumber) {
    ofstream plyFile(filename);
    if (!plyFile.is_open()) {
        cerr << "Failed to open the file: " << filename << endl;
        return;
    }

    Polygon3D vertices;
    TriangleList faces;

    ElementList3D elementListAtLayerNumber = extrudedLayerMap.at(layerNumber);
    for (const auto& element : elementListAtLayerNumber) {
        int baseIndex = vertices.size();

        // Add vertices for both top and bottom polygons
        for (const auto& vertex : element.polygon3D) {
            vertices.push_back(vertex);
        }

        // Add faces for the bottom and top polygons
        for (const auto& triplet : element.triangles) {
            faces.push_back({triplet.x + baseIndex, triplet.y + baseIndex, triplet.z + baseIndex});
        }
    }

    // Add side faces connecting top and bottom polygons
    int baseIndex1 = 0;
    int baseIndex2 = 0;
    for (int i = 0; i < elementListAtLayerNumber.size(); i += 2) {
        Element3D element1 = elementListAtLayerNumber[i];
        int numVertices = element1.polygon3D.size(); // should be same as element 2?
        baseIndex2 = baseIndex1 + numVertices;
        for (int j = 0; j < numVertices; j++) {
            int next = (j + 1) % numVertices;
            int bottom0 = baseIndex1 + j;
            int bottom1 = baseIndex1 + next;
            int top0 = baseIndex2 + j;
            int top1 = baseIndex2 + next;

            // Side face as two triangles forming a rectangle
            faces.push_back({bottom0, bottom1, top1}); // First triangle
            faces.push_back({top1, top0, bottom0});    // Second triangle
        }
        baseIndex1 += 2 * numVertices;
    }

    // Writing the PLY file
    plyFile << "ply" << endl;
    plyFile << "format ascii 1.0" << endl;
    plyFile << "element vertex " << vertices.size() << endl;
    plyFile << "property float x" << endl;
    plyFile << "property float y" << endl;
    plyFile << "property float z" << endl;
    plyFile << "element face " << faces.size() << endl;
    plyFile << "property list uchar int vertex_indices" << endl;
    plyFile << "end_header" << endl;

    for (const auto& vertex : vertices) {
        plyFile << vertex.x << " " << vertex.y << " " << vertex.z << endl;
    }

    for (const auto& face : faces) {
        plyFile << "3 " << face.x << " " << face.y << " " << face.z << endl;
    }

    plyFile.close();
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <GDS file>" << endl;
        return 1;
    }

    const char* gdsFileName = argv[1];
    GDSIIData* gdsIIData = readGDS(gdsFileName);
    map<int, PolygonList> layerPLMap= extractPolygons(gdsIIData);
    map<int, ElementList2D> layerMap = layerMapToElementList(layerPLMap);
    triangulatePolygons(layerMap);
    // printLayerMap(layerMap);
    
    map<int, ElementList3D> layerMap3D = extrudePolygons(layerMap, 0.0, 100.0);
    for (const auto& layer: layerMap3D) {
        string fileName = "Layer" + to_string(layer.first) + ".ply";
        writePLY(fileName, layerMap3D, layer.first);
    }
    delete gdsIIData;
    return 0;
}

