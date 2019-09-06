cmake_minimum_required(VERSION 3.8)

project(visvalingam)
enable_testing()

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(GDAL REQUIRED)

add_library(visvalingam_algorithm
  src/ogr.cpp
  src/ogr.h
  src/csv.cpp
  src/csv.h
  src/polyline.cpp
  src/polyline.h
  src/visvalingam_algorithm.cpp
  src/visvalingam_algorithm.h
)
target_link_libraries(visvalingam_algorithm PUBLIC ${GDAL_LIBRARY})
target_include_directories(visvalingam_algorithm PUBLIC src ${GDAL_INCLUDE_DIR})

add_executable(simplify src/main.cpp)
target_link_libraries(simplify visvalingam_algorithm)

add_executable(test_visvalingam src/test_visvalingam.cpp)
target_link_libraries(test_visvalingam visvalingam_algorithm)
add_test(NAME test_visvalingam COMMAND test_visvalingam)
