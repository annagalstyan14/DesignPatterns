cmake_minimum_required(VERSION 3.10)
project(AudioEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set Homebrew paths for macOS
set(CMAKE_PREFIX_PATH "/usr/local;/opt/homebrew" ${CMAKE_PREFIX_PATH})

# Find mpg123 using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(MPG123 REQUIRED libmpg123)

# Find lame
find_library(LAME_LIBRARY NAMES mp3lame lame)
if(NOT LAME_LIBRARY)
    message(FATAL_ERROR "LAME library not found")
endif()
find_path(LAME_INCLUDE_DIR lame/lame.h)
if(NOT LAME_INCLUDE_DIR)
    message(FATAL_ERROR "LAME headers not found")
endif()

# Include directories
include_directories(${MPG123_INCLUDE_DIRS} ${LAME_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/Core ${CMAKE_SOURCE_DIR}/Core/Adapters ${CMAKE_SOURCE_DIR}/Core/Effects)

# Add executable
add_executable(AudioEditor
    main.cpp
    Core/Logger.cpp
    Core/AudioClip.cpp
    Core/EffectFactory.cpp
    Core/Adapters/Mp3.cpp
    Core/Effects/Echo.cpp
    Core/Effects/Reverb.cpp
)

# Target-specific include directories
target_include_directories(AudioEditor PRIVATE ${MPG123_INCLUDE_DIRS} ${LAME_INCLUDE_DIR})

# Link libraries
target_link_libraries(AudioEditor PRIVATE ${MPG123_LIBRARIES} ${LAME_LIBRARY})
target_compile_options(AudioEditor PRIVATE ${MPG123_CFLAGS_OTHER})
target_link_directories(AudioEditor PRIVATE ${MPG123_LIBRARY_DIRS})