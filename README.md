### Performant Voxel renderer using Vulkan 

![image](https://github.com/user-attachments/assets/a0656f21-b1bc-4760-862b-cb3390fe99dd)

Plan is to make a factory building game.
Inspired by minecraft mods like gregtech.

### Features

Infinite build height

Binary greedy meshing

WPI inventory and factory components

### compiling

requires vulkan sdk 1.4
meson setup --wipe build --native-file clang.ini
cd build/
meson compile run
