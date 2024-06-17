#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "libGDSII.h"
#include "triangle/tpp_interface.hpp"

using namespace std;
using namespace libGDSII;

struct Vertex {
    double x, y, z;
};
typedef vector<Vertex> Polygon3D; // A polygon a vector of vertices
typedef vector<Polygon3D> PolygonList3D; // A polygon a vector of vertices

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

map<int, PolygonList3D> extrudePolygons(std::map<int, PolygonList> layerMap, double halfExtrusionHeight) {
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
                p3D.push_back(Vertex{x, y, -halfExtrusionHeight});
            }
            for (int i = 0; i < polygon.size(); i+=2) {
                double x = polygon[i];
                double y = polygon[i+1];
                p3D.push_back(Vertex{x, y, halfExtrusionHeight});
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
                printf("%e", polygon[j]); // Use %e for scientific notation
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
                const Vertex& vertex = polygon[j];
                printf("    Vertex %zu: (%e, %e, %e)\n", j, vertex.x, vertex.y, vertex.z);
            }
        }
    }
}

/* Triangulation of polygons. Refer to https://github.com/dteal/gdsiistl/blob/master/gdsiistl.py*/
// void triangulatePolygons(map<int, PolygonList> layerMap) {
//     for (const auto& entry : layerMap) {
//         int layerNumber = entry.first;
//         const PolygonList& polygons = entry.second;
//         for (size_t i = 0; i < polygons.size(); ++i) { // Triangulate every polygon in PolygonList
//             const auto& polygon = polygons[i];
//         }
//     }
// }

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <GDS file>" << endl;
        return 1;
    }

    const char* gdsFileName = argv[1];
    GDSIIData* gdsIIData = readGDS(gdsFileName);
    map<int, PolygonList> layerMap = extractPolygons(gdsIIData);
    map<int, PolygonList3D> layerMap3D = extrudePolygons(layerMap, 100.0);
    // printPolygons(layerMap);
    printPolygons3D(layerMap3D);
    // triangulatePolygons(layerMap);

    delete gdsIIData;
    return 0;
}

