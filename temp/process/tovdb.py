import os


source_dir = "stl"
output_dir = "vdb"
render = 1
render_dir = "ppm"

if not os.path.exists(source_dir): os.mkdir(source_dir)
if not os.path.exists(output_dir): os.mkdir(output_dir)
if not os.path.exists(render_dir): os.mkdir(render_dir)

convert_template = "vdb_tool -read stl/{}.stl -mesh2ls -write vdb/{}.vdb"
render_template = "vdb_render vdb/{}.vdb ppm/{}.ppm -shader diffuse -res 1920x1080 -focal 35 -samples 4 -lookat 0,0,0 -compression rle -v"
# render_template = "vdb_render vdb/{}.vdb ppm/{}.ppm -shader diffuse -res 1920x1080 -focal 35 -samples 4 -translate 0,5,5 -lookat 0,0,0 -compression rle -v"

quiet = 1
if quiet:
    convert_template += " -quiet"
    # render_template += " -quiet"

for filename in os.listdir(source_dir):
    if filename[-4:] != ".stl": continue
    name = filename[:-4]
    os.system(convert_template.format(name, name))
    if render: os.system(render_template.format(name, name))
    print(name)