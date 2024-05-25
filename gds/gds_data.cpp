#include "libGDSII.h"
#include <iostream>

using namespace std;
using namespace libGDSII;

typedef vector<int> iVec;
typedef vector<double> dVec;

GDSIIData* readGDS(const char* filename) {
    GDSIIData *gdsIIData = new GDSIIData(filename);
        if (gdsIIData->ErrMsg) {
            cout << "Error: " << *gdsIIData->ErrMsg << " (aborting)" << endl;
            delete gdsIIData;
            exit(1);
        } 
    return gdsIIData;
    }

int main() {
    GDSIIData *gdsIIData = new GDSIIData(string("example_for_deven.gds"));

    if (gdsIIData->ErrMsg) {
        printf("error: %s (aborting)\n", gdsIIData->ErrMsg->c_str());
        exit(1);
    }
    // nl = number of layers, np = number of polygons, nv = number of vertices
    iVec allLayers = gdsIIData->GetLayers();
    for (size_t nl = 0; nl < allLayers.size(); nl++) {
        PolygonList PolygonsOnLayerNL = gdsIIData->GetPolygons(nl);
        printf("Found %lu polygons on layer %lu: \n", PolygonsOnLayerNL.size(), nl);
        for (size_t np = 0; np < PolygonsOnLayerNL.size(); np++) {
            printf("Polygon #%lu has vertices: ", np);
            for (int nv = 0; nv < PolygonsOnLayerNL[np].size() / 2; nv++)
                printf(" {%e,%e} ", PolygonsOnLayerNL[np][2 * nv], PolygonsOnLayerNL[np][2 * nv + 1]); // Fixed the indexing here
            printf("\n");
    }
    }

    /***************************************************************/
    /* get all polygons on layer 3 that contain the reference point*/
    /* of the text string "Geometry" (also on layer 3)             */
    /***************************************************************/
    // PolygonList Layer3GeometryPolygons = gdsIIData->GetPolygons("Geometry", 3); // Renamed to avoid variable name conflict

    /***************************************************************/
    /* get all polygons on any layer that contain the reference pt */
    /* of the text string "Geometry" on the same layer             */
    /***************************************************************/
    // PolygonList AnyLayerGeometryPolygons = gdsIIData->GetPolygons("Geometry"); // Renamed to avoid variable name conflict

    // Don't forget to free the allocated memory
    delete gdsIIData;

    return 0;
}
