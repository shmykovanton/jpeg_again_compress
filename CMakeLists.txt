cmake_minimum_required(VERSION 3.2)

SET(GCC_COVERAGE_COMPILE_FLAGS "")
SET(GCC_COVERAGE_LINK_FLAGS    "")


SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

add_subdirectory(3dparty/jpeg-6b)
include_directories(3dparty/jpeg-6b)


include_directories(src/jpeg)

add_executable(jpeg_again_compress src/main.cpp
                                   src/jpeg/jpeg_decompressor.cpp
                                   src/jpeg/jpeg_compressor.cpp
               )
target_link_libraries(jpeg_again_compress libjpeg)

