#include <openvdb/openvdb.h>
#include <openvdb/points/PointDataGrid.h>
#include <openvdb/tools/PointIndexGrid.h>
#include <openvdb/tools/PointsToMask.h>
#include <iostream>
#include <vector>

using namespace openvdb;
using namespace openvdb::points;

void createExtrudedPolygon() {
    // Initialize OpenVDB
    openvdb::initialize();

    // Define the initial polygon vertices (a simple square in this example)
    std::vector<Vec3f> baseVertices = {
        Vec3f(0.0f, 0.0f, 0.0f),
        Vec3f(1.0f, 0.0f, 0.0f),
        Vec3f(1.0f, 1.0f, 0.0f),
        Vec3f(0.0f, 1.0f, 0.0f)
    };

    // Define the extrusion direction and length
    Vec3f extrusionDir(0.0f, 0.0f, 1.0f);
    float extrusionLength = 1.0f;

    // Generate the extruded vertices
    std::vector<Vec3f> extrudedVertices;
    for (const auto& vertex : baseVertices) {
        extrudedVertices.push_back(vertex);
        extrudedVertices.push_back(vertex + extrusionDir * extrusionLength);
    }

    // Create a transform with a voxel size of 1.0
    auto transform = math::Transform::createLinearTransform(1.0);

    // Create a descriptor for the position attribute
    PointAttributeArray::Descriptor::Ptr descriptor = PointAttributeArray::Descriptor::create();
    descriptor->registerAttribute<Vec3f>("P");

    // Create a PointAttributeArray for the extruded vertices
    PointAttributeArray positionArray(descriptor);
    positionArray.setAttributeValues<Vec3f>("P", extrudedVertices);

    // Create a PointDataGrid
    auto grid = createPointDataGrid<PointDataGrid>(positionArray, transform);

    // Set the name of the grid
    grid->setName("ExtrudedPolygon");

    // Write the grid to a .vdb file
    io::File file("extruded_polygon.vdb");
    file.write({grid});
    file.close();
}

int main() {
    createExtrudedPolygon();
    return 0;
}

