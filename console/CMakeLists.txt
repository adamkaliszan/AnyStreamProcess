cmake_minimum_required(VERSION 3.5)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_compile_definitions(INC_AGENDA_PERFORMANCE 1)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.


find_package(QT NAMES Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

if (ENABLE_TENSORFLOW)
    find_package(TensorflowCC REQUIRED)
endif()

find_package(Qt5Charts REQUIRED)
find_package(Qt5DataVisualization REQUIRED)
find_package(Qt5PrintSupport REQUIRED)


#set(TS_FILES gui/languages/kolejki_en.ts gui/languages/kolejki_pl.ts)

set(CONSOLE_SOURCES
        main.cpp
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(AppConsole
        MANUAL_FINALIZATION
        ${CONSOLE_SOURCES}
    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(AppConsole SHARED
            ${CONSOLE_SOURCES}
        )
    else()
        add_executable(AppConsole
            ${CONSOLE_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_include_directories(AppConsole PUBLIC ../core)
target_link_libraries(AppConsole PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts Qt${QT_VERSION_MAJOR}::DataVisualization protobuf)
target_link_libraries(AppConsole PUBLIC core)


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(AppConsole)
endif()

