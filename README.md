# MARs Engine

This code is under the [GNU General Public License v3.0](https://github.com/martimyc/Engine/blob/master/LICENSE).

Created by [Marc Latorre](https://github.com/marclafr) and [Marti MajÃ³](https://github.com/martimyc).

[Webpage](http://marsengine3d.webnode.cat/)

[Repository](https://github.com/martimyc/Engine).

## Innovation

- Multitexturing (avaiable since version:Geometry Viewer)
- Docking Finished!
- Vertex KDTree.

## Changes:

- Skeletal Animation
- Game Objects can be deleted by pressing the key delete.
- Most functionality optimized and working way better.

## Skeletal Animation

- To see the animations, put any mesh by double clicking them in the assets window. Then add the desired animations the same way and while the Game Object you want to put them into is selected, then click play.
- The skeleton is also transformed with the animation, if you want to see it put the draw mode to: wireframe mode.
- You can also check the bind pose with a button in the Animator component in the inspector.

### Resource Management:

- There is an “Asset” window that shows all user assets.
- Every Asset stores it’s importing options.

## Controls

### Loading an FBX:
Standard options may not be the best. For this reason, you have different importing options, a window will show and you can choose how to do the import.

## IMPORTANT:
If you export a scene and delete all directory hirarchy after to make a .zip (like the "street.fbx") you should load scene with the following option:
Import Settings --> Material's Configuration --> All textures are in scenes dir (load the fbx with this on)

After this all inside the fbx will be loaded in the assets window, you can load them in the scene by clicking whatever you want to import and click on "Load To Scene".

You have some samples to test the Engine.


### Camera controls

- Frustum culling optimization
- Right click turns on WASD fps-like movement and R/E (up, down)
- Mouse wheel zooms in and out
- Alt+Left click orbits the object
- Pressing F focus the camera around the selected geometry

### Inspector

- Shown when you focus any GameObject

### Configuration Window Options

#### Application

- Engine name and Organization
- Framerate and miliseconds historygram

#### Window

- Window size.
- Window size multiplier
- Options about

    1. Fullscreen
    2. Resizable
    3. Borderless
    4. Fullscreen Desktop

#### Audio

- Volume, we don't have sounds yet though :(

#### Camera 3D

- WASD movement speed
- Zoom speed
- Rotation sensivility
- Far Plane from Frustum
- FOV (vertical)

#### Hardware

- Information about CPU, GPU and versions about 3rd parties

#### Draw Modes

- Wireframe
- Normals
- Polygon
- Draw KDT

#### Renderer 3D

- Draw Mouse Picking
- Draw World Axis
- Draw Grid
- Options about Grid
