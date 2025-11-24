cmake_minimum_required(VERSION 3.10)
project(AudioEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set paths for Homebrew Qt
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@6;/usr/local;/opt/homebrew" ${CMAKE_PREFIX_PATH})

# Find Qt packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Multimedia Network)
set(CMAKE_AUTOMOC ON)  # Enable Qt MOC
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

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
include_directories(
    ${MPG123_INCLUDE_DIRS} 
    ${LAME_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/Core 
    ${CMAKE_SOURCE_DIR}/Core/Adapters 
    ${CMAKE_SOURCE_DIR}/Core/Effects
    ${CMAKE_SOURCE_DIR}/Core/Commands