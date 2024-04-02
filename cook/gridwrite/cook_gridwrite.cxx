#include <openvdb/openvdb.h>
int main()
{
    openvdb::initialize();
    // Create a shared pointer to a newly-allocated grid of a built-in type:
    // in this case, a FloatGrid, which stores one single-precision floating point
    // value per voxel.  Other built-in grid types include BoolGrid, DoubleGrid,
    // Int32Grid and Vec3SGrid (see openvdb.h for the complete list).
    // The grid comprises a sparse tree representation of voxel data,
    // user-supplied metadata and a voxel space to world space transform,
    // which defaults to the identity transform.
    openvdb::FloatGrid::Ptr grid =
        openvdb::FloatGrid::create(/*background value=*/2.0);
    // Populate the grid with a sparse, narrow-band level set representation
    // of a sphere with radius 50 voxels, located at (1.5, 2, 3) in index space.
    openvdb::tools::makeSphere(*grid, /*radius=*/50.0, /*center=*/openvdb::Vec3f(1.5, 2, 3));
    // Associate some metadata with the grid.
    grid->insertMeta("radius", openvdb::FloatMetadata(50.0));
    // Associate a scaling transform with the grid that sets the voxel size
    // to 0.5 units in world space.
    grid->setTransform(
        openvdb::math::Transform::createLinearTransform(/*voxel size=*/0.5));
    // Identify the grid as a level set.
    grid->setGridClass(openvdb::GRID_LEVEL_SET);
    // Name the grid "LevelSetSphere".
    grid->setName("LevelSetSphere");
    // Create a VDB file object.
    openvdb::io::File file("mygrids.vdb");
    // Add the grid pointer to a container.
    openvdb::GridPtrVec grids;
    grids.push_back(grid);
    // Write out the contents of the container.
    file.write(grids);
    file.close();
}