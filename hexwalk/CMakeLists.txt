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
)

# Windows static linking (MinGW only)
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(hexwalk PRIVATE
        -no-pie
        -static-libgcc
        -static-libstdc++
        -static
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