# ShootThem!

## Quick overview

This is a very simple shooting game I made many years ago.
It is written in C++ with Irrlicht game engine and IrrKlang library for sounds.
All level models are hand-made, other assets are taken from the Internet.

The version on the `master` branch has been significantly changed in 2020 for the sake of curiosity.

**2006:**

_back in 2006 game level editor was an FPS camera and one had to press magic keys on a keyboard to place targets on a level, it was very limited in functionality and it does not make any sense whatsoever to even put screenshots here, since it would simply look like a mesh viewer_

![Screenshot from 2006](https://github.com/shybovycha/shoot-them/raw/master/Screenshots/ShootThem!%2010_09_2020%208_15_14%20PM.png)
![Yet another screenshot from 2006](https://github.com/shybovycha/shoot-them/raw/master/Screenshots/ShootThem!%2013_09_2020%2010_18_52%20PM.png)

[Video on YouTube](https://youtu.be/m65z9kvWdYA)

**2020:**

The game now has main menu, end-game screen (although very crude, just a text for the moment being), a proper HUD with images and a more mind-bending post-processing for the "drunken" state of the main character:

![Game screenshot from 2020](https://github.com/shybovycha/shoot-them/raw/master/Screenshots/Shoot%20Them!%2019_10_2020%203_54_42%20PM.png)

Game editor now has a proper UI and is generally way more useful, allowing one to move both the objects on the levels, update and remove objects from the level, add lights to the level, save and load different levels from/to files, etc.

![Editor screenshot from 2020](https://github.com/shybovycha/shoot-them/raw/master/Screenshots/Shoot%20Them!%20Editor%2026_09_2020%201_08_56%20PM.png)
![Yet another editor screenshot from 2020](https://github.com/shybovycha/shoot-them/raw/master/Screenshots/Shoot%20Them!%20Editor%2026_09_2020%201_12_56%20PM.png)

[Video on YouTube](https://youtu.be/WDg1ctg_0GM)

## Build

Use CMake:

0. (optional) clean the build directory:

  ```
  cmake --build build --target clean
  ```

1. configure the build files:

  ```
  cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$VCPKG_HOME/scripts/buildsystems/vcpkg.cmake
  ```

2. build the editor and the game:

  ```
  cmake --build build --config Release
  ```

## Updating the dependencies

After making changes to the dependencies, make sure to update the `builtin-baseline` in the `vcpkg.json` file so that vcpkg does not install wrong versions of the dependencies.
This could be done with `vcpkg x-update-baseline`.

## TODO

N/A
