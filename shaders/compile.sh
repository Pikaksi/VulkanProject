# In vulkanSDK/<version>x86_64/bin or glslc project
glslc shader3d.vert -o vert3d.spv
glslc shader3d.frag -o frag3d.spv

glslc shader2d.vert -o vert2d.spv
glslc shader2d.frag -o frag2d.spv

glslc sunShadows.vert -o vertSunShadows.spv
glslc sunShadows.frag -o fragSunShadows.spv

glslc lod.vert -o vertLod.spv
glslc lod.frag -o fragLod.spv
