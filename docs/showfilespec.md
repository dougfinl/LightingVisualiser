# Show File

Show files should be named `<show_name>.vis`, and must follow the [YAML](https://yaml.org/) format.


## Specification

`ambientIntensity` - the overall ambient light in the scene (not affected by DMX values).


### Models

Section name: `models`

A list of model objects to be loaded into the 3D scene.

#### Required
- `src` - model location relative to the show file
- `position` - position for the model in the scene, formatted as `[x, y, z]`

#### Optional
- `scale` - scale across all axes _(default: 1)_
- `rotation` - degrees of rotation about each axis, formatted as `[x, y, z]` _(default `[0, 0, 0]`)_


### Spotlights

Section name: `spotLights`

A list of interactive spotlights that will light the 3D scene.

#### Required
- `position` - position of the spotlight in the scene, formatted as `[x, y, z]`
- `target` - formatted as `[x, y, z]`
- `address` - DMX address of the light (1 - 512)
- `ctemp` - colour temperature (Kelvin)
- `beamAngle` - beam angle in degrees
- `fieldAngle` - field angle in degrees

#### Optional
- `gel` - manufacturer gel string - must be defined in the `gel.colours` file
- `gobo` - masking pattern file name (without `.png` extension), located in the `gobos` directory
- `goboRotation` - the rotation, in degrees, of the mask pattern


### Point Lights

Section name: `pointLights`

A list of interactive point lights that will light the 3D scene.

#### Required
- `position` - _as per spot light_
- `address` - _as per spot light_
- `ctemp` - _as per spot light_

#### Optional

- `gel` - _as per spot light_


### Camera Presets

Section name: `camera.presets`

#### Required
- `key`: the numeric key that this preset will be mapped to `(0-9)`
- `position`: the position of the camera, formatted as `[x, y, z]`
- `target`: the camera's target, formatted as `[x, y, z]`


## Example (demo.vis)

```yaml
ambientIntensity: 0.01

models:
  - src: models/floor/floorWhite.obj
    position: [0, 0, 0]
  - src: models/armchair/armchair.obj
    scale: 0.01
    position: [2.5, 1.45, -2]
    rotation: [0, -145, 0]
  - src: models/tree/tree.obj
    scale: 2
    position: [3, 0, -3]

spotLights:
  - address: 1
    ctemp: 3200
    position: [1, 6, 3]
    target: [0, 2.8, 0]
    beamAngle: 30
    fieldAngle: 42
    gel: L201
  - address: 2
    ctemp: 5600
    position: [-1, 6, 3]
    target: [0, 2.8, 0]
    beamAngle: 20
    fieldAngle: 42
    cgel: L201
  - address: 3
    ctemp: 5600
    position: [-5, 6, -2]
    target: [0, 2.5, 0]
    beamAngle: 30
    fieldAngle: 40
    gel: L106
    gobo: 673
    goboRotation: -30

pointLights:
  - address: 31
    position: [0, 8, 0]
    ctemp: 5600
    gel: L728

camera:
  presets:
    - key: 1
      position: [0, 8, 6]
      target: [0, 2, 0]
```
