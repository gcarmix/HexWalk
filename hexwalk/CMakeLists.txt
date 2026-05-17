set(SOURCES
    # qhexedit component (compiled directly into the app)
    ../qhexedit/bytepattern.cpp
    ../qhexedit/colortag.cpp
    ../qhexedit/tagparser.cpp
    ../qhexedit/qhexedit.cpp
    ../qhexedit/chunks.cpp
    ../qhexedit/commands.cpp
    # application
    advancedsearchdialog.cpp
    binanalysisdialog.cpp
    hexdigworker.cpp
    bytemap.cpp
    bytemapdialog.cpp
    converterwidget.cpp
    diffdialog.cpp
    disasmwidget.cpp
    edittagdialog.cpp
    entropychart.cpp
    entropydialog.cpp
    hashdialog.cpp
    hexwalkmain.cpp
    main.cpp
    optionsdialog.cpp
    searchdialog.cpp
    stringsdialog.cpp
    tagsdialog.cpp
    worditemdelegate.cpp
    # UI forms
    advancedsearchdialog.ui
    binanalysisdialog.ui
    bytemapdialog.ui
    converterwidget.ui
    diffdialog.ui
    disasmwidget.ui
    edittagdialog.ui
    entropydialog.ui
    hashdialog.ui
    hexwalkmain.ui
    optionsdialog.ui
    searchdialog.ui
    stringsdialog.ui
    tagsdialog.ui
    # Qt resources
    hexwalk.qrc
)

# Windows: embed application icon via a .rc file
if(WIN32)
    set(WIN_RC "${CMAKE_CURRENT_BINARY_DIR}/hexwalk.rc")
    file(WRITE "${WIN_RC}" "IDI_ICON1 ICON \"${CMAKE_CURRENT_SOURCE_DIR}/images/hexwalk.ico\"\n")
    list(APPEND SOURCES "${WIN_RC}")
endif()

