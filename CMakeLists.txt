cmake_minimum_required(VERSION 3.16)

if(WIN32 AND CMAKE_GENERATOR STREQUAL "NMake Makefiles")
    find_program(NINJA_EXECUTABLE ninja)
    if(NINJA_EXECUTABLE)
        set(CMAKE_GENERATOR "Ninja" CACHE STRING "CMake generator" FORCE)
    endif()
endif()

project(jhatkaa LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
    set(QT_WIDGETS_TARGET Qt5::Widgets)
else()
    set(QT_WIDGETS_TARGET Qt6::Widgets)
endif()

add_subdirectory(src)
