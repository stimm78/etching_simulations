/**
 * ChatGPT prompt:
 * Can you write me a complete open vdb program in cpp that loads to vdb files (each containing one grid), creates a box, then performs a boolean union on the two vdb grids and intersects the result with the box?
 **/

#include <openvdb/openvdb.h>
#include <openvdb/tools/Composite.h>
#include <openvdb/tools/GridTransformer.h>
#include <openvdb/tools/VolumeToMesh.h>
#include <openvdb/math/Stencils.h>
#include <iostream>
#include <string>

int main() {
    // Initialize the OpenVDB library
    openvdb::initialize();

    // File paths to the VDB files
    std::string file1 = "path/to/first.vdb";
    std::string file2 = "path/to/second.vdb";

    // Open the VDB files
    openvdb::io::File vdbFile1(file1);
    openvdb::io::File vdbFile2(file2);

    vdbFile1.open();
    vdbFile2.open();

    // Read the grids from the files
    openvdb::GridBase::Ptr baseGrid1 = vdbFile1.readGrid(/* grid name */);
    openvdb::GridBase::Ptr baseGrid2 = vdbFile2.readGrid(/* grid name */);

    vdbFile1.close();
    vdbFile2.close();

    // Cast the base grids to a specific grid type (e.g., FloatGrid)
    openvdb::FloatGrid::Ptr grid1 = openvdb::gridPtrCast<openvdb::FloatGrid>(baseGrid1);
    openvdb::FloatGrid::Ptr grid2 = openvdb::gridPtrCast<openvdb::FloatGrid>(baseGrid2);

    if (!grid1 || !grid2) {
        std::cerr << "Grids could not be read or cast." << std::endl;
        return -1;
    }

    // Perform a boolean union
    openvdb::tools::csgUnion(*grid1, *grid2);

    // Create a box for intersection
    openvdb::math::BBox<openvdb::Vec3f> box(openvdb::Vec3f(-10.0f, -10.0f, -10.0f), openvdb::Vec3f(10.0f, 10.0f, 10.0f));
    openvdb::tools::GridTransformer transformer;
    transformer.transformBox(grid1->transform(), box);

    // Intersect the union result with the box
    openvdb::FloatGrid::Ptr result = openvdb::tools::csgIntersection(*grid1, *transformer.transformBox(grid1->transform(), box));

    // Optional: Convert the grid to a mesh for visualization
    std::vector<openvdb::Vec3s> points;
    std::vector<openvdb::Vec4I> quads;
    std::vector<openvdb::Vec3I> triangles;

    openvdb::tools::volumeToMesh(*result, points, triangles, quads);

    // Process the mesh (points, quads, triangles) as needed

    return 0;
}