# macOS: bundle the .icns icon
if(APPLE)
    set(MACOS_ICON "${CMAKE_CURRENT_SOURCE_DIR}/images/hexwalk.icns")
    set_source_files_properties("${MACOS_ICON}" PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
    list(APPEND SOURCES "${MACOS_ICON}")
endif()

# qt_add_executable is the modern Qt6 way; fall back to add_executable for Qt5
if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(hexwalk WIN32 MACOSX_BUNDLE ${SOURCES})
else()
    add_executable(hexwalk WIN32 MACOSX_BUNDLE ${SOURCES})
endif()

target_compile_definitions(hexwalk PRIVATE
    QHEXEDIT_EXPORTS
    APP_VERSION="${PROJECT_VERSION}"
    # GCC 15 + C++17: std::byte conflicts with the byte typedef in older MinGW Windows SDK
    # headers (rpcndr.h). WIN32_LEAN_AND_MEAN prevents pulling in the COM subsystem headers
    # that trigger the conflict. This can be dropped once Qt6 + updated MinGW headers are used.
    $<$<AND:$<BOOL:${WIN32}>,$<CXX_COMPILER_ID:GNU>>:WIN32_LEAN_AND_MEAN>
)

target_include_directories(hexwalk PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../qhexedit"
    "${CMAKE_CURRENT_SOURCE_DIR}/../capstone/include"
)

# Tell AUTOUIC where to find .ui files when processing qhexedit sources
set_target_properties(hexwalk PROPERTIES
    AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(hexwalk PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
    capstone
    hexdig_lib
)

# Windows: static linking of GCC runtime + extra Win32 libs required by static Qt
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(hexwalk PRIVATE
        -no-pie
        -static-libgcc
        -static-libstdc++
        -static
    )
    # These system libraries are pulled in implicitly by dynamic Qt but must be
    # listed explicitly when linking against a static Qt build.
    if(NOT BUILD_SHARED_LIBS)
        target_link_libraries(hexwalk PRIVATE
            dwmapi uxtheme setupapi opengl32 glu32
            ws2_32 winmm imm32 ole32 oleaut32 uuid
            version winspool shlwapi
        )
    endif()
endif()

# Qt6 static build: auto-import platform and style plugins via CMake
if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_import_plugins(hexwalk
        INCLUDE Qt6::QWindowsIntegrationPlugin
                Qt6::QWindowsVistaStylePlugin
    )
endif()

if(APPLE)
    set_target_properties(hexwalk PROPERTIES
        MACOSX_BUNDLE_ICON_FILE      hexwalk.icns
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
    )
endif()

set_target_properties(hexwalk PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

# Bundle a portable 7-Zip distribution alongside the executable. Required for
# extractors that delegate to 7-Zip (sevenzip, deb, squashfs, ...). Drop the
# appropriate files into vendor/7z/ (download from https://www.7-zip.org/):
#   Windows full:    7z.exe + 7z.dll  → SquashFS, ext, ISO, archives, ...
#   Windows minimal: 7za.exe (archives only) or 7zr.exe (.7z only)
#   Linux:           7zz   (statically linked binary from 7-zip.org)
#   macOS:           7zz   (placed inside the .app bundle's MacOS directory)
set(_7Z_VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendor/7z")
if(WIN32)
    set(_7Z_NAMES 7z.exe 7za.exe 7zr.exe)
    # 7z.exe loads format/codec code from 7z.dll at runtime — copy it alongside.
    set(_7Z_COMPANIONS 7z.dll)
else()
    set(_7Z_NAMES 7zz 7z)
    set(_7Z_COMPANIONS)
endif()

if(APPLE)
    set(_7Z_DEST_DIR "$<TARGET_BUNDLE_CONTENT_DIR:hexwalk>/MacOS")
else()
    set(_7Z_DEST_DIR "$<TARGET_FILE_DIR:hexwalk>")
endif()

foreach(_name IN LISTS _7Z_NAMES)
    if(EXISTS "${_7Z_VENDOR_DIR}/${_name}")
        add_custom_command(TARGET hexwalk POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_7Z_VENDOR_DIR}/${_name}"
                "${_7Z_DEST_DIR}/${_name}"
        )
        message(STATUS "Bundling ${_name} from ${_7Z_VENDOR_DIR}")

        foreach(_extra IN LISTS _7Z_COMPANIONS)
            if(EXISTS "${_7Z_VENDOR_DIR}/${_extra}")
                add_custom_command(TARGET hexwalk POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${_7Z_VENDOR_DIR}/${_extra}"
                        "${_7Z_DEST_DIR}/${_extra}"
                )
                message(STATUS "Bundling ${_extra} from ${_7Z_VENDOR_DIR}")
            endif()
        endforeach()
        break()
    endif()
endforeach()

# Release: minimise binary size
if(CMAKE_BUILD_TYPE STREQUAL "Release" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(hexwalk PRIVATE
        -Os                  # optimise for size rather than speed
        -ffunction-sections  # one ELF section per function → gc-sections can prune each one
        -fdata-sections      # same for data
    )
    target_link_options(hexwalk PRIVATE
        -Wl,--gc-sections    # discard sections with no references
        -s                   # strip all symbols and debug info from the output
    )
endif()

# ---- Linux install rules (also used by the AppImage build script) ---------
# `cmake --install` and `make install DESTDIR=AppDir` both consume these.
# Layout follows freedesktop.org / GNU coding standards.
if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)

    install(TARGETS hexwalk RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/hexwalk.desktop"
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/images/hexwalk256.png"
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps
        RENAME hexwalk.png)

    # If a bundled 7-Zip binary was staged under vendor/7z, install it next
    # to hexwalk so the runtime locator (find_7z in hexdig_lib) picks it up
    # the same way the POST_BUILD copy does for plain `cmake --build`.
    foreach(_n IN LISTS _7Z_NAMES)
        if(EXISTS "${_7Z_VENDOR_DIR}/${_n}")
            install(PROGRAMS "${_7Z_VENDOR_DIR}/${_n}"
                DESTINATION ${CMAKE_INSTALL_BINDIR})
            break()
        endif()
    endforeach()
endif()