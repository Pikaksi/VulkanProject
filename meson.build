project('my_app', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3'])

src = [
  'src/util/tlsfAllocator.cpp',
  'src/World/WorldGeneration/ChunkGenerator.cpp',
  'src/3dRendering/BlockTexCoordinateLookup.cpp',
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
  'src/3dRendering/VertexCreator.cpp',
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
  'src/ECS/EntityManager.cpp',
  'src/ECS/EntityCreator.cpp',
  'src/Rendering/Vertex.cpp',
  'src/FilePathHandler.cpp',
  'src/CameraHandler.cpp',
  'src/World/Chunk.cpp',
  'src/Application.cpp',
  'src/main.cpp'
]

glfw_dep = dependency('glfw', fallback: ['glfw', 'glfw_dep'])
#meson wrap install glfw

glm_dep = dependency('glm')
#meson wrap install glm

cc = meson.get_compiler('cpp')

vulkan_dep = dependency('vulkan', required: false)
if not vulkan_dep.found()
  vulkan_sdk_path = '/home/miika/vulkanSDK/1.4.350.1/x86_64/'
  vulkan_lib = cc.find_library('vulkan', dirs: vulkan_sdk_path / 'lib/VulkanLoader/lib/')
  vulkan_dep = declare_dependency(
    include_directories : vulkan_sdk_path / 'include',
    dependencies: vulkan_lib
  )
endif

if host_machine.system() == 'linux'
  fast_noise_lib = cc.find_library('FastNoise', dirs: meson.current_source_dir() / 'dependencies/FastNoise2Linux/lib')
  fast_noise_dep = declare_dependency(
    include_directories : 'dependencies/FastNoise2Linux/include/',
    dependencies: fast_noise_lib
  )
else
  fast_noise_lib = cc.find_library('FastNoise', dirs: meson.current_source_dir() / 'dependencies/FastNoise2Win/lib')
  fast_noise_dep = declare_dependency(
    include_directories : 'dependencies/FastNoise2Win/include/',
    dependencies: fast_noise_lib
  )
endif

deps = [
  glfw_dep,
  glm_dep,
  vulkan_dep,
  #vulkan_validation_layer_dep,
  fast_noise_dep
]

if host_machine.system() == 'linux'
  fastNoise2IncludeOs = include_directories('dependencies/FastNoise2Linux/include')
else
  fastNoise2IncludeOs = include_directories('dependencies/FastNoise2Win/include')
endif
include_dirs = [
  include_directories('dependencies/stbImages'),
  
  fastNoise2IncludeOs,
  #include_directories('dependencies/FastNoise2/include/FastNoise/Utility'),
  #include_directories('dependencies/FastNoise2/include/FastNoise/Generators'),
  #include_directories('dependencies/FastNoise2/build/_deps/fastsimd-src/include/FastSIMD'),
  #include_directories('dependencies/FastNoise2/build/_deps/fastsimd-src/include/FastSIMD/Utility'),
  #include_directories('dependencies/FastNoise2/include/FastSIMD'),
  #include_directories('dependencies/FastNoise2/include/FastSIMD/Utility'),

  include_directories('src'),
  include_directories('src/2dRendering'),
  include_directories('src/3dRendering'),
  include_directories('src/ECS'),
  include_directories('src/Inventory'),
  include_directories('src/Player'),
  include_directories('src/Rendering'),
  include_directories('shaders'),
  include_directories('src/World'),
  include_directories('src/VulkanRendering'),
  include_directories('src/util'),
]

if meson.get_compiler('cpp').get_id() == 'clang++'
extra_args = ['-w']
else
extra_args = []
endif

my_exe = executable('myexe',
  sources : src,
  dependencies : deps,
  include_directories : include_dirs,
  override_options : ['cpp_std=c++20'],
  c_args : extra_args
)

run_env = environment()
layer_path = vulkan_sdk_path / 'share/vulkan/explicit_layer.d'
run_env.set('VK_LAYER_PATH', layer_path)

run_target('run',
  command: my_exe, # Pass the executable() object you defined earlier
  env: run_env
)
