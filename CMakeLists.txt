cmake_minimum_required(VERSION 3.10)

project(shoot-them)

set(SOURCE_FILES Game/Sources/main.cpp)
set(EXECUTABLE_NAME shoot-them)

if(APPLE)
    file(GLOB RESOURCE_FILES Game/Resources)
    set_source_files_properties(${RESOURCE_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "MacOS")

    add_executable(shoot-them MACOSX_BUNDLE ${SOURCE_FILES} ${RESOURCE_FILES})
else()
    add_executable(shoot-them ${SOURCE_FILES})

    file(COPY Game/Resources DESTINATION ${PROJECT_BINARY_DIR})
endif()

set_target_properties(shoot-them PROPERTIES CXX_STANDARD 17)
target_compile_features(shoot-them PUBLIC cxx_std_14)

set(IRRLICHT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/irrlicht-1.8.4)
set(IRRKLANG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/irrKlang-1.5.0)

find_package(OpenGL REQUIRED)
find_package(ZLIB REQUIRED)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "64 bit platform detected")
    SET(EX_PLATFORM 64)
    SET(EX_PLATFORM_NAME "x64")
else()
    message(STATUS "32 bit platform detected")
    SET(EX_PLATFORM 32)
    SET(EX_PLATFORM_NAME "x86")
endif()

if(NOT IRRLICHT_LIBRARY_PATH)
    if(UNIX)
        set(IRRLICHT_PATH_SUFFIX Linux)
    endif()

    if(APPLE)
        set(IRRLICHT_PATH_SUFFIX MacOSX)
    endif()

    if(WIN32)
        if(MSVC)
            set(IRRLICHT_PATH_SUFFIX Win32-visualstudio Win64-visualstudio)
        endif()

        if(MINGW)
            set(IRRLICHT_PATH_SUFFIX Win32-gcc)
        endif()
    endif()

    find_library(IRRLICHT_LIBRARY_PATH
        NAMES Irrlicht
        PATHS ${IRRLICHT_PATH}/lib/
        PATH_SUFFIXES ${IRRLICHT_PATH_SUFFIX}
    )

    message(STATUS "Manually found Irrlicht: ${IRRLICHT_LIBRARY_PATH}")
endif()

if(NOT IRRKLANG_LIBRARY_PATH)
    if(UNIX)
        set(IRRKLANG_PATH_SUFFIX linux-gcc)
    endif()

    if(APPLE)
        set(IRRKLANG_PATH_SUFFIX macosx-gcc)
    endif()

    if(WIN32)
        if(MSVC)
            set(IRRKLANG_PATH_SUFFIX winx64 dotnet-4)
        endif()
    endif()

    if(EX_PLATFORM EQUAL 64)
        if(NOT APPLE)
            set(IRRKLANG_PATH_SUFFIX ${IRRKLANG_PATH_SUFFIX}-64)
        endif()
    endif()

    message(STATUS "Looking for IrrKlang in ${IRRKLANG_PATH}/bin with ${IRRKLANG_PATH_SUFFIX}")

    find_library(IRRKLANG_LIBRARY_PATH
        NAMES irrklang
        PATHS ${IRRKLANG_PATH}/bin/
        PATH_SUFFIXES ${IRRKLANG_PATH_SUFFIX}
    )

    message(STATUS "Manually found IrrKlang: ${IRRKLANG_LIBRARY_PATH}")
endif()

add_library(Irrlicht SHARED IMPORTED)

set_target_properties(Irrlicht PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${IRRLICHT_PATH}/include
    INTERFACE_LINK_LIBRARIES ${IRRLICHT_LIBRARY_PATH}
    IMPORTED_LOCATION ${IRRLICHT_LIBRARY_PATH}
)

add_library(irrKlang SHARED IMPORTED)

set_target_properties(irrKlang PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${IRRKLANG_PATH}/include
    INTERFACE_LINK_LIBRARIES ${IRRKLANG_LIBRARY_PATH}
    IMPORTED_LOCATION ${IRRKLANG_LIBRARY_PATH}
)

target_include_directories(shoot-them
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${IRRLICHT_INCLUDE_PATH}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${IRRKLANG_INCLUDE_PATH}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(shoot-them
    PRIVATE
        Irrlicht
        irrKlang
        OpenGL::GL
        ZLIB::ZLIB
)

if(NOT APPLE)
    find_package(X11)
    target_link_libraries(shoot-them PRIVATE ${X11_Xxf86vm_LIB})
endif()

if(APPLE)
    find_library(IOKIT NAMES IOKit REQUIRED)
    find_library(OpenGL NAMES OpenGL REQUIRED)
    find_library(COREVIDEO NAMES CoreVideo REQUIRED)
    find_library(COCOA NAMES Cocoa REQUIRED)

    # target_link_libraries(shoot-them
    #     PRIVATE
    #         IOKit
    #         CoreVideo
    #         Cocoa
    # )
endif()
