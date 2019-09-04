cmake_minimum_required (VERSION 2.8.11)
project (emtest)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../src/include")

execute_process(COMMAND which emcc OUTPUT_VARIABLE EMCC)
execute_process(COMMAND which em++ OUTPUT_VARIABLE EMPP)
string(STRIP ${EMCC} EMCC)
string(STRIP ${EMPP} EMPP)
set(CMAKE_C_COMPILER ${EMCC})
set(CMAKE_CXX_COMPILER ${EMPP})
set(CMAKE_TOOLCHAIN_FILE "$ENV{EMSCRIPTEN_ROOT}/cmake/Modules/Platform/Emscripten.cmake")
set(CMAKE_AR emar)
set(CMAKE_RANLIB emranlib)
set(CMAKE_EXECUTABLE_SUFFIX ".js")
list(APPEND CMAKE_PREFIX_PATH /usr/local)

set(EXTENDED_FLAGS " \
    --bind \
    --source-map-base ./build/ \
    --memory-init-file 0 \
    -s NO_EXIT_RUNTIME=1 \
    -s NO_FILESYSTEM=1 \
    -s ENVIRONMENT=\"web\" \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s MODULARIZE=1 \
    -s EXPORT_NAME=\"load_emtest\" \
    -s EXPORT_ES6=1 \
    -s EXPORTED_FUNCTIONS=\"['_main']\" \
    ")

set(OPT_FLAGS " \
    -g0 \
    -s WASM_OBJECT_FILES=0 \
    ")

set(ASYNC_MODE_FLAGS "-s -s BINARYEN_ASYNC_COMPILATION=1 --bind -s WASM=1")

set(CMAKE_C_FLAGS " \
	${CMAKE_C_FLAGS} \
	${EXTENDED_FLAGS} \
	${OPT_FLAGS} \
	")

set (SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/../src/cpp/emtest.cpp
)

add_library(emtest ${SOURCE_FILES})
set_target_properties(emtest PROPERTIES COMPILE_FLAGS "${ASYNC_MODE_FLAGS}")

add_executable(emtestrunner.async ${CMAKE_CURRENT_SOURCE_DIR}/../src/cpp/emtest.cpp)
target_link_libraries(emtestrunner.async emtest "${ASYNC_MODE_FLAGS}")
set_target_properties(emtestrunner.async PROPERTIES COMPILE_FLAGS "${ASYNC_MODE_FLAGS}")
set_target_properties(emtestrunner.async PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../build/)
set_target_properties(emtestrunner.async PROPERTIES OUTPUT_NAME "emtest.async")