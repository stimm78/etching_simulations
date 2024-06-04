#include "libGDSII.h"
#include <iostream>
#include <openvdb/openvdb.h>
#include <cstdio>
#include <string>

using namespace std;
using namespace libGDSII;

struct Point3D {
    double x, y, z;
    Point3D(double x = 0.0, double y = 0.0, double z = 0.0) : x(x), y(y), z(z) {} // default constructor
};

typedef vector<int> iVec;
typedef vector<double> dVec;
typedef vector<Point3D> pVec;

struct LayerPolygonList { // LayerPolygonList holds the layer number and a list of polygons on that layer
    int layer;
    PolygonList pl; // PolygonList is a vector of polygons. Each polygon contains a size 2 dVec of points.
};

struct LayerPolygonList3D {
    int layer;
    vector<pVec> pl3D;
};

GDSIIData* readGDS(const char* gdsFileName) {
    GDSIIData *gdsIIData = new GDSIIData(gdsFileName); 
    if (gdsIIData->ErrMsg) {
        printf("error: %s (aborting)\n", gdsIIData->ErrMsg->c_str());
        exit(1);
    }
    return gdsIIData;
}
// ------------------ 2D
vector<LayerPolygonList> parseGDSPolygons(GDSIIData* gdsIIData) { // given gds file, returns a vector of LayerPolygonLists
    vector<LayerPolygonList> layerPolygonLists;

    iVec Layers = gdsIIData->Layers;
    for (int nl = 0; nl < Layers.size(); nl++) {
        PolygonList layerNLPolygons = gdsIIData->GetPolygons(Layers[nl]);
        layerPolygonLists.push_back({Layers[nl], layerNLPolygons});
    }
    return layerPolygonLists;
}

void printPolygons(const vector<LayerPolygonList>& layerPolygonLists) { // for debugging
    // np = number polygons, nv = number vertices
    for (const auto& layerpl : layerPolygonLists) {
        printf("Layer: %i\n\n", layerpl.layer);
        for (int np = 0; np < layerpl.pl.size(); np++) { // iterate through polygonList
            printf("Polygon #%i has vertices: ", np);
            for (int nv = 0; nv < layerpl.pl[np].size()/2; nv++) {
                printf(" (%f, %f) ", layerpl.pl[np][2*nv+0], layerpl.pl[np][2*nv+1]);
            }
            cout << "\n\n\n";
        }
        std::cout << "-----\n";
    }
}
// ------------------ 3D
vector<LayerPolygonList3D> polygonTo3D(vector<LayerPolygonList>& layerPolygonLists, double z) {
    vector<LayerPolygonList3D> layerPolygonLists3D;
    for (auto& layerpl : layerPolygonLists) { // iterate through layerpolygons
        vector<pVec> polygonList3D(layerpl.pl.size() * 2); // Double the size of the vector to accommodate extruded vertices
        for (int np = 0; np < layerpl.pl.size(); np++) { // iterate through polygons in polygonList
            for (int nv = 0; nv < layerpl.pl[np].size() / 2; nv++) { // iterate through vertices of polygon layerpl.pl[np]
                double vx = layerpl.pl[np][2 * nv + 0];
                double vy = layerpl.pl[np][2 * nv + 1];
                
                // Push the original vertex with z-coordinate 0
                Point3D point1 = Point3D{vx, vy, 0};
                polygonList3D[np * 2].push_back(point1); 
            } 
            for (int nv = 0; nv < layerpl.pl[np].size() / 2; nv++) { // iterate through vertices of polygon layerpl.pl[np] again (not efficient, but probably works)
                double vx = layerpl.pl[np][2 * nv + 0];
                double vy = layerpl.pl[np][2 * nv + 1];
                
                // Push the extruded vertex with the specified z-coordinate
                Point3D point2 = Point3D{vx, vy, z};
                polygonList3D[np * 2 + 1].push_back(point2);
            }
        }
        layerPolygonLists3D.push_back(LayerPolygonList3D{layerpl.layer, polygonList3D});
    }
    return layerPolygonLists3D;
}

void printPolygons3D(const vector<LayerPolygonList3D>& layerPolygonLists3D) { // for debugging
    // np = number polygons, nv = number vertices
    for (const auto& layerpl : layerPolygonLists3D) {
        printf("Layer: %i\n\n", layerpl.layer);
        for (int np = 0; np < layerpl.pl3D.size(); np++) { // iterate through polygonList3D
            printf("Polygon #%i has vertices:\n", np);
            for (const auto& vertex : layerpl.pl3D[np]) {
                printf(" (%f, %f, %f) ", vertex.x, vertex.y, vertex.z);
            }
            cout << "\n\n";
        }
        std::cout << "-----\n";
    }
}

// void writePLY(const vector<LayerPolygonList>& polygons) { //To do? Or write directly to VDB
//
// }

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <argument>" << endl;
        return 1;
    }
    
    GDSIIData* gdsIIData = readGDS(argv[1]);
    vector<LayerPolygonList> polygons = parseGDSPolygons(gdsIIData);
    vector<LayerPolygonList3D> polygons3D = polygonTo3D(polygons, 5.0);

    // printPolygons(polygons);
    printPolygons3D(polygons3D);
    delete gdsIIData; // don't leak memory

    return 0;
}
