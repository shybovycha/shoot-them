cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(game VERSION 1.0.2 LANGUAGES CXX)

set(SOURCES
    "src/main.cpp"
)

set(EXECUTABLE_NAME game)

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
    file(GLOB RESOURCE_FILES resources)
    set_source_files_properties(${RESOURCE_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION "MacOS")

    add_executable(${EXECUTABLE_NAME} MACOSX_BUNDLE ${SOURCES} ${RESOURCE_FILES})
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

find_package(SFML COMPONENTS system graphics audio CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE sfml-system sfml-graphics sfml-audio)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE assimp::assimp)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE glfw)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE glm::glm)

find_package(globjects CONFIG REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE globjects::globjects)

# find_package(imgui CONFIG REQUIRED)
# target_link_libraries(${EXECUTABLE_NAME} PRIVATE imgui::imgui)

find_package(OpenGL REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${OPENGL_LIBRARIES})

find_package(Lua REQUIRED)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${LUA_LIBRARIES})
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${LUA_INCLUDE_DIR})

find_path(LUABRIDGE3_INCLUDE_DIRS "luabridge3/LuaBridge/Array.h")
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${LUABRIDGE3_INCLUDE_DIRS})

# Copy resources
add_custom_command(
    TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/resources
)
