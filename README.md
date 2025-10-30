## template
my c++ project template

## using
- windows: cmake, ninja and msvc
- linux: cmake, ninja and prebuilt gcc (from xpack)

## build
build.py provides basic (configurue, build, gcc setup on linux)
but also limits you to:
- linux: gcc version (well, the version is defined on the python script but yeah)
- windows: msvc (gcc is kinda usable on winlibs but msvc just works)