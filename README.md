## template
my c++ project template

## using
- windows: cmake, ninja and msvc
- linux: cmake, ninja and prebuilt gcc (from xpack)

## build
build.py provides basic configure, build, and GCC setup on Linux,
but has the following platform constraints:
- linux: gcc version (well, the version is defined on the python script but yeah)
- windows: MSVC (you can use GCC via winlibs is supported but MSVC just works)
