cmake_minimum_required(VERSION 3.16)

project(HexWalk
    VERSION 2.0.0
    DESCRIPTION "Hex Editor/Viewer/Analyzer"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Charts)

# Capstone: static library, no tests or extra tools
set(BUILD_SHARED_LIBS       OFF CACHE BOOL "" FORCE)
set(CAPSTONE_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(CAPSTONE_BUILD_CSTOOL   OFF CACHE BOOL "" FORCE)

add_subdirectory(capstone)
add_subdirectory(hexdig)
add_subdirectory(hexwalk)