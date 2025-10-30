## template
my c++ project template

## using
- windows: cmake, ninja and msvc
- linux: cmake, ninja and prebuilt gcc (from xpack)

## build
build.py provides basic configure, build, and GCC setup on Linux,
but has the following platform constraints:
- linux: gcc version (well, you can change the version on the script but yeah)
- windows: MSVC (you can use GCC via winlibs but MSVC just works)
