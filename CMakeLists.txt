cmake_minimum_required(VERSION 3.16)

project(PcMonitor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp

    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    snapshot.h
    systeminfo.cpp
    systeminfo.h
    perfmonitor.cpp
    perfmonitor.h

    circulargauge.cpp
    circulargauge.h
    infocard.cpp
    infocard.h
)

if(WIN32)
    list(APPEND PROJECT_SOURCES pcmonitor.rc)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(PcMonitor
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(PcMonitor ${PROJECT_SOURCES})
endif()

target_include_directories(PcMonitor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(PcMonitor PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(WIN32)
    target_link_libraries(PcMonitor PRIVATE
        ole32
        oleaut32
    )
endif()

set_target_properties(PcMonitor PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(PcMonitor)
endif()
