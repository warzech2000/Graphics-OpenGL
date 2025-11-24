# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "D:/Grafika 3D/Base/3d 2/3d/out/build/x64-Debug/_deps/glfw-src")
  file(MAKE_DIRECTORY "D:/Grafika 3D/Base/3d 2/3d/out/build/x64-Debug/_deps/glfw-src")
endif()
file(MAKE_DIRECTORY
  "D:/Grafika 3D/Base/3d 2/3d/out/build/x64-Debug/_deps/glfw-build"
  "D:/Grafika 3D/Base/3d 2/3d/out/build/x64-Debug/_deps/glfw-subbuild/glfw-populate-prefix"
  "D:/Grafika 3D/Base/3d 2/3d/out/build/x64-Debug/_deps/glfw-subbuild/glfw-populate-prefix/tmp"
  "D:/Grafika 3D/Base/3d 2/3d/out/build/x64-Debug/_deps/glfw-subbuild/glfw-populate-prefix/src/glfw-populate-stamp"
  "D:/Grafika 3D/Base/3d 2/3d/out/build/x64-Debug/_deps/glfw-subbuild/glfw-populate-prefix/src"
  "D:/Grafika 3D/Base/3d 2/3d/out/build/x64-Debug/_deps/glfw-subbuild/glfw-populate-prefix/src/glfw-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Grafika 3D/Base/3d 2/3d/out/build/x64-Debug/_deps/glfw-subbuild/glfw-populate-prefix/src/glfw-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Grafika 3D/Base/3d 2/3d/out/build/x64-Debug/_deps/glfw-subbuild/glfw-populate-prefix/src/glfw-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
