# Development Guide

This guide provides in-depth information for developers working on the Render Engine Family.

## Architecture Overview

### Module Responsibilities

#### engine_core
**Purpose:** Foundational rendering infrastructure

**Key Components:**
- Camera system with view generation
- GLFW window management
- Mathematical utilities (Vec, Mat, Ray)
- Render loop templates
- Event handling (keyboard, mouse)

**Design Patterns:**
- Template-based for type flexibility
- Policy-based design for render modes
- RAII for resource management

#### model_render_core
**Purpose:** 3D model rendering pipeline

**Key Components:**
- Model representation (vertices, faces, UVs, normals)
- Material system with PBR textures
- Particle system with SoA layout
- Tiled rasterizer for parallel processing
- Blinn-Phong shader pipeline

**Design Patterns:**
- SoA (Structure of Arrays) for particles
- Tiled processing for cache efficiency
- Function object pattern for shaders

#### ray_marching_core
**Purpose:** Procedural rendering with SDFs

**Key Components:**
- SDF object interface
- Ray marching iterator
- Curvature calculation
- Scene composition

**Design Patterns:**
- Interface segregation for SDF objects
- Strategy pattern for curvature types

## Code Patterns

### Camera Usage

```cpp
// Create camera
sc::Camera<float, sc::VecArray> camera;
camera.pos() = {0.f, 0.f, 2.f};
camera.rot() = {0.f, 0.f, 0.f};
camera.setLen(0.3f);
camera.setRes({1000, 800});

// Generate view
auto view = sc::makeViewFromCamera(camera);

// Use in render
for (const auto& pixel : view) {
    // Process pixel
}
```

### Render Loop

**Per-Frame Rendering:**
```cpp
sc::initPerFrameRender(
    camera,
    [](GLFWRenderer& renderer, std::size_t frame, std::size_t time) {
        renderer.clear();
        // Draw frame
    },
    [](std::size_t frame, std::size_t time) {
        // Per-frame updates
    }
);
```

**Per-Pixel Rendering:**
```cpp
sc::initEachPixelRender(
    camera,
    [](const PixelSample& ps, std::size_t frame, std::size_t time) {
        return color; // Vec<float, 3>
    }
);
```

### Model Rendering

```cpp
// Load model
auto model = mrc::io::readFromObjFile<float>("model.obj");

// Create renderer
mrc::initMrcRender(
    camera,
    {model},
    {
        LightSource{
            {0.f, 3.f, 0.f},  // position
            {0.f, -1.f, 0.f}, // direction
            {1.f, 1.f, 1.f},  // color
            1.f               // intensity
        }
    }
);
```

### Particle System

```cpp
// Create particle system
mrc::ParticleSystem<float> particles;
mrc::makeCircleBillboard(particles, 8);  // 8-segment circle

// Set particle data
particles.positions = {{0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}};
particles.colors = {{1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}};
particles.sizes = {0.01f, 0.02f};
particles.enableRender = {true, true};

// Render
mrc::initMrcRender(camera, models, lights, {}, {}, {}, {}, 60, {}, &particles);
```

## Shader System

### Custom Shader

```cpp
auto customShader = [](const FragmentInput<float>& frag) -> sc::utils::Vec<float, 3> {
    // Access fragment data
    auto worldPos = frag.worldPos;
    auto normal = frag.normal;
    auto uv = frag.uv;
    auto color = frag.color;

    // Compute lighting
    // ...

    return resultColor;
};

// Use in render
mrc::initMrcRender(camera, models, lights, {}, {}, {}, {}, 60, customShader);
```

### Blinn-Phong Shader

The default `BlinnFongShaderFactory` provides:
- Albedo from color map or base color
- Tangent space normal mapping
- Roughness-based shininess
- Multi-light support

## Performance Considerations

### Tiled Rasterization

The tiled rasterizer divides the screen into tiles for cache efficiency:
- Default tile size: 8x8 pixels
- Reduces memory bandwidth
- Improves cache locality

### Particle System

SoA layout provides:
- Better cache utilization
- SIMD-friendly memory access
- Efficient filtering with `enableRender` array

### Optimization Tips

1. **Minimize allocations** in render loop
2. **Reuse buffers** across frames
3. **Precompute** static data
4. **Use appropriate precision** (float vs double)
5. **Enable compiler optimizations** (`-O3`)

## Debugging

### Enable Logging

```cpp
#define ENABLE_LOG
#include "entry_point.h"
```

This outputs:
- Frame number
- Time (ms)
- Render time (ms)
- FPS

### Common Issues

**Black Screen:**
- Check camera position (too far/close?)
- Verify models are loaded
- Enable logging to see if render loop runs

**Incorrect Colors:**
- Check material definitions
- Verify texture file paths
- Ensure normal map is in correct space

**Performance Issues:**
- Check polygon count
- Verify no unnecessary allocations
- Profile with profilers

## Extending the Engine

### Adding a New SDF Object

```cpp
namespace rmc::object {

template<typename NumericT>
class MySDF : public SceneObject<NumericT> {
public:
    NumericT distance(const Vec<NumericT, 3>& p) const override {
        // Return signed distance
        return std::sqrt(...) - radius;
    }
};

} // namespace rmc::object
```

### Adding a New Curvature Type

```cpp
namespace rmc::curvature {

template<typename NumericT>
class MyCurvature : public ICurvature<NumericT> {
public:
    Vec<NumericT, 3> transform(const Vec<NumericT, 3>& p) const override {
        // Transform point based on curvature
        return transformed;
    }
};

} // namespace rmc::curvature
```

### Custom Billboard Shape

```cpp
// Create custom billboard vertices
particles.billboardVerts = {
    {-1.f, -1.f},
    { 1.f, -1.f},
    { 0.f,  1.f},  // Triangle
};

// Define faces
particles.billboardFaces = {
    {0, 1, 2}
};
```

## Building Different Configurations

### Debug Build
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

### Release Build
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### RelWithDebInfo
```bash
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

### Disable Specific Modules

To disable a module, comment it out in root `CMakeLists.txt`:
```cmake
# add_subdirectory(sfm_core)  # Disabled
```

## Platform-Specific Notes

### Windows
- Use MSVC or MinGW
- Ensure vcpkg packages are installed for dependencies

### macOS
- Use Xcode or Clang
- Install dependencies via Homebrew
- May need to specify SDK path

### Linux
- Use GCC or Clang
- Install development packages:
  ```bash
  sudo apt install libglfw3-dev libeigen3-dev
  ```

## Further Reading

- [CMake Documentation](https://cmake.org/documentation/)
- [GLFW Documentation](https://www.glfw.org/documentation.html)
- [OpenGL Tutorial](https://learnopengl.com/)
- [Ray Marching](https://iquilezles.org/www/articles/raymarchingdf/raymarchingdf.htm)
- [Blinn-Phong Shading](https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_reflection_model)
