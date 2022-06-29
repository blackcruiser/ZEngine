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


# Engine
# should not use file(GLOB ...)
file(GLOB EngineIncludes "${EngineSourcesDirectory}/*.h")
aux_source_directory(${EngineSourcesDirectory} EngineSources)

add_library(Engine ${GraphicSources} ${GraphicIncludes} ${InputSources} ${InputIncludes} ${RenderSources} ${RenderIncludes} ${SceneSources} ${SceneIncludes} ${EngineSources} ${EngineIncludes})
target_include_directories(Engine 
                            PUBLIC ${EngineSourcesDirectory}
                            PRIVATE ${GraphicSourceDirectory} ${InputSourceDirectory} ${RenderSourceDirectory} ${SceneSourceDirectory})
target_link_libraries(Engine PUBLIC glfw glm::glm Vulkan::Vulkan)


# Samples
set(SamplesDirectory ${CMAKE_CURRENT_LIST_DIR}/Samples/Sources)

aux_source_directory(${SamplesDirectory}/Win64 SamplesSources)
add_executable(Samples ${SamplesSources})
target_link_libraries(Samples PRIVATE Engine)
