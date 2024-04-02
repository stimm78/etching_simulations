#include <openvdb/openvdb.h>
#include "Geometry.h" //<openvdb/tools/LevelSetSphere.h>
// #include <std>

int main()
{
    openvdb::initialize();
    // Create a FloatGrid
    openvdb::FloatGrid::Ptr grid =
        openvdb::FloatGrid::create(/*background value=*/2.0);
        // openvdb::tools::createLevelSetSphere<openvdb::FloatGrid>(
        //     /*radius=*/50.0, /*center=*/openvdb::Vec3f(1.5, 2, 3),
        //     /*voxel size=*/0.5, /*width=*/4.0);

    // load mesh from file

    Geometry::Ptr geom(new Geometry());
    geom->read("./stl/TJM_B013V1_pass1_mask.stl");

    // // Associate some metadata with the grid.
    // grid->insertMeta("radius", openvdb::FloatMetadata(50.0));
    // Name the grid "LevelSetSphere".
    grid->setName("LevelSetSphere");
    // Create a VDB file object and write out the grid.
    openvdb::io::File("mygrids.vdb").write({grid});
}

