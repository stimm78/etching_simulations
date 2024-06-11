#/bin/bash
./gds example_for_deven.gds
vdb_tool -read output.ply -mesh2ls -write output.vdb config.txt
vdb_view test.vdb
