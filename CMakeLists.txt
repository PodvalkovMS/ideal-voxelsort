cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(zer-search)

find_package(VTK REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem program_options)


set(_include_dir ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE _headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${_include_dir}/*.h*)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/stdafx.cpp ${_headers} ${CMAKE_CURRENT_SOURCE_DIR}/src/vtk_viz.cpp)

set(_precompiled_header "stdafx.h")

if(MSVC)
target_compile_options(${PROJECT_NAME} PRIVATE /Yu${_precompiled_header} PRIVATE /FI${_precompiled_header})
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/stdafx.cpp PROPERTIES COMPILE_FLAGS /Yc${_precompiled_header})
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)
target_include_directories(${PROJECT_NAME} PUBLIC ${_include_dir} PUBLIC ${VTK_USE_FILE})
unset(_include_dir)
target_link_libraries(${PROJECT_NAME} PUBLIC ${VTK_LIBRARIES} PUBLIC Boost::filesystem PUBLIC Boost::boost PUBLIC Boost::program_options)
