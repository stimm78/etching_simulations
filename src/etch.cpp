#include <openvdb/openvdb.h>
#include <openvdb/tools/Composite.h>

int main() {
    openvdb::initialize();
    openvdb::io::File file1("cubeA.vdb");
    openvdb::io::File file2("cubeB.vdb");
    file1.open();
    file2.open();

    openvdb::GridBase::Ptr gridA_base = file1.readGrid("LevelSetCubeA");
    openvdb::GridBase::Ptr gridB_base = file2.readGrid("LevelSetCubeB");
    
    file1.close();
    file2.close();
    
    openvdb::FloatGrid::Ptr gridA = openvdb::gridPtrCast<openvdb::FloatGrid>(gridA_base);
    openvdb::FloatGrid::Ptr gridB = openvdb::gridPtrCast<openvdb::FloatGrid>(gridB_base);
    
    openvdb::FloatGrid::ConstPtr
    copyOfGridA = gridA->deepCopy(),
    copyOfGridB = gridB->deepCopy();

    openvdb::tools::csgUnion(*gridA, *gridB);
    openvdb::io::File("cubeAUnion.vdb").write({gridA});
    openvdb::io::File("cubeBUnion.vdb").write({gridB});

    gridA = copyOfGridA->deepCopy();
    gridB = copyOfGridB->deepCopy();

    openvdb::tools::csgIntersection(*gridA, *gridB);
    openvdb::io::File("cubeAIntersect.vdb").write({gridA});
    openvdb::io::File("cubeBIntersect.vdb").write({gridB});

    gridA = copyOfGridA->deepCopy();
    gridB = copyOfGridB->deepCopy();

    openvdb::tools::csgDifference(*gridA, *gridB);
    openvdb::io::File("cubeADifference.vdb").write({gridA});
    openvdb::io::File("cubeBDifference.vdb").write({gridB});
}
