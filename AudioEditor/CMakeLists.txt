cmake_minimum_required(VERSION 3.10)
project(AudioEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find MPG123 library
find_library(MPG123_LIBRARY mpg123)
if(NOT MPG123_LIBRARY)
    message(FATAL_ERROR "mpg123 library not found. Please install libmpg123 using 'brew install mpg123'.")
endif()

# Find LAME library for MP3 encoding
find_library(LAME_LIBRARY lame)
if(NOT LAME_LIBRARY)
    message(FATAL_ERROR "lame library not found. Please install lame using 'brew install lame'.")
endif()

# Add executable
add_executable(AudioEditor
    main.cpp
    Core/AudioClip.cpp
    Core/Adapters/Mp3.cpp
    Core/Effects/Reverb.cpp
    Core/Effects/Speed.cpp
    Core/Logger.cpp
    Core/EffectFactory.cpp
    Core/Adapters/AudioFileAdapter.cpp
)

# Include directories
target_include_directories(AudioEditor PRIVATE Core Core/Adapters Core/Effects)
# Add system include directories for mpg123 and lame (Apple Silicon path)
if(EXISTS /opt/homebrew/include)
    target_include_directories(AudioEditor PRIVATE /opt/homebrew/include) # For Apple Silicon
endif()

# Link libraries
target_link_libraries(AudioEditor PRIVATE ${MPG123_LIBRARY} ${LAME_LIBRARY})