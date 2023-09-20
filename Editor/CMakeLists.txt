cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(editor VERSION 1.0.2 LANGUAGES CXX)

set(EXECUTABLE_NAME editor)

set(SOURCES
    "src/main.cpp"
    "src/ApplicationDelegate.h"
    "src/ApplicationDelegate.cpp"
    "src/IrrlichtEventReceiver.h"
    "src/IrrlichtEventReceiver.cpp"
    "src/Application.h"
    "src/Application.cpp"
    "src/Level.h"
    "src/Level.cpp"
    "src/GameManagerNodeData.h"
    "src/GameManagerNodeData.cpp"
    "src/Target.h"
    "src/Target.cpp"
    "src/GameData.h"
    "src/GameData.cpp"
    "src/GameManagerTree.h"
    "src/GameManagerTree.cpp"
    "src/GUIElementId.h"
    "src/CameraSceneNodeAnimator.h"
    "src/CameraSceneNodeAnimator.cpp"
    "src/Light.h"
    "src/Light.cpp"
    "src/LevelEntity.h"
    "src/LevelEntity.cpp"
    "src/SaveFileDialog.h"
    "src/SaveFileDialog.cpp")

# set(LIBRARIES
#         ${X11_LIBRARIES}
#         ${OPENGL_LIBRARIES}
#         ${ZLIB_LIBRARIES})

# if(NOT APPLE)
#     set(LIBRARIES ${LIBRARIES} ${X11_Xxf86vm_LIB})
# endif()

# if(APPLE)
#     find_library(IOKIT NAMES IOKit)
#     find_library(OpenGL NAMES OpenGL)
#     find_library(COREVIDEO NAMES CoreVideo)
#     find_library(COCOA NAMES Cocoa)
#     set(LIBRARIES ${LIBRARIES} ${IOKIT} ${COREVIDEO} ${COCOA})
# endif()

if(APPLE)
    add_executable(${EXECUTABLE_NAME} MACOSX_BUNDLE ${SOURCES})
else()
    add_executable(${EXECUTABLE_NAME} ${SOURCES})

    # file(COPY resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    # file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

target_compile_features(${EXECUTABLE_NAME} PRIVATE cxx_std_20)

find_package(SFML COMPONENTS audio CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE sfml-audio)

find_package(tinyxml2 CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE tinyxml2::tinyxml2)

find_package(irrlicht CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE Irrlicht)

# # Copy libraries' DLLs
# add_custom_command(
#     TARGET ${EXECUTABLE_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:tinyxml2> $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>
# )

# TODO: copy DLLs, not LIBs
# add_custom_command(
#     TARGET ${EXECUTABLE_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:irrlicht> $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>
# )

# Copy resources
add_custom_command(
    TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/resources
)
