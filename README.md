# MARs Engine

This code is under the [GNU General Public License v3.0](https://github.com/martimyc/Engine/blob/master/LICENSE).

Created by [Marc Latorre](https://github.com/marclafr) and [Marti Maj√≥](https://github.com/martimyc).

[Repository](https://github.com/martimyc/Engine).

## Innovation

- Multitexturing (avaiable since version:Geometry Viewer)
- Docking: We need to save/load the docking positions yet. For now you can dock the windows that are in the tabbar from the start inside your window, however if you restart the MARs Engine or resize the window, the docking windows will return in its original position. We will solve this issue in the next version.
- Vertex KDTree.

## Incomplete/Problems:

### Not finished: 

#### Time Manager: 

Timers work (window Timers), imported scenes are serialitzated to be loaded later, but GameObjects don't Update acording.

#### Resource manager (see below what is finished): 

- You can't manage files and folders: create / remove / rename yet.
- Upon start, all resources not managed inside Assets aren't generated in Library yet.
- Importing options can't be changed from the editor from the inspector yet.

#### AABB's problem: 

Update from parents don't work if you don't update the child later.

## Changes:

- MD5 UIDs

- GameObjects follow a hierarchy that applies all transformations.

- Inspector show all properties for each component on selected GameObject.

- You can pick objects by clicking the mouse, BUT: You must calculate the KDTree first, or it won't work.

- Camera component created.

- All meshes use a bounding volume (AABB) and can be discarded using Frustum Culling.

- Meshes and Textures are saved to our own format (.mm) under ìLibraryî folder.


### Resource Management:

- There is an ìAssetî window that shows all user assets.
- Every Asset stores itís importing options.

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
- Pressing Fù focus the camera around the selected geometry

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
