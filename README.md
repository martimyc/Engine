# MARs Engine

This code is under the [GNU General Public License v3.0](https://github.com/martimyc/Engine/blob/master/LICENSE).

Created by [Marc Latorre](https://github.com/marclafr) and [Marti Majó](https://github.com/martimyc).

[Repository](https://github.com/martimyc/Engine).

## Controls

Drag and drop FBX models, the camera will be centered on it and the other models will be removed.
Drag and drop PNG to texturize those models. We have multitexturing.
The textures are added into the materials and the materials added into the geometry.
During the process, a console will tell you if anything fails ;)

### Camera control

- Right click turns on “WASD” fps-like movement.
- Mouse wheel zooms in and out.
- Alt+Left click orbits the object.
- Pressing “f” focus the camera around the geometry.

### Inspector

- Shows the current mesh values and its materials.

### Configuration Window Options

#### Application

- Engine name and Organization.
- Framerate and miliseconds historygram.

#### Window

- Window size.
- Window size multiplier.
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
- 6 Buttons to focus the camera in the different angles of the object.

#### Hardware

- Information about CPU, GPU and versions about 3rd parties.

#### Textures

- A button in order to empty the textures loaded.
- A Add to mesh button to apply the Checkers in the texture.
- A delete button to delete the texture.

#### Draw Modes

- Wireframe
- Normals
- Polygon

#### Materials



#### Renderer 3D

- Draw World Axis
- Draw Grid
- Options about Grid.
