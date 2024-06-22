#include <openvdb/openvdb.h>
#include <openvdb/tools/LevelSetPlatonic.h>

int main()
{
    openvdb::initialize();
    openvdb::FloatGrid::Ptr cubeA = 
        openvdb::tools::createLevelSetCube<openvdb::FloatGrid>(
            /*scale=*/1.5f, /*center=*/openvdb::Vec3f(0,0,0));
    cubeA->setName("LevelSetCube1");
    openvdb::FloatGrid::Ptr cubeB = 
        openvdb::tools::createLevelSetCube<openvdb::FloatGrid>(
            /*scale=*/1.0f, /*center=*/openvdb::Vec3f(0,0.5,0));
    cubeA->setName("LevelSetCubeA");
    cubeB->setName("LevelSetCubeB");
    // Create a VDB file object and write out the grid.
    openvdb::io::File("cubeA.vdb").write({cubeA});
    openvdb::io::File("cubeB.vdb").write({cubeB});
}
