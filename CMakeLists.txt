cmake_minimum_required(VERSION 3.16)
project(jhatkaa LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Make CMake look in common Qt locations used by MSYS2/MinGW and custom installs.
if(DEFINED ENV{QTDIR})
    list(APPEND CMAKE_PREFIX_PATH "$ENV{QTDIR}")
endif()
if(DEFINED ENV{QT_ROOT})
    list(APPEND CMAKE_PREFIX_PATH "$ENV{QT_ROOT}")
endif()
if(MINGW)
    list(APPEND CMAKE_PREFIX_PATH
        "C:/msys64/mingw64"
        "C:/msys64/ucrt64"
        "C:/msys64/clang64"
    )
endif()

find_package(Qt6 COMPONENTS Widgets Sql REQUIRED)
set(QT_WIDGETS_TARGET Qt6::Widgets)
set(QT_SQL_TARGET Qt6::Sql)

message(STATUS "Using Qt targets: ${QT_WIDGETS_TARGET}, ${QT_SQL_TARGET}")

add_subdirectory(src)
