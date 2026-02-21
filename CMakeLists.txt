cmake_minimum_required(VERSION 3.11)
project(ChessGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# пути
set(OXYGINE_ROOT "${CMAKE_SOURCE_DIR}/oxygine-framework")
set(LIB_OXYGINE "${OXYGINE_ROOT}/build/liboxygine-framework.a")

# Настройка портативности (RPATH)
set(CMAKE_INSTALL_RPATH "$ORIGIN/lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# поиск зависимостей в системе
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)

find_library(FREEIMAGE_LIB NAMES freeimage FreeImage REQUIRED)
find_library(JPEG_LIB NAMES jpeg REQUIRED)
find_library(PNG_LIB NAMES png REQUIRED)

# подключение заголовков и исходников
include_directories(
    ${OXYGINE_ROOT}/oxygine/src
    src
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(ChessGame ${SOURCES})

# линковка
target_link_libraries(ChessGame 
    ${LIB_OXYGINE}
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${FREEIMAGE_LIB}
    ${JPEG_LIB}
    ${PNG_LIB}
    z GL pthread dl
)

# создание bin
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# копирование ресурсов в bin/data
add_custom_command(TARGET ChessGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data
    ${CMAKE_SOURCE_DIR}/bin/data
    COMMENT "Copying resources..."
)