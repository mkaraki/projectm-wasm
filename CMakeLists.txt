cmake_minimum_required(VERSION 3.21)
project(ProjectM_Wasm)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/projectm/src
    ${CMAKE_CURRENT_SOURCE_DIR}/projectm/include
)

#find_package(projectM4 REQUIRED)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(ENABLE_SDL_UI OFF CACHE BOOL "" FORCE)
add_subdirectory(projectm)

set(EMSCRIPTEN_FLAGS
	"-sMAX_WEBGL_VERSION=2"
	"-sUSE_SDL=2"
	"-sDISABLE_EXCEPTION_CATCHING=0"
	"-sEXPORTED_FUNCTIONS=_main,_get_audio_buffer,_process_audio_buffer,_on_resize"
	"-sEXPORTED_RUNTIME_METHODS=HEAPF32"
)

add_executable(ProjectMWasm
    main.cpp
)
#set_property(TARGET ProjectMWasm PROPERTY SUFFIX ".html")
target_compile_options(ProjectMWasm PRIVATE ${EMSCRIPTEN_FLAGS})
target_link_options(ProjectMWasm PRIVATE ${EMSCRIPTEN_FLAGS})

target_link_libraries(ProjectMWasm
    PRIVATE
    libprojectM::projectM
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/index.html
DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
