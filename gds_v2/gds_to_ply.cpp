#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "libGDSII.h"
#include "triangle/tpp_interface.hpp"

using namespace std;
using namespace libGDSII;
using namespace tpp;
using Point = tpp::Delaunay::Point;

struct Vertex2D {
    double x, y;
};
struct Vertex3D {
    double x, y, z;
};
struct Triangle { // A triangle is a triplet of vertex indices
    int x, y, z;
};

typedef vector<Vertex2D> Polygon2D;
typedef vector<Polygon2D> PolygonList2D;
typedef vector<Vertex3D> Polygon3D;
typedef vector<Polygon3D> PolygonList3D;
typedef vector<Triangle> triangleList;

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
 * A layerMap2D is the following:
 * layerMap2D = {
 *  6 : [[Vertex1, Vertex2, Vertex3, ...], [Vertex1, Vertex2, Vertex3, ...], ...]
 * }
 * Where vertex is Vertex2D{x, y}
 * */
map<int, PolygonList2D> layerMapTo2D(map<int, PolygonList> layerMap) {
    map<int, PolygonList2D> layerMap2D;
    map<int, PolygonList>::iterator it;
    for (it = layerMap.begin(); it != layerMap.end(); it++) {
        int layerNumber = it->first;
        PolygonList polygons = it->second;
        PolygonList2D pList2D;
        for (const auto& polygon : polygons) {
            Polygon2D p2D;
            for (int i = 0; i < polygon.size(); i+=2) {
                double x = polygon[i];
                double y = polygon[i+1];
                p2D.push_back(Vertex2D{x, y});
            }
            pList2D.push_back(p2D);
        }
        layerMap2D[layerNumber] = pList2D;
    }
    return layerMap2D;
}

/* Extrudes polygons in the layerMap to extrusionHeight and -extrusionHeight. Returns a layerMap3D.
 * A layerMap3D is the following:
 * layerMap3D = {
 *  6 : [[Vertex1, Vertex2, Vertex3, ...], [Vertex1, Vertex2, Vertex3, ...], ...]
 * }
 * Where vertex is Vertex3D{x, y, z} and z is the extrusionHeight
 * EDIT AFTER COMPLETING TRIANGULATION
 * */
map<int, PolygonList3D> extrudePolygons(map<int, PolygonList> layerMap, double extrusionHeight) {
    map<int, PolygonList3D> layerMap3D;
    map<int, PolygonList>::iterator it; 
    for (it = layerMap.begin(); it != layerMap.end(); it++) {
        int layerNumber = it->first;
        PolygonList polygons = it->second;

        PolygonList3D pList3D;
        for (const auto& polygon : polygons) {
            Polygon3D p3D;
            for (int i = 0; i < polygon.size(); i+=2) {
                double x = polygon[i];
                double y = polygon[i+1];
                p3D.push_back(Vertex3D{x, y, -extrusionHeight});
            }
            for (int i = 0; i < polygon.size(); i+=2) {
                double x = polygon[i];
                double y = polygon[i+1];
                p3D.push_back(Vertex3D{x, y, extrusionHeight});
            }
            pList3D.push_back(p3D);
        }
        layerMap3D[layerNumber] = pList3D;
    }
    return layerMap3D;
}

void printPolygons(map<int, PolygonList> layerMap) {
    map<int, PolygonList>::iterator it;
    for (it = layerMap.begin(); it != layerMap.end(); it++) {
        int layerNumber = it->first;
        PolygonList polygons = it->second;
        printf("Layer %d:\n", layerNumber);
        // Iterate through the list of polygons
        for (size_t i = 0; i < polygons.size(); ++i) {
            vector<double> polygon = polygons[i];
            printf("  Polygon %zu: [", i);
            for (size_t j = 0; j < polygon.size(); ++j) {
                printf("%f", polygon[j]); // Use %e for scientific notation
                if (j != polygon.size() - 1) {
                    printf(", ");
                }
            }
            printf("]\n");
        }
    }
}

void printPolygons3D(map<int, PolygonList3D> layerMap3D) {
    map<int, PolygonList3D>::iterator it;
    for (it = layerMap3D.begin(); it != layerMap3D.end(); it++) {
        int layerNumber = it->first;
        PolygonList3D polygons = it->second;
        printf("Layer %d:\n", layerNumber);
        // Iterate through the list of polygons
        for (size_t i = 0; i < polygons.size(); ++i) {
            const Polygon3D& polygon = polygons[i];
            printf("  Polygon %zu:\n", i);
            for (size_t j = 0; j < polygon.size(); ++j) {
                const Vertex3D& vertex = polygon[j];
                printf("    Vertex %zu: (%e, %e, %e)\n", j, vertex.x, vertex.y, vertex.z);
            }
        }
    }
}

