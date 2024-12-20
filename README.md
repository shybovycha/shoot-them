# ShootThem!

## Quick overview

This is a very simple shooting game I made many years ago.
It is written in C++ with Irrlicht game engine and IrrKlang library for sounds.
All level models are hand-made, other assets are taken from the Internet.

The version on the `master` branch has been significantly changed in 2020 for the sake of curiosity.

**2006:**

_back in 2006 game level editor was an FPS camera and one had to press magic keys on a keyboard to place targets on a level, it was very limited in functionality and it does not make any sense whatsoever to even put screenshots here, since it would simply look like a mesh viewer_

![Screenshot from 2006](https://github.com/shybovycha/shoot-them/raw/master/screenshots/ShootThem!%2010_09_2020%208_15_14%20PM.png)
![Yet another screenshot from 2006](https://github.com/shybovycha/shoot-them/raw/master/screenshots/ShootThem!%2013_09_2020%2010_18_52%20PM.png)

[Video on YouTube](https://youtu.be/m65z9kvWdYA)

**2020:**

The game now has main menu, end-game screen (although very crude, just a text for the moment being), a proper HUD with images and a more mind-bending post-processing for the "drunken" state of the main character:

![Game screenshot from 2020](https://github.com/shybovycha/shoot-them/raw/master/screenshots/Shoot%20Them!%2019_10_2020%203_54_42%20PM.png)

Game editor now has a proper UI and is generally way more useful, allowing one to move both the objects on the levels, update and remove objects from the level, add lights to the level, save and load different levels from/to files, etc.

![Editor screenshot from 2020](https://github.com/shybovycha/shoot-them/raw/master/screenshots/Shoot%20Them!%20Editor%2026_09_2020%201_08_56%20PM.png)
![Yet another editor screenshot from 2020](https://github.com/shybovycha/shoot-them/raw/master/screenshots/Shoot%20Them!%20Editor%2026_09_2020%201_12_56%20PM.png)

[Video on YouTube](https://youtu.be/WDg1ctg_0GM)

**2023:**

Slightly reworked the shader so it looks nicer:

![Game screenshot from 2023](https://github.com/shybovycha/shoot-them/raw/master/screenshots/Shoot%20Them!%2021_09_2023.png)

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

### Game

- [x] remove implicit namespace imports
- [ ] replace explicit colors with named constants
- [x] replace raw pointers with smart pointers
- [x] fix wobbling effect
- [x] record a video (https://youtu.be/m65z9kvWdYA)
- [x] fix timer speed
- [x] display endgame screen with high scores `¯\_(ツ)_/¯`
- [x] import each scene from a scene file rather than DAT files with questionable undocumented format
- [x] load game settings from a separate file rather than being hard-coded vector population code O_o
- [ ] add splash screen
- [x] encapsulate logic in classes
- [x] ~~`ResourceManager` is redundant~~
- [x] clean up the design - `InputHandler` (aka dispatcher) is probably redundant, `GameState` (aka state) should be read-only, maybe extract event / action queue to a separate class
- [ ] rework the abuse of `std::shared_ptr` and raw pointers (according to https://www.internalpointers.com/post/move-smart-pointers-and-out-functions-modern-c and https://www.modernescpp.com/index.php/c-core-guidelines-passing-smart-pointer)
- [x] ~~two targets on 3rd level are indestructable~~
- [x] when the time runs out the next level is not loaded
- [ ] add end level menu
- [ ] replace existing 3D models with the new artwork (TBD)
- [x] ~~change the default GUI theme~~
- [x] do not use exceptions (https://google.github.io/styleguide/cppguide.html#Exceptions)
- [x] new game in the middle of the game should reset everything
- [x] properly copy resources to the directory with the binary
- [x] ~~copy the DLLs to the directory with the binary~~
- [x] replace status text with some sane HUD
- [ ] add settings menu
- [ ] write settings to file
- [x] read settings from file
- [x] bring the sound back
- [x] replace level meshes with OBJ
- [x] replace level meshes with Collada
- [x] update font in menu
- [x] ~~load UI from file~~
- [x] use shader for the post-processing ("drunk" effect)
- [x] clamp the rendered picture so there are no artifacts (mb zoom the quad a bit)
- [x] fix collision detection - potentially the ray overlaps the rendering quad first and hence no collisions with targets are detected (either hide the quad or start ray a bit further from the camera)
- [ ] add hints for controls to the first level
- [ ] create & use target model for the first level
- [ ] add level transition animation

### Editor

- [x] add UI
- [x] manage targets with UI elements, not keyboard and mouse click
- [x] display the structure of a level in UI
- [x] add multiple levels in one run
- [x] allow for level removal
- [x] remove targets
- [x] change target position
- [x] place target in front of camera instead of in place of camera
- [x] replace FPS camera with mouse-controlled one
- [x] add helpful help in UI
- [x] update the default low-res font with something more readable
- [x] make button icons use transparency
- [x] prevent game manager window from closing
- [x] ~~save file dialog does not pick up non-existent files~~
- [x] use mesh filename when adding game manager tree node
- [x] store level mesh in level object
- [x] load level mesh when switching levels in manager
- [x] ~~camera can not be controlled with keyboard~~
- [ ] when loading levels file, load models from the same directory
- [x] create custom camera animator to control camera with both keyboard and mouse in a reasonable way
- [x] create custom file dialog to allow saving files
- [x] manage lights
- [x] refactor Light and Target on level to Entity
- [ ] add & manage properties for level entities (?)
- [x] ~~render AABB around selected entity~~
- [x] draw axes to allow for translation of entities
- [x] ~~set ambient light so that levels are not pitch black~~
- [x] do not hide scene nodes - drop them; but first figure out why that causes runtime exception
- [x] replace mouse wheel for moving camera back/forth with Shift + Mouse Right Button + Mouse Move
- [x] camera translation does not work in **current** camera plane, but rather in initial
- [x] moving selected entity relies on the **absolute** offset, whilst it should rely on **relative** offset instead
- [x] ~~load UI from file~~
- [ ] use chicken model instead of sphere for target placement

### Miscellanious

- [ ] extract level reader & writer into separate library
- [x] update Irrlicht
- [ ] replace Irrlicht with OGRE or [Skylicht](https://github.com/skylicht-lab/skylicht-engine) or [Dilligent](https://github.com/DiligentGraphics/DiligentEngine) or anything newer than DX9
- [x] add github releases
- [x] document progress
- [x] add screenshots to github page
