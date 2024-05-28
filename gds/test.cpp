#include "libGDSII.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace libGDSII;

int main() {
    /********************************************************************/
    /* try to instantiate GDSIIData structure from binary GDSII file    */
    /********************************************************************/
    GDSIIData *gdsIIData  = new GDSIIData(string("example_for_deven.gds"));

    if (gdsIIData->ErrMsg) {
        printf("error: %s (aborting)\n", gdsIIData->ErrMsg->c_str());
        exit(1);
    }

    /***************************************************************/
    /* output a text-based description of the geometry             */
    /***************************************************************/
    // gdsIIData->WriteDescription(); // writes to console
    // gdsIIData->WriteDescription("MyOutputFile");

    /***************************************************************/
    /* get all polygons on layer 3 *********************************/
    /***************************************************************/
    PolygonList Layer3Polygons = gdsIIData->GetPolygons(1);

    printf("Found %lu polygons on layer 1: \n", Layer3Polygons.size());
    for (size_t np = 0; np < Layer3Polygons.size(); np++) {
        printf("Polygon #%lu has vertices: ", np);
        for (int nv = 0; nv < Layer3Polygons[np].size() / 2; nv++) {
            printf(" {%e, %e} ", Layer3Polygons[np][2 * nv], Layer3Polygons[np][2 * nv + 1]);
        }
        printf("\n");
    }

    /***************************************************************/
    /* get all polygons on layer 3 that contain the reference point */
    /* of the text string "Geometry" (also on layer 3)             */
    /***************************************************************/
    PolygonList Layer3PolygonsWithRef = gdsIIData->GetPolygons("Geometry", 3);

    /***************************************************************/
    /* get all polygons on any layer that contain the reference pt  */
    /* of the text string "Geometry" on the same layer             */
    /***************************************************************/
    PolygonList AllLayerPolygonsWithRef = gdsIIData->GetPolygons("Geometry");

    delete gdsIIData;

    return 0;
}

