# DrunkEngine

## About the Project
DrunkEngine is the the result of developing a 3D Game Engine for our third year subject Game Engines. This is not a fully fledged game engine but holds core subsystems for building one.

## Info
This release includes an editor where you can import a .FBX file, transform all its contents and make your own scenes (you will be able to save the scene and load it from the editor).

Github Page - https://github.com/MarcFly/DrunkEngine

## Authors
The people currently working in this engine are Andreu Sacasas and Marc Torres, currently 3rd Year students coursing Design and Game Development at CITM, a center abscribed to the UPC.

## How to use it
DrunkEngine will automatically load an .FBX file with a street and some houses with textures.
It will export the files to our own format under a HIDDEN directory ./Library/

To move around the space, you can orbit around something with Alt+LeftClick or move in first person using RightClick+WASD. If you wander off to far, don't worry, press F to focus again to the current model.

In the menu bar, an array of options are available, take some time to look into it. We specially recommend using the Inspector and the Scene Viewer. Besides clicking them on the menu you can also open them with Ctrl+I and Ctrl+V respectively (you can check and change other shortcuts on the "input" section in the Options menu).
All keybindings are rebindable, make use of your perfect layout.

To add new fbx into the scene, just select the object that will be the parent for it on the Scene Viewer, go to Menu->Import... and select the new asset.
If you want to save the current scene or to load a new one, just go to Menu->Save/Load. 

In order to check out how time is flowing in-engine vs in-game, a simple collapsable header is under the Options Menu->Time Viewer.
The first 2 values represent engine delta time and time since startup, the 2 below represent current game delta time and time since game startup.

To change game speed you can change between modes:
 - Play - Normal Playback speed
 - Slow - Will slow gameplay by X times
 - Fast - Will speed up gameplay by X times
 - Reverse - Reverses play speed (negative delta time)
 - RSlow - Reverses play speed and slows reverse time by X amount
 - RFast - Reverses play speed and speeds reverse time by X amount

Currently game time is not used in any shape or form, but there is a structure prepared for future use of it, separating editor and game runtime code.

When saving a scene, it will be created automatically under an Assets folder inside the Executable directory.

If you make a mess loading things, you can clear up the editor at Options->Delete Scene.

## Innovation
- KD-Tree (used on frustum and mouse-picking to accelerate static Objs)
- Input window in options to change the camera controls and the window shortcuts.

## Each member's Work

### Andreu Sacasas

- Amplified Input options.
- Frustum implementation (Editor camera + component Cameras & FOV).
- Static System.
- KD-Tree + implemented it on frustum culling / mouse picking optimizations.
- ImGuizmo.
- World / Local transformations to GameObjs.
- Event System.
- Scene Viewer (Objs Tree + Selection).

### Marc Torres

- Mouse Pick.
- Recource Management.
- Save/Load Scene.
- Import Assets.
- GameObject Components in the Inspector.
- Export assets to our own format (into a hidden "Library" folder in Game).
- Reference counting for Resource Management
- Engine Time Management for Game and Editor mode.

### Skeletal Animation Sub-System
As part of the last assignment, a basic skeletal animation sub-system has been implemented. Currently it can handle single mesh skeletons with different animations. Animations can be created from a single timeline import, through the inspector and later revised with the animation panel or ingame.
GIFS

For a detailed explanation on our susbsystem and getting into our engine, refer to he following video:
[![](https://img.youtube.com/vi/bZ9W4b0eMIY&feature=youtu.be/0.jpg)](https://www.youtube.com/watch?v=bZ9W4b0eMIY&feature=youtu.be)
!(https://media.giphy.com/media/9x5b92dJqSBvtiqYBQ/giphy.gif)
!(https://media.giphy.com/media/A7VJ37OYWXSTzRGHP1/giphy.gif)

[**Link to Repo**][1]  
[**Latest Release**][2] 

## MIT License

Copyright (c) 2018 Marc Torres Jimenez & Andreu Sacasas Vel�zquez

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

[1]:
[2]:
