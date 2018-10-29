# ShootThem!

## Quick overview

This is a very simple shooting game I made many years ago. It is written in C++ with Irrlicht game engine and IrrKlang library for sounds. All level models are hand-made, other assets are taken from third-party games or libraries.

## Build

1. Unpack `irrKlang` and `irrlicht` libraries within the `Dependencies` directory
2. Go to `Dependencies/irrlicht-x.y.z/source/Irrlicht` and build it
  1. if you're using OSX:
    1. apply the `Dependencies/irrlicht-x.y.z.osx.patch` patch first
    2. use `make staticlib_osx` or `make sharedlib_osx`
  2. otherwise, just use `make`
3. Build the project: `cmake -B_build -H. && cmake --build _build`
