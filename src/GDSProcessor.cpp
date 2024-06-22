// GDSProcessor.cpp

#include "include/GDSProcessor.h"

// Reads GDS file and returns its data
GDSIIData* readGDS(const char* gdsFileName) {
    GDSIIData *gdsIIData = new GDSIIData(gdsFileName); 
    if (gdsIIData->ErrMsg) {
        printf("error: %s (aborting)\n", gdsIIData->ErrMsg->c_str());
        exit(1);
    }
    return gdsIIData;
}

// Extracts polygons from GDSData and returns a map from layer number to a polygon list
map<int, PolygonList> extractPolygons(GDSIIData* gdsIIData) { 
    map<int, PolygonList> layerMap;
    vector<int> layers = gdsIIData->GetLayers();
    for (int nl = 0; nl < layers.size(); nl++) {
        PolygonList layerNLPolygons = gdsIIData->GetPolygons(layers[nl]);
        layerMap[layers[nl]] = layerNLPolygons;
    }
    return layerMap;
}

// Converts polygons in layerMap to Vertex2D representation
map<int, ElementList2D> layerMapToElementList(map<int, PolygonList>& layerMap) {
    map<int, ElementList2D> layerMap2D;
    for (const auto& it : layerMap) {
        int layerNumber = it.first;
        const PolygonList& polygons = it.second;
        ElementList2D elementList;
        for (const auto& polygon : polygons) {
            Polygon2D polygon2D;
            for (int i = 0; i < polygon.size(); i += 2) {
                double x = polygon[i];
                double y = polygon[i + 1];
                polygon2D.push_back(Vertex2D{x, y});
            }
            elementList.push_back({polygon2D, {}, false});
        }
        layerMap2D[layerNumber] = elementList;
    }
    return layerMap2D;
}

// Checks if the polygon points are in clockwise order
bool checkClockwise(Polygon2D polygon) {
    double area = 0;
    for (int i = 0; i < polygon.size(); i++) {
        Vertex2D v1 = polygon[i];
        Vertex2D v2 = polygon[(i + 1) % polygon.size()];
        area += (v2.x - v1.x) * (v2.y + v1.y);
    }
    return area > 0;
}

// Performs constrained Delaunay triangulation of polygons
void triangulatePolygons(map<int, ElementList2D>& layerMap) {
    for (auto& layerPair : layerMap) {
        ElementList2D& elements = layerPair.second;
        for (Element2D& element : elements) {
            Polygon2D& polygon2D = element.polygon2D;
            TriangleList& triangles = element.triangles;

            if (checkClockwise(polygon2D)) {
                element.clockwise = true;
            }

            vector<CustomPoint2D> points;
            for (const auto& vertex : polygon2D) {
                points.push_back({vertex.x, vertex.y});
            }

            vector<pair<int, int>> boundarySegments(points.size());
            for (int j = 0; j < points.size(); j++) {
                boundarySegments[j] = {j, (j + 1) % points.size()};
            }

            vector<CustomEdge> edges;
            for (const auto& edge : boundarySegments) {
                edges.push_back({edge});
            }

            CDT::Triangulation<double> cdt(CDT::VertexInsertionOrder::AsProvided);
            cdt.insertVertices(points.begin(), points.end(),
                [](const CustomPoint2D& p) { return p.data[0]; },
                [](const CustomPoint2D& p) { return p.data[1]; }
            );
            cdt.insertEdges(edges.begin(), edges.end(),
                [](const CustomEdge& e) { return e.vertices.first; },
                [](const CustomEdge& e) { return e.vertices.second; }
            );
            cdt.eraseOuterTrianglesAndHoles();

            for (const auto& tri : cdt.triangles) {
                triangles.push_back({static_cast<int>(tri.vertices[0]), static_cast<int>(tri.vertices[1]), static_cast<int>(tri.vertices[2])});
            }
        }
    }
}

// Converts 2D vertices to 3D by adding a Z coordinate
Polygon3D insertZ(const Polygon2D& polygon2D, double z) {
    Polygon3D result;
    for (const auto& point : polygon2D) {
        result.push_back({point.x, point.y, z});
    }
    return result;
}

// Extrudes 2D polygons to 3D by adding top and bottom layers
map<int, ElementList3D> extrudePolygons(map<int, ElementList2D>& layerMap, double zMin, double zMax) {
    map<int, ElementList3D> extrudedLayerMap;
    for (auto& it : layerMap) {
        int layerNumber = it.first;
        ElementList2D& elementList2D = it.second;
        ElementList3D elementList3D;
        for (auto& element2D : elementList2D) {
            Polygon2D& polygon2D = element2D.polygon2D;
            TriangleList& triangles = element2D.triangles;
            
            Polygon3D polygon3DMin = insertZ(polygon2D, zMin);
            Polygon3D polygon3DMax = insertZ(polygon2D, zMax);

            elementList3D.push_back({polygon3DMin, triangles, false});
            elementList3D.push_back({polygon3DMax, triangles, false});
        }
        extrudedLayerMap[layerNumber] = elementList3D;
    }
    return extrudedLayerMap;
}

// Writes the extruded polygons on a specific layer to a PLY file
void writePLY(const string& filename, const map<int, ElementList3D>& extrudedLayerMap, int layerNumber) {
    ofstream plyFile(filename);
    if (!plyFile.is_open()) {
        cerr << "Failed to open the file: " << filename << endl;
        return;
    }

    Polygon3D vertices;
    TriangleList faces;

    const ElementList3D& elementListAtLayerNumber = extrudedLayerMap.at(layerNumber);
    for (const auto& element : elementListAtLayerNumber) {
        int baseIndex = vertices.size();

        for (const auto& vertex : element.polygon3D) {
            vertices.push_back(vertex);
        }

        for (const auto& triplet : element.triangles) {
            faces.push_back({triplet.x + baseIndex, triplet.y + baseIndex, triplet.z + baseIndex});
        }
    }

    int baseIndex1 = 0;
    int baseIndex2 = 0;
    for (int i = 0; i < elementListAtLayerNumber.size(); i += 2) {
        const Element3D& element1 = elementListAtLayerNumber[i];
        int numVertices = element1.polygon3D.size();
        baseIndex2 = baseIndex1 + numVertices;
        for (int j = 0; j < numVertices; j++) {
            int next = (j + 1) % numVertices;
            int bottom0 = baseIndex1 + j;
            int bottom1 = baseIndex1 + next;
            int top0 = baseIndex2 + j;
            int top1 = baseIndex2 + next;

            faces.push_back({bottom0, bottom1, top1});
            faces.push_back({top1, top0, bottom0});
        }
        baseIndex1 += 2 * numVertices;
    }

    plyFile << "ply" << endl;
    plyFile << "format ascii 1.0" << endl;
    plyFile << "element vertex " << vertices.size() << endl;
    plyFile << "property float x" << endl;
    plyFile << "property float y" << endl;
    plyFile << "property float z" << endl;
    plyFile << "element face " << faces.size() << endl;
    plyFile << "property list uchar int vertex_indices" << endl;
    plyFile << "end_header" << endl;

    for (const auto& vertex : vertices) {
        plyFile << vertex.x << " " << vertex.y << " " << vertex.z << endl;
    }

    for (const auto& face : faces) {
        plyFile << "3 " << face.x << " " << face.y << " " << face.z << endl;
    }

    plyFile.close();
}

