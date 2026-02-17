# Contributing to Render Engine Family

Thank you for your interest in contributing! This document provides guidelines for contributing to the Render Engine Family project.

## Getting Started

### Prerequisites

Before contributing, ensure you have:
- CMake 3.15 or higher
- A C++20 compatible compiler (GCC 10+, Clang 12+, MSVC 19.28+)
- Git installed
- Familiarity with C++ and computer graphics concepts

### Setting Up Development Environment

```bash
# Clone your fork (replace with your fork URL)
git clone <your-fork-url>/render_engine_family.git
cd render_engine_family

# Create a build directory
mkdir build && cd build

# Configure and build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .

# Run examples to verify (from build directory)
cd build
./engine_core_examples/simple_ray_tracer
```

## Development Workflow

### Branching Strategy

- `main`/`master` - Main stable branch (check your repo)
- `docs/*` - Documentation changes
- `feature/*` - New features
- `fix/*` - Bug fixes
- `example/*` - New examples

### Making Changes

1. Create a new branch from main/master:
   ```bash
   git checkout main || git checkout master
   git pull
   git checkout -b feature/your-feature-name
   ```

2. Make your changes following the code style guidelines

3. Build and test your changes:
   ```bash
   cd build
   cmake --build .
   ./your_example_name
   ```

4. Commit with descriptive messages:
   ```bash
   git add .
   git commit -m "Add: feature description"
   ```

5. Push to your fork:
   ```bash
   git push origin feature/your-feature-name
   ```

6. Create a pull request

## Code Style Guidelines

### C++20 Features

This project actively uses modern C++20 features:

- **Templates** for generic algorithms
- **Lambda expressions** for callbacks
- **Structured bindings**
- `auto` type deduction where clear
- `std::optional` for optional values
- `std::filesystem` for path operations

### Naming Conventions

- **Classes/Structs**: `PascalCase` (e.g., `Camera`, `Model`)
- **Functions**: `camelCase` (e.g., `initMrcRender`, `loadModel`)
- **Variables**: `camelCase` (e.g., `cameraPos`, `meshCount`)
- **Constants**: `UPPER_CASE` (e.g., `MAX_PARTICLES`)
- **File names**: `snake_case` (e.g., `main_pipeline.h`, `particle_system.h`)

### Code Organization

```cpp
#pragma once

// Includes first
#include "relative_header.h"
#include <standard_library>

// Namespace declarations
namespace my_namespace {

// Then class definitions
class MyClass {
    // Public interface first
public:
    MyClass();
    ~MyClass();

    void publicMethod();

    // Then protected
protected:
    void protectedMethod();

    // Then private
private:
    void privateMethod();

    // Member variables at the end
    Type member_variable_;
};

} // namespace my_namespace
```

### Comments

- Use `///` for documentation comments
- Use `//` for inline comments
- Keep comments concise and up-to-date
- Comment non-obvious algorithms only

### Formatting

- Use 4 spaces for indentation (no tabs)
- Max line length: 120 characters
- One statement per line
- Braces on new lines (Allman style preferred in headers)

## Adding Examples

### Creating a New Example

1. Create a new directory in `examples/`
2. Add `CMakeLists.txt`:
   ```cmake
   add_executable(my_example main.cpp)
   target_link_libraries(my_example PRIVATE engine_core model_render_core)
   ```
3. Add to root `CMakeLists.txt`:
   ```cmake
   add_subdirectory(examples/my_example)
   ```
4. Implement `main.cpp`:
   ```cpp
   #include "entry_point.h"
   #include "main_pipeline.h"
   #include "model/io.h"

   int main() {
       // Setup camera
       sc::Camera<float, sc::VecArray> camera;
       camera.pos()[2] = 2.0f;
       camera.setLen(0.3);

       // Load models
       std::vector<mrc::Model<float>> models;
       auto model = mrc::io::readFromObjFile<float>("model.obj");
       models.push_back(model);

       // Setup lights
       std::vector<mrc::LightSource<float>> lights;
       lights.emplace_back(
           sc::utils::Vec<float, 3>{0.f, 3.f, 0.f},
           sc::utils::Vec<float, 3>{0.f, -1.f, 0.f},
           sc::utils::Vec<float, 3>{1.f, 1.f, 1.f},
           1.f
       );

       // Start rendering
       mrc::initMrcRender(camera, models, lights);

       return 0;
   }
   ```

### Example Guidelines

- Keep examples self-contained
- Use descriptive names
- Add comments explaining key concepts
- Include assets in the example directory
- Document controls and keyboard shortcuts

## Adding Assets

### Model Files

Place OBJ/MTL files in your example's directory:
```
examples/my_example/
├── CMakeLists.txt
├── main.cpp
├── model.obj
└── model.mtl
```

### Texture Files

Supported formats: JPG, PNG (via stb_image)
```
examples/my_example/
├── ...
├── color.jpg
├── normal.jpg
└── roughness.jpg
```

## Testing

### Running Examples

Before submitting, ensure all examples compile and run:
```bash
cd build
cmake --build .
./engine_core_examples/simple_ray_tracer
./model_render_core_example
./particles_exmaple
# ... other examples
```

### Testing Checklist

- [ ] Code compiles without warnings
- [ ] All examples run without crashes
- [ ] New feature works as expected
- [ ] Documentation is updated
- [ ] Code follows style guidelines

## Pull Request Process

### Creating a Pull Request

1. Push your branch to your fork
2. Go to the GitHub repository
3. Click "New Pull Request"
4. Select your branch
5. Fill in the PR template:
   - **Title**: Brief description (e.g., "Add: normal map support")
   - **Description**: Detailed explanation of changes
   - **Related Issues**: Link to any issues this addresses

### PR Template

```markdown
## Description
Brief description of what this PR does.

## Changes
- Bullet points of main changes

## Testing
How you tested this PR.

## Checklist
- [ ] Code compiles
- [ ] Examples run successfully
- [ ] Documentation updated
- [ ] Style guidelines followed
```

## Reporting Issues

When reporting bugs or issues, please include:

- **Description**: Clear description of the issue
- **Steps to Reproduce**: Detailed steps to recreate the issue
- **Expected Behavior**: What you expected to happen
- **Actual Behavior**: What actually happened
- **Environment**: OS, compiler, CMake version
- **Screenshots**: If applicable

## Questions

Feel free to open an issue for questions about:
- Implementation details
- Best practices
- Feature requests
- Documentation improvements

## Recognition

Contributors will be acknowledged in the project documentation. Thank you for making Render Engine Family better!
