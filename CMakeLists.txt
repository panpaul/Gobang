cmake_minimum_required(VERSION 3.5)

project(Gobang LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Doxygen
FIND_PACKAGE(Doxygen)
OPTION(BUILD_DOCUMENTATION "Create the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

IF (BUILD_DOCUMENTATION)
    IF (NOT DOXYGEN_FOUND)
        MESSAGE(FATAL_ERROR "Doxygen is needed to build the documentation.")
    ENDIF ()

    SET(doxyfile ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)

    ADD_CUSTOM_TARGET(doc
            COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
ENDIF ()

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

set(TS_FILES Gobang_zh_CN.ts)

if (ANDROID)
    add_library(Gobang SHARED
            main.cpp
            mainwindow.cpp mainwindow.h mainwindow.ui
            gamewindow.cpp gamewindow.h gamewindow.h
            Board.cpp Board.h
            Game.cpp Game.h
            Engine.cpp Engine.h
            ${TS_FILES})
else ()
    add_executable(Gobang
            main.cpp
            mainwindow.cpp mainwindow.h mainwindow.ui
            gamewindow.cpp gamewindow.h gamewindow.ui
            Board.cpp Board.h
            Game.cpp Game.h
            Engine.cpp Engine.h
            ${TS_FILES})
endif ()

target_link_libraries(Gobang PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
