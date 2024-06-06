#/bin/bash
./gds example_for_deven.gds
vdb_tool -read output.ply -mesh2ls -write test.vdb
vdb_view test.vdb
