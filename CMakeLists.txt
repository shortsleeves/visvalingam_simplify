cmake_minimum_required(VERSION 3.8)

project(visvalingam)
enable_testing()

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(VISVALINGAM_WITH_GDAL "Use GDAL OGR format" ON)
if(VISVALINGAM_WITH_GDAL)
    find_package(GDAL REQUIRED)
endif()

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
target_include_directories(visvalingam_algorithm PUBLIC src)
if(VISVALINGAM_WITH_GDAL)
    target_link_libraries(visvalingam_algorithm PUBLIC ${GDAL_LIBRARY})
    target_include_directories(visvalingam_algorithm PUBLIC ${GDAL_INCLUDE_DIR})
    target_compile_definitions(visvalingam_algorithm PUBLIC HAVE_GDAL)
endif()

add_executable(simplify src/main.cpp)
target_link_libraries(simplify visvalingam_algorithm)

add_executable(test_visvalingam src/test_visvalingam.cpp)
target_link_libraries(test_visvalingam visvalingam_algorithm)
add_test(NAME test_visvalingam COMMAND test_visvalingam)

install(TARGETS simplify DESTINATION bin COMPONENT app)
include(InstallRequiredSystemLibraries)
add_subdirectory(cmake/bundle)
