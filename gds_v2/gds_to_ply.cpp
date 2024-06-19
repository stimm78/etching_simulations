#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <algorithm>
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
struct Triplet { 
    int x, y, z;
};

typedef vector<Vertex2D> Polygon2D; // Polygon2D is a vector of vertices Vertex2D{x, y}
typedef vector<Polygon2D> PolygonList2D; // PolygonList2D is a vector of Polygon2Ds
typedef vector<Vertex3D> Polygon3D;// Polygon3D is a vector of vertices Vertex3D{x, y, z}
typedef vector<Polygon3D> PolygonList3D;// Polygon3D is a vector of vertices Polygon3Ds
typedef vector<Triplet> Triangles; // Triangles is a vector of triplets
typedef vector<Triangles> TrianglesList;// TriangleList is a vector of Triangles 

struct Element { // Element is a struct of the polygon, its triangulation, and whether it is clockwise
    Polygon2D polygon2D;
    Triangles triangles; 
    bool clockwise;
};
typedef vector<Element> ElementList;

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
map<int, ElementList> layerMapToElementList(map<int, PolygonList>& layerMap) {

    map<int, ElementList> layerMap2D;

    map<int, PolygonList>::iterator it;
    for (it = layerMap.begin(); it != layerMap.end(); it++) {
        int layerNumber = it->first;
        PolygonList polygons = it->second;
        ElementList elementList;
        for (const auto& polygon : polygons) {

            Polygon2D polygon2D;
            Triangles triangles;
            bool clockwise = false;
            Element element;

            for (int i = 0; i < polygon.size(); i+=2) {
                double x = polygon[i];
                double y = polygon[i+1];
                polygon2D.push_back(Vertex2D{x, y});
            }
            element = Element{polygon2D, triangles, clockwise};
            elementList.push_back(element);
        }
        layerMap2D[layerNumber] = elementList;
    }
    return layerMap2D;
}

/* Checks clockwise configuration of points in polygon using method linked here:
 * https://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order*/
bool checkClockwise(Polygon2D polygon) {
    double area = 0;
    for (int i = 0; i < polygon.size(); i++) {
        Vertex2D v1 = polygon[i];
        Vertex2D v2 = polygon[(i+1) % polygon.size()];
        area += (v2.x - v1.x) * (v2.y + v1.y);
    }
    return area > 0;
}