/* Constained Delanauy triangulation of polygons. Returns a map of layer number to triangleList.
 * A triangleList is the following:
 * triMap = {
 * 6 : [Triangle1, Triangle2, ...]
 * }
 * where each Triangle is a struct of three Vertex2Ds.
*/
map<int, triangleList> triangulatePolygons(map<int, PolygonList2D> layerMap2D) {
    // map<int, int> numTriangles; // store number of triangles for each layer
    map<int, triangleList> triMap;
    map<int, PolygonList2D>::iterator it;
    for (it = layerMap2D.begin(); it != layerMap2D.end(); it++) {
        int layerNumber = it->first;
        PolygonList2D polygons = it->second;

        for (size_t i = 0; i < polygons.size(); i++) { // Might need CW or CCW orientation?
            Polygon2D polygon = polygons[i];
            int totalPolygonPoints = polygons[i].size(); 

            vector<int> pointVector; // pointVector is [0, 1, 2, ... polygons[i].size()]
            for (int j = 0; j < totalPolygonPoints; j++) {
                pointVector.push_back(i); 
            } 

            vector<int> rotated_array(totalPolygonPoints); // Shift all elements right, wrap last element
            for (int j = 0; j < totalPolygonPoints; j++) {
                rotated_array[j] = pointVector[(j + 1) % totalPolygonPoints]; 
            }

            vector<pair<int, int>> edges(totalPolygonPoints); // Connects pairs of vertices to make edges
            for (int j = 0; j < totalPolygonPoints; j++) {
                edges[j] = make_pair(pointVector[j], rotated_array[j]); 
            }

            vector<Point> delaunayInput; // Convert vertices to Delaunay Points
            for (const auto& vertex : polygon) {
                delaunayInput.push_back(Point(vertex.x, vertex.y));
            }

            vector<Point> segments; // Convert edges to Delaunay segments
            for (const auto& edge : edges) {
                segments.push_back(delaunayInput[edge.first]);
                segments.push_back(delaunayInput[edge.second]);
            }
            // Triangulating time!
            Delaunay trGenerator(delaunayInput);
            trGenerator.setSegmentConstraint(segments);
            bool enforceQuality = false;
            trGenerator.Triangulate(enforceQuality);
            for (FaceIterator fit = trGenerator.fbegin(); fit != trGenerator.fend(); ++fit) {
                int vertexIdx1 = fit.Org();
                int vertexIdx2 = fit.Dest();
                int vertexIdx3 = fit.Apex();
                Triangle triangle{vertexIdx1, vertexIdx2, vertexIdx3};
                triMap[layerNumber].push_back(triangle);
                // double x1 = delaunayInput[vertexIdx1][0];
                // double y1 = delaunayInput[vertexIdx1][1];
                // double x2 = delaunayInput[vertexIdx2][0];
                // double y2 = delaunayInput[vertexIdx2][1];
                // double x3 = delaunayInput[vertexIdx3][0];
                // double y3 = delaunayInput[vertexIdx3][1];
                // Vertex2D a{x1, y1};
                // Vertex2D b{x2, y2};
                // Vertex2D c{x3, y3};
            }
        }
    }
    return triMap;
}

void printTriangulatedPolygons(map<int, triangleList> triMap) {
    for (auto it = triMap.begin(); it != triMap.end(); ++it) {
        int layerNumber = it->first;
        triangleList triangles = it->second;

        cout << "Layer " << layerNumber << ":" << endl;
        for (size_t i = 0; i < triangles.size(); ++i) {
            Triangle triangle = triangles[i];
            cout << "  Triangle " << i << ": (" << triangle.x << ", " << triangle.y << ", " << triangle.z << ")" << endl;
        }
    }
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <GDS file>" << endl;
        return 1;
    }

    const char* gdsFileName = argv[1];
    GDSIIData* gdsIIData = readGDS(gdsFileName);
    map<int, PolygonList> layerMap = extractPolygons(gdsIIData);

    map<int, PolygonList2D> layerMap2D = layerMapTo2D(layerMap);
    map<int, triangleList> triList = triangulatePolygons(layerMap2D);
    // printTriangulatedPolygons(triList);

    // double extrusionHeight = 100.0;
    // map<int, PolygonList3D> layerMap3D = extrudePolygons(layerMap, extrusionHeight);
    // printPolygons(layerMap);
    // printPolygons3D(layerMap3D);

    delete gdsIIData;
    return 0;
}

