# In dependencies,

GLEW, GLFW, glm include folders are hidden.

GLEW, GLFW's library files are hidden.

Read .gitignore file

If you add all the hidden things into proper folder, you will get the intended result.

# How to execute

There are some own test files.

`#include "renderers/basic_triangle.hpp"      // btri`

`#include "renderers/colored_triangle.hpp" // ctri`

`#include "renderers/colored_triangle_3d.hpp" // ctri3d`

`#include "renderers/utah_teapot.hpp" // utah`

`#include "renderers/parallel_projection_test.hpp" // pptest`

Each header file has different namespace. You can test any of them by changing some parts of `main.cpp` file.

1. Include header file what you wanna test.

2. Write only one `using namespace <name>`. You can check the distinct name of headers in each `~.hpp` files.

3. Run!

## Common features

- Some file can rotate camera by `dragging` and modify fovy by `scrolling`

## utah_teapot.hpp

- `Up/Down arrow`: Increase/Decrease light intensity.

- `Left/Right arrow`: Increase/Decrease shininess.

- `Space`: Rotate the light position by y-axis.

- `B`: Switch shading between **Phong lighting** and **Blinn-Phong lighting**.

- `G`: Toggle `Gamma Correction`.

## parallel_projection_test.hpp

- `Left Click`: Switch projection method between **Perspective** and **Parallel**.
