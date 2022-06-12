cmake_minimum_required(VERSION 3.10) 

# set the project name 
project(ToyEngine VERSION 1.0) 

set(CMAKE_CXX_STANDARD 20)


# Source
set(EngineSourcesDirectory ${CMAKE_CURRENT_LIST_DIR}/Engine/Sources)

## Render Module
set(RenderSourceDirectory ${EngineSourcesDirectory}/Render)
# should not use file(GLOB ...)
file(GLOB RenderIncludes "${RenderSourceDirectory}/Public/*.h")
aux_source_directory(${RenderSourceDirectory} RenderSources)
source_group(TREE ${RenderSourceDirectory} FILES ${RenderSources} ${RenderIncludes})


# ThirdParties
set(ThirdPartiesSourcesDirectory ${CMAKE_CURRENT_LIST_DIR}/Engine/ThirdParties)

## Vulkan
find_package(Vulkan REQUIRED)

## GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${ThirdPartiesSourcesDirectory}/glfw)


# Engine
# should not use file(GLOB ...)
file(GLOB EngineIncludes "${EngineSourcesDirectory}/*.h")
aux_source_directory(${EngineSourcesDirectory} EngineSources)

add_library(Engine ${RenderSources} ${RenderIncludes} ${EngineSources} ${EngineIncludes})
target_include_directories(Engine 
                            PUBLIC ${RenderSourceDirectory}/Public ${EngineSourcesDirectory})
target_link_libraries(Engine PUBLIC glfw Vulkan::Vulkan)


# Samples
set(SamplesDirectory ${CMAKE_CURRENT_LIST_DIR}/Samples/Sources)

aux_source_directory(${SamplesDirectory}/Win64 SamplesSources)
add_executable(Samples ${SamplesSources})
target_link_libraries(Samples PRIVATE Engine)
