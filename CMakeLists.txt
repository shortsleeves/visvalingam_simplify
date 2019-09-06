cmake_minimum_required(VERSION 3.8)

project(visvalingam)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(GDAL REQUIRED)

add_library(visvalingam_algorithm
  src/ogr.cpp
  src/polyline.cpp
  src/visvalingam_algorithm.cpp
)
target_link_libraries(visvalingam_algorithm PUBLIC ${GDAL_LIBRARY})
target_include_directories(visvalingam_algorithm PUBLIC src ${GDAL_INCLUDE_DIR})

add_executable(simplify src/main.cpp)
target_link_libraries(simplify visvalingam_algorithm)

