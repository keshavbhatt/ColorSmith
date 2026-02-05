cmake_minimum_required(VERSION 3.16)

project(colorsmith
    VERSION 1.1.0
    DESCRIPTION "A modern Qt-based color picker application"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Options
option(BUILD_TESTING "Build tests" OFF)

# Find Qt packages
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    DBus
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}
)

# UI files directory
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/ui)

# Sources
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/ColorLogic.cpp
    src/ScreenPicker.cpp
    src/AboutDialog.cpp
    src/Settings.cpp
    src/PaletteWidget.cpp
    src/ColorPreviewWidget.cpp
    src/qthsvrectpicker.cpp
    src/Palette.cpp
    src/PaletteManager.cpp
    src/ColorExtractor.cpp
    src/GradientMaker.cpp
    src/BrightnessSliderWidget.cpp
    src/ShortcutsDialog.cpp
)

# Headers
set(HEADERS
        include/MainWindow.h
        include/ColorLogic.h
        include/ScreenPicker.h
        include/AboutDialog.h
        include/Settings.h
        include/PaletteWidget.h
        include/ColorPreviewWidget.h
        include/qthsvrectpicker.h
        include/Palette.h
        include/PaletteManager.h
        include/ColorExtractor.h
        include/GradientMaker.h
        include/BrightnessSliderWidget.h
        include/ShortcutsDialog.h
)

# UI files
set(UI_FILES
    ui/MainWindow.ui
    ui/AboutDialog.ui
)

# Resource files
set(RESOURCE_FILES
    resources/resources.qrc
)

# Application executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
    ${RESOURCE_FILES}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::DBus
)

# Set target properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "colorsmith"
    VERSION ${PROJECT_VERSION}
)

# Installation
include(GNUInstallDirs)

# Install binary
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install desktop file
install(FILES resources/com.ktechpit.colorsmith.desktop
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
)

# Install AppStream metadata
install(FILES resources/com.ktechpit.colorsmith.appdata.xml
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo
)

# Install icons (if available)
if(EXISTS ${CMAKE_SOURCE_DIR}/resources/icons/colorsmith.svg)
    install(FILES resources/icons/colorsmith.svg
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps
        RENAME com.ktechpit.colorsmith.svg
    )
endif()

# Install translation files if any exist
file(GLOB TRANSLATION_FILES resources/translations/*.qm)
if(TRANSLATION_FILES)
    install(FILES ${TRANSLATION_FILES}
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/colorsmith/translations
    )
endif()

# Install documentation
install(FILES
    ${CMAKE_SOURCE_DIR}/README.md
    ${CMAKE_SOURCE_DIR}/LICENSE
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
    OPTIONAL
)


# Uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
    )

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/cmake_uninstall.cmake
    )
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "ColorSmith Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "")
