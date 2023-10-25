cmake_minimum_required(VERSION 3.22.1)
project("giflib")

file(GLOB libgif_src ${CMAKE_SOURCE_DIR}/giflib/*.c)
include_directories(${CMAKE_SOURCE_DIR}/giflib)
file(GLOB gif_src_c ${CMAKE_SOURCE_DIR}/gifjni/*.c)
file(GLOB gif_src_cpp ${CMAKE_SOURCE_DIR}/gifjni/*.cpp)

include_directories(${CMAKE_SOURCE_DIR})


add_library(${CMAKE_PROJECT_NAME}
        SHARED

        ${libgif_src}

        ${gif_src_c}

        ${gif_src_cpp})

target_link_libraries(${CMAKE_PROJECT_NAME}
        jnigraphics
        android
        log)