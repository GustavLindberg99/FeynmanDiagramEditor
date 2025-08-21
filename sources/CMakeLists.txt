cmake_minimum_required(VERSION 3.22)

project(FeynmanDiagramEditor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set Qt packages
find_package(QT NAMES Qt6)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network)

# Set source files
set(PROJECT_SOURCES
    diagramviewer.cpp
    diagramviewer.hpp
    latexParser.cpp
    latexParser.hpp
    main.cpp
    mainwindow.hpp
    particle.cpp
    particle.hpp
    version.h
)
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    enable_language("RC")
    set (WIN32_RESOURCES resource.rc)
endif()
qt_add_executable(FeynmanDiagramEditor
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    resource.qrc
    ${WIN32_RESOURCES}
)

# Link Qt packages
target_link_libraries(FeynmanDiagramEditor PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(FeynmanDiagramEditor PRIVATE Qt${QT_VERSION_MAJOR}::Svg)
target_link_libraries(FeynmanDiagramEditor PRIVATE Qt${QT_VERSION_MAJOR}::Network)

# Compile executable
set_target_properties(FeynmanDiagramEditor PROPERTIES
    ${BUNDLE_ID_OPTION}
    WIN32_EXECUTABLE TRUE
)
include(GNUInstallDirs)
install(TARGETS FeynmanDiagramEditor
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Automatically run windeployqt when compiling
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_custom_command(TARGET FeynmanDiagramEditor
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/windeployqt"
        COMMAND Qt6::windeployqt --dir "${CMAKE_CURRENT_BINARY_DIR}/windeployqt" "$<TARGET_FILE_DIR:FeynmanDiagramEditor>/$<TARGET_FILE_NAME:FeynmanDiagramEditor>"
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/windeployqt/translations"
        COMMAND ${CMAKE_COMMAND} -E remove "${CMAKE_CURRENT_BINARY_DIR}/windeployqt/vc_redist.x64.exe"
    )
endif()

qt_finalize_executable(FeynmanDiagramEditor)
