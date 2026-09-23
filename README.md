### Performant Voxel renderer using Vulkan 

![image](https://github.com/user-attachments/assets/a0656f21-b1bc-4760-862b-cb3390fe99dd)

Plan is to make a factory building game.
Inspired by minecraft mods like gregtech.

### Features

Infinite build height

Binary greedy meshing

WPI inventory and factory components

shadows (not in image)

### Compiling

requires vulkan sdk 1.4. When installing a tarball there is a setup-env.sh file.

meson setup --wipe build --native-file clang.ini
cd build/
meson compile run

#### Address sanitizer and undefined behaviour sanitizer setup
meson setup --wipe build --native-file clang.ini -Dc_link_args="-L/usr/lib/llvm-18/lib/clang/18/lib/linux -Wl,-rpath=/usr/lib/llvm-18/lib/clang/18/lib/linux" -Dcpp_link_args="-L/usr/lib/llvm-18/lib/clang/18/lib/linux -Wl,-rpath=/usr/lib/llvm-18/lib/clang/18/lib/linux" -Db_sanitize=address -Db_sanitize=undefined -Db_lundef=false

#### Release build
meson setup --wipe buildRelease --native-file clang.ini --buildtype=release
