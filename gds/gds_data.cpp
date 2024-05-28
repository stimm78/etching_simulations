#include "libGDSII.h"
#include <iostream>
#include <openvdb/openvdb.h>
#include <cstdio>

using namespace std;
using namespace libGDSII;

typedef vector<int> iVec;
typedef vector<double> dVec;

struct PolygonLayer { // PolygonLayer holds the layer number and a list of polygons on that layer
    int layer;
    PolygonList pl; // PolygonList is a vector of polygons. Each polygon contains an size 2 dVec of points.
};


GDSIIData* readGDS(const char* gdsFileName) {
    GDSIIData *gdsIIData = new GDSIIData(gdsFileName); 
    if (gdsIIData->ErrMsg) {
        printf("error: %s (aborting)\n", gdsIIData->ErrMsg->c_str());
        exit(1);
    }
    return gdsIIData;
}

vector<PolygonLayer> parseGDSPolygons(GDSIIData* gdsIIData) {
    vector<PolygonLayer> polygons;

    iVec Layers = gdsIIData->Layers;
    for (int nl = 0; nl < Layers.size(); nl++) {
        PolygonList layerNLPolygons = gdsIIData->GetPolygons(Layers[nl]);
        polygons.push_back({Layers[nl], layerNLPolygons});
    }
    return polygons;
}

void printPolygons(const vector<PolygonLayer>& polygons) { // for debugging
    for (const auto& polygon : polygons) {
        cout << "Layer: " << polygon.layer << "\nVertices:\n";
        for (int np = 0; np < polygon.pl.size(); np++) {
            for (int nv = 0; nv < polygon.pl[np].size()/2; nv++) {
                cout << "(" << polygon.pl[np][2*nv+0] << ", " << polygon.pl[np][2*nv+1] << ")\n";
            }
        }
        std::cout << "-----\n";
    }
}

void writePLY(const vector<PolygonLayer>& polygons) {

}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <argument>" << endl;
        return 1;
    }
    
    GDSIIData* gdsIIData = readGDS(argv[1]);
    vector<PolygonLayer> polygons = parseGDSPolygons(gdsIIData);

    printPolygons(polygons);
    delete gdsIIData; // don't leak memory

    return 0;
}
