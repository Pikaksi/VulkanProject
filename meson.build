project('my_app', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3'])

src = [
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
  'src/VulkanRendering/RenderPass.cpp',
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

cc = meson.get_compiler('cpp')

vulkan_sdk_path = '/home/miika/vulkanSDK/1.4.350.1/x86_64/'

#vulkan_validation_layer_lib = cc.find_library('VkLayer_khronos_validation', dirs: vulkan_sdk_path / 'lib/')
#vulkan_validation_layer_dep = declare_dependency(
  #dependencies: vulkan_validation_layer_lib
#)

#vulkan_dep = dependency('vulkan')
vulkan_lib = cc.find_library('vulkan', dirs: vulkan_sdk_path / 'lib/VulkanLoader/lib/')
vulkan_dep = declare_dependency(
  include_directories : vulkan_sdk_path / 'include',
  dependencies: vulkan_lib
)

fast_noise_lib = cc.find_library('FastNoise', dirs: meson.current_source_dir() / 'dependencies/FastNoise2/lib')
fast_noise_dep = declare_dependency(
  include_directories : 'dependencies/FastNoise2/include/',
  #link_with : meson.current_source_dir() / 'dependencies/FastNoise2/lib/libFastNoise.so',
  #link_args : meson.current_source_dir() / 'dependencies/FastNoise2/lib/libFastNoise.so',
  dependencies: fast_noise_lib
)


deps = [
  glfw_dep,
  glm_dep,
  vulkan_dep,
  #vulkan_validation_layer_dep,
  fast_noise_dep
]

include_dirs = [
  include_directories('dependencies/stbImages'),
  include_directories('dependencies/FastNoise2/include/FastNoise'),
  include_directories('dependencies/FastNoise2/include/FastNoise/Utility'),
  include_directories('dependencies/FastNoise2/include/FastNoise/Generators'),
  include_directories('dependencies/FastNoise2/include/FastSIMD'),
  include_directories('dependencies/FastNoise2/include/FastSIMD/Utility'),

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
