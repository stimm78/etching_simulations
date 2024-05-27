#include "libGDSII.h"
#include <iostream>
#include <openvdb/openvdb.h>
#include <cstdio>

using namespace std;
using namespace libGDSII;

typedef vector<int> iVec;
typedef vector<double> dVec;

struct Polygon { // A Polygon is a set of vertices, located on specified layer number
    int layer;
    PolygonList pl;
};


GDSIIData* readGDS(const char* gdsFileName) {
    GDSIIData *gdsIIData = new GDSIIData(gdsFileName); 
    if (gdsIIData->ErrMsg) {
        printf("error: %s (aborting)\n", gdsIIData->ErrMsg->c_str());
        exit(1);
    }
    return gdsIIData;
}

std::vector<Polygon> parseGDSPolygons(GDSIIData* gdsIIData) {
    vector<Polygon> polygons;

    iVec allLayers = gdsIIData->GetLayers();

    // iterate over all layers and polygons
    // nl = number layers, np = number polygons, nv = number vertices
    for (int nl = 0; nl < allLayers.size(); nl++) {
        PolygonList PolygonsOnLayerNL = gdsIIData->GetPolygons(nl);
        for (size_t np = 0; np < PolygonsOnLayerNL.size(); np++) {
            polygons.push_back({nl, PolygonsOnLayerNL});
        }
    }
    return polygons;
}

void printPolygons(const vector<Polygon>& polygons) { // for debugging
    for (const auto& polygon : polygons) {
        std::cout << "Layer: " << polygon.layer << "\nVertices:\n";
        for (const auto& vertex : polygon.pl) {
            std::cout << "(" << vertex.at(0)<< ", " << vertex.at(1) << ")\n";
        }
        std::cout << "-----\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <argument>" << endl;
        return 1;
    }
    
    GDSIIData* gdsIIData = readGDS(argv[1]);
    vector<Polygon> polygons = parseGDSPolygons(gdsIIData);

    printPolygons(polygons);
    delete gdsIIData; // don't leak memory

    return 0;
}
