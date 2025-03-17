cmake_minimum_required(VERSION 3.10) 

# set the project name 
project(ToyEngine VERSION 1.0) 

set(CMAKE_CXX_STANDARD 20)


# Source
set(EngineSourcesDirectory ${CMAKE_CURRENT_LIST_DIR}/Engine/Sources)

## Graphic Module
set(GraphicSourceDirectory ${EngineSourcesDirectory}/Graphic)
# should not use file(GLOB ...)
file(GLOB GraphicIncludes "${GraphicSourceDirectory}/*.h")
aux_source_directory(${GraphicSourceDirectory}/Src GraphicSources)
source_group(TREE ${EngineSourcesDirectory} FILES ${GraphicSources} ${GraphicIncludes})

## Input Module
set(InputSourceDirectory ${EngineSourcesDirectory}/Input)
# should not use file(GLOB ...)
file(GLOB InputIncludes "${InputSourceDirectory}/*.h")
aux_source_directory(${InputSourceDirectory}/Src GraphicSources)
source_group(TREE ${EngineSourcesDirectory} FILES ${InputSources} ${InputIncludes})

## Render Module
set(RenderSourceDirectory ${EngineSourcesDirectory}/Render)
# should not use file(GLOB ...)
file(GLOB RenderIncludes "${RenderSourceDirectory}/*.h")
aux_source_directory(${RenderSourceDirectory}/Src RenderSources)
source_group(TREE ${EngineSourcesDirectory} FILES ${RenderSources} ${RenderIncludes})

## Scene Module
set(SceneSourceDirectory ${EngineSourcesDirectory}/Scene)
# should not use file(GLOB ...)
file(GLOB SceneIncludes "${SceneSourceDirectory}/*.h")
aux_source_directory(${SceneSourceDirectory}/Src SceneSources)
source_group(TREE ${EngineSourcesDirectory} FILES ${SceneSources} ${SceneIncludes})

## Resource Module
set(ResourceSourceDirectory ${EngineSourcesDirectory}/Resource)
# should not use file(GLOB ...)
file(GLOB ResourceIncludes "${ResourceSourceDirectory}/*.h")
aux_source_directory(${ResourceSourceDirectory}/Src ResourceSources)
source_group(TREE ${EngineSourcesDirectory} FILES ${ResourceSources} ${ResourceIncludes})


# ThirdParties
set(ThirdPartiesSourcesDirectory ${CMAKE_CURRENT_LIST_DIR}/Engine/ThirdParties)

## Vulkan
find_package(Vulkan REQUIRED)

## GLM
add_subdirectory(${ThirdPartiesSourcesDirectory}/glm)

## GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${ThirdPartiesSourcesDirectory}/glfw)

## Tinyobjloader
add_subdirectory(${ThirdPartiesSourcesDirectory}/tinyobjloader)

## stb_image
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE ${ThirdPartiesSourcesDirectory}/stb)


# Engine
# should not use file(GLOB ...)
file(GLOB EngineIncludes "${EngineSourcesDirectory}/*.h")
aux_source_directory(${EngineSourcesDirectory} EngineSources)

add_library(Engine ${GraphicSources} ${GraphicIncludes} ${InputSources} ${InputIncludes} ${RenderSources} ${RenderIncludes} ${SceneSources} ${SceneIncludes} ${ResourceSources} ${ResourceIncludes} ${EngineSources} ${EngineIncludes})
target_include_directories(Engine 
                            PUBLIC ${EngineSourcesDirectory}
                            PRIVATE ${GraphicSourceDirectory} ${InputSourceDirectory} ${RenderSourceDirectory} ${SceneSourceDirectory} ${ResourceSourceDirectory})
target_link_libraries(Engine PUBLIC glfw glm::glm Vulkan::Vulkan tinyobjloader stb_image)


# Samples
set(SamplesDirectory ${CMAKE_CURRENT_LIST_DIR}/Samples/Sources)

## Sample Component Module
set(SampleComponentDirectory ${SamplesDirectory}/Components)
# should not use file(GLOB ...)
file(GLOB SampleComponentIncludes "${SampleComponentDirectory}/*.h")
aux_source_directory(${SampleComponentDirectory} SampleComponentSources)

## Sample platform
file(GLOB SamplePlatformIncludes "${SamplesDirectory}/Win64/*.h")
aux_source_directory(${SamplesDirectory}/Win64 SamplePlatformSources)

add_executable(Samples ${SampleComponentSources} ${SampleComponentIncludes} ${SamplePlatformSources} ${SamplePlatformIncludes}) 
target_include_directories(Samples PRIVATE ${SamplesDirectory})
target_link_libraries(Samples PRIVATE Engine)