/* Constained Delanauy triangulation of polygons. Constraints are the outside edges of the polygon.*/
void triangulatePolygons(map<int, ElementList>& layerMap) {

    map<int, ElementList>::iterator it;
    for (it = layerMap.begin(); it != layerMap.end(); it++) {
        int layerNumber = it->first;
        ElementList& elements = it->second;

        for (int i = 0; i < elements.size(); i++) {
            Polygon2D polygon2D = elements[i].polygon2D;
            Triangles triangle = elements[i].triangles;
            if (checkClockwise(polygon2D)) elements[i].clockwise = true;

            int totalPolygonPoints = polygon2D.size();

            vector<int> pointVector; // pointVector is [0, 1, 2, ... polygons[i].size()]
            for (int j = 0; j < totalPolygonPoints; j++) {
                pointVector.push_back(j); 
            } 

            vector<int> rotated_array(totalPolygonPoints); // Shift all elements right, wrap last element (there could be some stl algo for this, check later)
            for (int j = 0; j < totalPolygonPoints; j++) {
                rotated_array[j] = pointVector[(j + 1) % totalPolygonPoints]; 
            }

            vector<pair<int, int>> edges(totalPolygonPoints); // Connects pairs of vertices to make edges
            for (int j = 0; j < totalPolygonPoints; j++) {
                edges[j] = make_pair(pointVector[j], rotated_array[j]); 
            }

            vector<Point> delaunayInput; // Convert vertices to Delaunay Points
            for (const auto& vertex : polygon2D) {
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
            Triangles triangles;
            for (FaceIterator fit = trGenerator.fbegin(); fit != trGenerator.fend(); ++fit) {
                int vertexIdx1 = fit.Org();
                int vertexIdx2 = fit.Dest();
                int vertexIdx3 = fit.Apex();
                triangles.push_back(Triplet{vertexIdx1, vertexIdx2, vertexIdx3});
            }
            elements[i].triangles = triangles; 
        }
    }
}

void printLayerMap(const map<int, ElementList>& layerMap) {
    cout << "layers = {" << endl;
    for (auto it = layerMap.begin(); it != layerMap.end(); ++it) {
        int layerNumber = it->first;
        const ElementList& elements = it->second;

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
                const Triplet& triplet = element.triangles[i];
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

// void printTriangulatedPolygons(map<int, TrianglesList> triMap) {
//     for (auto it = triMap.begin(); it != triMap.end(); ++it) {
//         int layerNumber = it->first;
//         TrianglesList triList = it->second;
//
//         cout << "Layer " << layerNumber << ":" << endl;
//         for (size_t i = 0; i < triList.size(); ++i) {
//
//         }
//     }
// }

// vector<Vertex3D> insertZ(const Polygon2D& points, double z) {
//     vector<Vertex3D> result;
//     for (const auto& p : points) {
//         result.push_back({p.x, p.y, z});
//     }
//     return result;
// }
//
// vector<Vertex3D> roll(const vector<Vertex3D>& points) {
//     vector<Vertex3D> result = points;
//     rotate(result.begin(), result.begin() + 1, result.end());
//     return result;
// }
//
// void extrudePolygons(const map<int, PolygonList2D>& layerMap2D, const map<int, TrianglesList>& triMap, double zmin, double zmax, map<int, vector<Triangle>>& extrudedTriangles) {
//     for (const auto& layer : layerMap2D) {
//         int layerID = layer.first;
//         const auto& polygons = layer.second;
//
//         for (const auto& polygon : polygons) {
//             vector<Vertex3D> points_i_min = insertZ(polygon, zmin);
//             vector<Vertex3D> points_i_max = insertZ(polygon, zmax);
//             vector<Vertex3D> points_j_min = roll(points_i_min);
//             vector<Vertex3D> points_j_max = roll(points_i_max);
//
//             for (size_t i = 0; i < points_i_min.size(); ++i) {
//                 // Create the side faces of the extrusion
//                 extrudedTriangles[layerID].push_back({points_i_min[i], points_j_min[i], points_j_max[i]});
//                 extrudedTriangles[layerID].push_back({points_j_max[i], points_i_max[i], points_i_min[i]});
//             }
//
//             // Add the top and bottom faces using the triangulated 2D polygons
//             const auto& triangles = triMap.at(layerID);
//             for (const auto& triangle : triangles) {
//                 for (const auto& triplet : triangle) {
//                     // Bottom face
//                     extrudedTriangles[layerID].push_back({
//                         points_i_min[triplet.x],
//                         points_i_min[triplet.y],
//                         points_i_min[triplet.z]
//                     });
//
//                     // Top face
//                     extrudedTriangles[layerID].push_back({
//                         points_i_max[triplet.x],
//                         points_i_max[triplet.y],
//                         points_i_max[triplet.z]
//                     });
//                 }
//             }
//         }
//     }
// }
// void writePLY(const map<int, vector<Triangle>>& extrudedTriangles, const string& filename) {
//     ofstream plyFile;
//     plyFile.open(filename);
//
//     plyFile << "ply\nformat ascii 1.0\n";
//     size_t vertexCount = 0;
//     size_t faceCount = 0;
//     map<Vertex3D, int> vertexMap;
//     vector<Vertex3D> vertices;
//     vector<vector<int>> faces;
//
//     for (const auto& layer : extrudedTriangles) {
//         const auto& triangles = layer.second;
//         for (const auto& triangle : triangles) {
//             vector<int> face;
//             for (const auto& vertex : {triangle.v1, triangle.v2, triangle.v3}) {
//                 if (vertexMap.find(vertex) == vertexMap.end()) {
//                     vertexMap[vertex] = vertexCount++;
//                     vertices.push_back(vertex);
//                 }
//                 face.push_back(vertexMap[vertex]);
//             }
//             faces.push_back(face);
//         }
//     }
//
//     plyFile << "element vertex " << vertexCount << "\n";
//     plyFile << "property float x\nproperty float y\nproperty float z\n";
//     plyFile << "element face " << faces.size() << "\n";
//     plyFile << "property list uchar int vertex_indices\n";
//     plyFile << "end_header\n";
//
//     for (const auto& vertex : vertices) {
//         plyFile << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
//     }
//
//     for (const auto& face : faces) {
//         plyFile << "3 " << face[0] << " " << face[1] << " " << face[2] << "\n";
//     }
//
//     plyFile.close();
// }

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <GDS file>" << endl;
        return 1;
    }

    const char* gdsFileName = argv[1];
    GDSIIData* gdsIIData = readGDS(gdsFileName);
    map<int, PolygonList> layerPLMap= extractPolygons(gdsIIData);
    map<int, ElementList> layerMap = layerMapToElementList(layerPLMap);
    triangulatePolygons(layerMap);
    printLayerMap(layerMap);
   
    delete gdsIIData;
    return 0;
}

