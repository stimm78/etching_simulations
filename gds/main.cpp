// main.cpp

#include "GDSProcessor.h"

int main(int argc, char* argv[]) {

    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <GDS file>" << endl;
        return 1;
    }

    const char* gdsFileName = argv[1];
    GDSIIData* gdsIIData = readGDS(gdsFileName);
    map<int, PolygonList> layerPLMap = extractPolygons(gdsIIData);
    map<int, ElementList2D> layerMap = layerMapToElementList(layerPLMap);
    triangulatePolygons(layerMap);

    map<int, ElementList3D> layerMap3D = extrudePolygons(layerMap, 0.0, 100.0);

    // Separate .ply for each layer
    for (const auto& layer : layerMap3D) { 
        string fileName = "Layer" + to_string(layer.first) + ".ply";
        writePLY(fileName, layerMap3D, layer.first);
    }
    
    delete gdsIIData;
    return 0;
}

