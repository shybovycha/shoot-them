cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(editor VERSION 2.0.0 LANGUAGES CXX)

set(EXECUTABLE_NAME editor)

set(SOURCES
    "src/main.cpp"
    "src/widgets/imfiledialog/ImFileDialog.h"
    "src/widgets/imfiledialog/ImFileDialog.cpp"
    "src/widgets/imguicolortextedit/TextEditor.h"
    "src/widgets/imguicolortextedit/TextEditor.cpp"
    "src/widgets/imguicurveeditor/ImGuiCurveEditor.cpp"
    "src/widgets/imguinodeeditor/crude_json.cpp"
    "src/widgets/imguinodeeditor/crude_json.h"
    "src/widgets/imguinodeeditor/imgui_bezier_math.h"
    "src/widgets/imguinodeeditor/imgui_bezier_math.inl"
    "src/widgets/imguinodeeditor/imgui_canvas.cpp"
    "src/widgets/imguinodeeditor/imgui_canvas.h"
    "src/widgets/imguinodeeditor/imgui_extra_math.h"
    "src/widgets/imguinodeeditor/imgui_extra_math.inl"
    "src/widgets/imguinodeeditor/imgui_node_editor.cpp"
    "src/widgets/imguinodeeditor/imgui_node_editor.h"
    "src/widgets/imguinodeeditor/imgui_node_editor_api.cpp"
    "src/widgets/imguinodeeditor/imgui_node_editor_internal.h"
    "src/widgets/imguinodeeditor/imgui_node_editor_internal.inl"
    "src/widgets/imnodes/imnodes.cpp"
    "src/widgets/imnodes/imnodes.h"
    "src/widgets/imnodes/imnodes_internal.h"
)

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

    target_compile_definitions(${EXECUTABLE_NAME} PUBLIC GL_SILENCE_DEPRECATION)
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

find_package(fmt CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE fmt::fmt)

# find_package(SFML COMPONENTS system graphics audio CONFIG REQUIRED)
# target_link_libraries(${EXECUTABLE_NAME} PRIVATE sfml-system sfml-graphics sfml-audio)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE assimp::assimp)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE glfw)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE glm::glm)

find_package(globjects CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE globjects::globjects)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE imgui::imgui)

find_package(imguizmo CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE imguizmo::imguizmo)

find_package(RapidJSON CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE rapidjson)

find_package(OpenGL REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${OPENGL_LIBRARIES})

# Copy resources
add_custom_command(
    TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/resources
)
