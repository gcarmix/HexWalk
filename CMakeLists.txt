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

# Windows + MinGW: prefer static archives (.a) over import libraries (.dll.a)
# when CMake resolves library names. Without this, Qt6's WrapZSTD / WrapBLAKE2 /
# OpenMP find_library calls pick up the .dll.a first and the resulting .exe
# carries runtime dependencies on libzstd.dll, libb2-1.dll, libgomp-1.dll, etc.
# Must be set BEFORE find_package(Qt...) because Qt creates imported targets
# from these results that cannot be redirected later.
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a .lib)

    # WrapZSTD goes through find_package(zstd CONFIG), which creates both
    # zstd::libzstd_shared and zstd::libzstd_static targets, and Qt's wrapper
    # then picks the _shared one. Pre-create WrapZSTD::WrapZSTD pointing at
    # the static archive so Qt's "if(NOT TARGET WrapZSTD::WrapZSTD)" guard
    # leaves it intact.
    find_library(_ZSTD_STATIC NAMES libzstd.a zstd_static)
    find_path(_ZSTD_INCLUDE_DIR zstd.h)
    if(_ZSTD_STATIC AND _ZSTD_INCLUDE_DIR)
        add_library(WrapZSTD::WrapZSTD INTERFACE IMPORTED)
        set_target_properties(WrapZSTD::WrapZSTD PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${_ZSTD_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${_ZSTD_STATIC}")
        set(WrapZSTD_FOUND TRUE)
    endif()

    # Same pattern for libb2 (BLAKE2): Qt's FindLibb2 routes through
    # pkg_check_modules first and pkg-config's import library wins. The
    # FindLibb2.cmake short-circuits with `if(TARGET Libb2::Libb2) return()`,
    # so pre-creating the target keeps the build static.
    find_library(_LIBB2_STATIC NAMES libb2.a b2)
    find_path(_LIBB2_INCLUDE_DIR NAMES blake2.h)
    if(_LIBB2_STATIC AND _LIBB2_INCLUDE_DIR)
        add_library(Libb2::Libb2 UNKNOWN IMPORTED)
        set_target_properties(Libb2::Libb2 PROPERTIES
            IMPORTED_LOCATION "${_LIBB2_STATIC}"
            INTERFACE_INCLUDE_DIRECTORIES "${_LIBB2_INCLUDE_DIR}")
        set(Libb2_FOUND TRUE)
    endif()
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Charts)

# Capstone: static library, no tests or extra tools
set(BUILD_SHARED_LIBS       OFF CACHE BOOL "" FORCE)
set(CAPSTONE_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(CAPSTONE_BUILD_CSTOOL   OFF CACHE BOOL "" FORCE)

add_subdirectory(capstone)
add_subdirectory(hexdig)
add_subdirectory(hexwalk)