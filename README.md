This is an example implementation of the *closest point on mesh* algorithm. Given any 3D point and any mesh, it should find the closest point on the mesh to the given point in a maximum given radius. A GUI comes with it to demonstrate the results and performance.

- :notebook: [Documentation](https://oktomus.com/cpp-closest-point-on-mesh)
- :boom: [Try it](https://github.com/oktomus/cpp-closest-point-on-mesh/releases)

![demo](demo.gif)
![demo](demo2.gif)

# Performance

Tests were made with an AMD Ryzen 5 3600 @ 3.59Ghz. The implemntation only use one core.
In the following tests, all vertices were normalized in the range [-1, 1].

The obj files I have used are available in [common-3d-test-models](https://github.com/alecjacobson/common-3d-test-models).

- Pre-process time: KDTree generation of the mesh point cloud
- Compute time: finding the closest point on the mesh of a given point

| OBJ | Vertex count | Triangles count | Pre-process time | Compute time (1 query) | Compute time (1000 query) |
|----|-----|-----|----|----|-----|
|teapot.obj|~19k|~6k|1ms|<1ms|~11ms|
|fandisk.obj|~39k|~13k|2ms|<1ms|~18ms|
|stanford-bunny.obj|~209k|~70k|23ms|<1ms|~26ms|
|armadillo.obj|~300k|~100k|36ms|<1ms|~40ms|
|xyzrgb_dragon.obj|~750k|~250k|77ms|<1ms|~40ms|

The search radius doesn't effectively affect the performances because of the way I'm using the KDTree. Although, if the query point is far away from the mesh, the compute time will increase. A simple point to bbox distance check could avoid that.

# Algorithm

I am using a point cloud with a KDTree to find the closest point on the mesh. 

**Before-hand:**

1. Compute a point cloud of the mesh
    1. Add vertices in the point cloud
    2. Generate points on the mesh to increase the point cloud precision **Not implemented**
2. Build a KDTree containing the point cloud

**When requesting the closest point:**

1. Find the N closest points in the point cloud using the KDTree
2. For each of these points
    1. Get the mesh triangle on which the point is
    2. Compute the closest point to the query point that is on the triangle
3. Keep the closest point to the query point

**Requirements:**

- The mesh needs to be uniform
- The search must always be higher than the size of one triangle

# Build

This program works only on Linux and require to install the following dependencies:
- assimp
- glfw

```sh
$ git clone git@github.com:oktomus/cpp-closest-point-on-mesh.git
$ cd cpp-closest-point-on-mesh
$ mkdir build && cd build;
$ cmake -DCMAKE_BUILD_TYPE=Release .. && make -j # Build
$ ./app.gui # Run the GUI
```

**Used thirdparties:**

- [nanoflann](https://github.com/jlblancoc/nanoflann): KDTree implementation
- GLFW
- GLAD
- [assimp](https://github.com/assimp/assimp): Assets importer
- [imgui](https://github.com/ocornut/imgui): GUI

# Generate the docs

First, install `doxygen`.

```sh
$ doxygen doxygen
$ firefox docs/html/index.html # Open it
```

# Tests

I didn't write any tests for this application. But if I do so, here is what I will do:
- Write unit tests for the low-level math functions
- Write test files to assert that the implementation gives correct result for a given mesh

# Possible improvements

- Generate more points in the mesh point cloud to support any type of mesh.
- When using the KDTree, find the N closest points **that are in the given search radius**. Right now I only consider the N closest points, even if they are too far away.
- Use SIMD instructions to compute distances and closest point on triangle.
- Make the algorithm multithread by splitting the point cloud in N buckets where N. Then simply give a bucket to each thread.

# References

- [1](https://github.com/bronzelion/closest-point-on-mesh/wiki/Closest-Point-on-a-Mesh-:-Background-and-Approaches): Closest Point on a Mesh : Background and Approaches
- [2](http://jcgt.org/published/0003/04/05/paper.pdf): Fast Distance Queries for Triangles, Lines, andPoints using SSE Instructions
- [3](https://www.geometrictools.com/Documentation/DistancePoint3Triangle3.pdf): Distance Between Point and Triangle in 3D