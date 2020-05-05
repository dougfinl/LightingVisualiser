# Lighting Visualiser

This application is a continuation of my university final-year project. It allows the user to assemble 3d objects on a virtual stage, define positions and properties of lighting instruments, and visualise the scene based on lighting data received by the [Open Lighting Architecture](https://www.openlighting.org/ola/).

![Simple visualiser scene](https://github.com/dougfinl/lightingvisualiser/raw/master/.assets/images/visualiser.png)


## Dependencies

- Assimp
- Boost 1.72
  - filesystem
  - log
  - log_setup
  - program_options
  - system
  - thread
- CMake
- GLEW
- GLFW3
- GLM
- Google Protobuf
- OLA (Open Lighting Architecture)
- OpenGL
- yaml-cpp


## Building

```
mkdir build
cd build
cmake ..
make -j4
```
Binary will be located at `build/visualiser`


## Usage

`./visualiser show_name.vis`

### Relative Directory Structure

__Note:__ currently, the executable must be located at the same level as the show files.

<pre><code><strong>demo</strong>
├── demo.vis
├── gel.colours
├── <strong>gobos</strong>
│   ├── 228.png
│   ├── 673.png
│   ├── 77053.png
│   └── ...
├── <strong>models</strong>
│   ├── <strong>armchair</strong>
│   │   ├── armchair.mtl
│   │   └── armchair.obj
│   ├── <strong>floor</strong>
│   │   └── ...
│   └── ...
├── <strong>shaders</strong>
│   └── ...
└── <em>visualiser</em></code></pre>

Read [`docs/showfilespec.md`](docs/showfilespec.md) for the show file specification.

Read [`docs/gelcoloursspec.md`](docs/gelcoloursspec.md) for the gel colours file specification.
