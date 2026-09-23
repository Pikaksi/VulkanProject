project('my_app', 'cpp',
  version : '0.1',
  default_options : ['warning_level=1', 'b_ndebug=if-release'])

src = [
  'src/Rendering/drawCallRecorder.cpp',
  'src/Rendering/sunShadows.cpp',
  'src/blockEntities/blockEntityCrafting.cpp',
  'src/blockEntities/blockEntity.cpp',
  'src/blockEntities/blockEntityManager.cpp',
  'src/util/threadPool.cpp',
  'src/util/stbi_images.cpp',
  'src/World/chunkMesher.cpp',
  'src/util/tlsfAllocator.cpp',
  'src/World/WorldGeneration/ChunkGenerator.cpp',
  'src/3dRendering/QuadStripIndexBuffer.cpp',
  'src/VulkanRendering/GraphicsPipeline.cpp',
  'src/VulkanRendering/VulkanUtilities.cpp',
  'src/3dRendering/WorldVertexTracker.cpp',
  'src/3dRendering/BinaryGreedyMesher.cpp',
  'src/2dRendering/UIHelperFunctions.cpp',
  'src/VulkanRendering/DeviceCreator.cpp',
  'src/Player/PlayerInventoryManager.cpp',
  'src/Rendering/VertexBufferManager.cpp',
  'src/2dRendering/UITextureCreator.cpp',
  'src/VulkanRendering/ImageCreator.cpp',
  'src/VulkanRendering/FrameDrawer.cpp',
  'src/Inventory/InventoryRenderer.cpp',
  'src/VulkanRendering/Descriptor.cpp',
  'src/Inventory/InventoryLayouts.cpp',
  'src/3dRendering/ChunkRenderer.cpp',
  'src/2dRendering/DefaultWindow.cpp',
  'src/VulkanRendering/SwapChain.cpp',
  'src/VulkanRendering/Commands.cpp',
  'src/Rendering/TextureCreator.cpp',
  'src/Rendering/GPUMemoryBlock.cpp',
  'src/VulkanRendering/Buffers.cpp',
  'src/2dRendering/DebugMenu.cpp',
  'src/2dRendering/UIManager.cpp',
  'src/World/BlockDataLookup.cpp',
  'src/Player/PlayerControls.cpp',
  'src/Inventory/Inventory.cpp',
  'src/2dRendering/UIText.cpp',
  'src/2dRendering/UIQuad.cpp',
  'src/World/WorldManager.cpp',
  'src/PlayerInputHandler.cpp',
  'src/Player/PlaceBlock.cpp',
  'src/Rendering/Vertex.cpp',
  'src/FilePathHandler.cpp',
  'src/CameraHandler.cpp',
  'src/World/Chunk.cpp',
  'src/Application.cpp',
  'src/main.cpp'
]

#meson wrap install glfw
glfw_dep = dependency('glfw', fallback: ['glfw', 'glfw_dep'])

#meson wrap install glm
glm_dep = dependency('glm')

vulkan_dep = dependency('vulkan', required: true)

cpp_compiler = meson.get_compiler('cpp')
if host_machine.system() == 'linux'
  fast_noise_lib = cpp_compiler.find_library('FastNoise', dirs: meson.current_source_dir() / 'dependencies/FastNoise2Linux/lib')
  fast_noise_dep = declare_dependency(
    include_directories : 'dependencies/FastNoise2Linux/include/',
    dependencies: fast_noise_lib
  )
elif host_machine.system() == 'windows'
  fast_noise_lib = cpp_compiler.find_library('FastNoise', dirs: meson.current_source_dir() / 'dependencies/FastNoise2Win/lib')
  fast_noise_dep = declare_dependency(
    include_directories : 'dependencies/FastNoise2Win/include/',
    dependencies: fast_noise_lib
  )
endif

dependencies = [
  glfw_dep,
  glm_dep,
  vulkan_dep,
  fast_noise_dep
]

if host_machine.system() == 'linux'
  fastNoise2IncludeOs = include_directories('dependencies/FastNoise2Linux/include')
elif host_machine.system() == 'windows'
  fastNoise2IncludeOs = include_directories('dependencies/FastNoise2Win/include')
endif

include_dirs = [
  include_directories('dependencies/stbImages'),

  fastNoise2IncludeOs,

  include_directories('src'),
  include_directories('src/2dRendering'),
  include_directories('src/3dRendering'),
  include_directories('src/Inventory'),
  include_directories('src/Player'),
  include_directories('src/Rendering'),
  include_directories('shaders'),
  include_directories('src/World'),
  include_directories('src/VulkanRendering'),
  include_directories('src/util'),
  include_directories('src/blockEntities'),
]

my_exe = executable('myexe',
  sources : src,
  dependencies : dependencies,
  include_directories : include_dirs,
  override_options : ['cpp_std=c++20'],
  cpp_args : ['-w'],
)

#if host_machine.system() == 'windows'
#install_data(meson.current_source_dir() / 'dependencies/FastNoise2Win/bin/FastNoise.dll', install_dir : 'bin/vulkanProject/bin')
#install_subdir(meson.current_source_dir() / 'shaders', install_dir : 'bin/vulkanProject/shaders', strip_directory : true)
#install_subdir(meson.current_source_dir() / 'textures', install_dir : 'bin/vulkanProject/textures', strip_directory : true)
#endif

run_target('run', command: my_exe)
