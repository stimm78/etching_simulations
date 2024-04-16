import os
from sys import argv
name = argv[1]

os.mkdir(name)
os.system("cp build.sh {}/".format(name))
os.system("cp CMakeLists.txt {}/".format(name))


