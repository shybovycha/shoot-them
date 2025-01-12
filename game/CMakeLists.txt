cmake_minimum_required(VERSION 3.29 FATAL_ERROR)

project(game VERSION 2.0.2 LANGUAGES CXX)

set(SOURCES
    "src/main.cpp"
    "src/core/application.cpp"
    "src/core/scenemanager.cpp"
    "src/core/shader.cpp"
    "src/core/gltfmodel.cpp"
    "src/core/windowmanager.cpp"
    "src/scenes/scene1.cpp"
    "src/scenes/scene2.cpp"
    "src/shaders/sceneshader.cpp"
    "src/shaders/forwardrenderpassshader.cpp"
    "src/shaders/shadingrenderpassshader.cpp")

set(EXECUTABLE_NAME game)

if(APPLE)
    file(GLOB RESOURCE_FILES resources)
    set_source_files_properties(${RESOURCE_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "MacOS")

    add_executable(${EXECUTABLE_NAME} MACOSX_BUNDLE ${SOURCES} ${RESOURCE_FILES})
else()
    add_executable(${EXECUTABLE_NAME} ${SOURCES})

    # file(COPY resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    # file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

target_compile_features(${EXECUTABLE_NAME} PRIVATE cxx_std_20)

target_precompile_headers(${EXECUTABLE_NAME} PRIVATE "src/stdafx.hpp")

# fixes "static_assert failed: 'Unicode support requires compiling with /utf-8'" on Windows
target_compile_options(${EXECUTABLE_NAME} PUBLIC "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
target_compile_options(${EXECUTABLE_NAME} PUBLIC "$<$<C_COMPILER_ID:MSVC>:/utf-8>")

# find_package(RapidJSON CONFIG REQUIRED)
# target_link_libraries(main PRIVATE rapidjson)

# find_package(imgui CONFIG REQUIRED)
# target_link_libraries(main PRIVATE imgui::imgui)

# find_package(imguizmo CONFIG REQUIRED)
# target_link_libraries(main PRIVATE imguizmo::imguizmo)

# find_path(MINIAUDIO_INCLUDE_DIRS "miniaudio.h")
# target_include_directories(${EXECUTABLE_NAME} PRIVATE ${MINIAUDIO_INCLUDE_DIRS})

find_package(Stb REQUIRED)
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${Stb_INCLUDE_DIR})

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE glfw)

find_package(GLEW REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE GLEW::GLEW)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE glm::glm)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE fmt::fmt)

# set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
# set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)

find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${TINYGLTF_INCLUDE_DIRS})

# Copy resources
add_custom_command(
    TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/resources
)

add_custom_command(
    TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/data $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/data
)
