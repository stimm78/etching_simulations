#include "libGDSII.h"
#include <iostream>
#include <openvdb/openvdb.h>
#include <cstdio>
#include <string>
#include <fstream>

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

vector<LayerPolygonList> parseGDSPolygons(GDSIIData* gdsIIData) { // parses a GDS file for all polygons, returns a vector of LayerPolygonLists
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

vector<LayerPolygonList3D> polygonTo3D(const vector<LayerPolygonList>& polygons, double height) {
    vector<LayerPolygonList3D> polygons3D;
    for (const auto& layerPolygon : polygons) {
        LayerPolygonList3D layerPolygon3D;
        layerPolygon3D.layer = layerPolygon.layer;
        for (const auto& polygon : layerPolygon.pl) {
            pVec polygon3D;
            for (int i = 0; i < polygon.size(); i += 2) {
                double x = polygon[i];
                double y = polygon[i + 1];
                polygon3D.push_back(Point3D(x, y, -height)); // Bottom face vertex
                polygon3D.push_back(Point3D(x, y, height)); // Top face vertex
            }
            layerPolygon3D.pl3D.push_back(polygon3D);
        }
        polygons3D.push_back(layerPolygon3D);
    }
    return polygons3D;
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

void writePLY(const vector<LayerPolygonList3D>& polygons3D, const string& filename) {
    ofstream plyFile(filename);

    if (!plyFile) {
        cerr << "Error: Cannot open file " << filename << " for writing." << endl;
        return;
    }

    // Count the total number of vertices and faces
    int vertexCount = 0;
    int faceCount = 0;

    for (const auto& layerpl : polygons3D) {
        for (const auto& polygon : layerpl.pl3D) {
            int n = polygon.size() / 2; // Number of vertices in each face
            vertexCount += polygon.size();
            faceCount += 2 * (n - 2); // Triangular faces for top and bottom
            faceCount += n; // Quad faces for sides
        }
    }

    // Write PLY header
    plyFile << "ply\n";
    plyFile << "format ascii 1.0\n";
    plyFile << "element vertex " << vertexCount << "\n";
    plyFile << "property float x\n";
    plyFile << "property float y\n";
    plyFile << "property float z\n";
    plyFile << "element face " << faceCount << "\n";
    plyFile << "property list uchar int vertex_indices\n";
    plyFile << "end_header\n";

    // Write vertices
    for (const auto& layerpl : polygons3D) {
        for (const auto& polygon : layerpl.pl3D) {
            for (const auto& vertex : polygon) {
                plyFile << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
            }
        }
    }

    // Write faces
    int vertexIndex = 0;
    for (const auto& layerpl : polygons3D) {
        for (const auto& polygon : layerpl.pl3D) {
            int n = polygon.size() / 2; // Number of vertices in each face

            // Top face
            for (int i = 1; i < n - 1; ++i) {
                plyFile << "3 " << vertexIndex << " " << vertexIndex + i << " " << vertexIndex + i + 1 << "\n";
            }

            // Bottom face
            int bottomStart = vertexIndex + n;
            for (int i = 1; i < n - 1; ++i) {
                plyFile << "3 " << bottomStart << " " << bottomStart + i + 1 << " " << bottomStart + i << "\n";
            }

            // Side faces
            for (int i = 0; i < n; ++i) {
                int next = (i + 1) % n;
                // First triangle of quad
                plyFile << "4 " << vertexIndex + i << " " << vertexIndex + next << " " << bottomStart + next << " " << bottomStart + i << "\n";
            }

            vertexIndex += polygon.size();
        }
    }

    plyFile.close();
    cout << "PLY file written to " << filename << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <argument>" << endl;
        return 1;
    }
    
    GDSIIData* gdsIIData = readGDS(argv[1]);
    vector<LayerPolygonList> polygons = parseGDSPolygons(gdsIIData);
    vector<LayerPolygonList3D> polygons3D = polygonTo3D(polygons, 1000.0);

    // printPolygons(polygons);
    // printPolygons3D(polygons3D);
    writePLY(polygons3D, "output.ply");
    delete gdsIIData; // don't leak memory

    return 0;
}
