cmake_minimum_required(VERSION 3.10)
project(AudioEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set Homebrew paths for macOS (adjust for Intel or Apple Silicon)
set(CMAKE_PREFIX_PATH "/usr/local;/opt/homebrew" ${CMAKE_PREFIX_PATH})

# Find PortAudio using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)

# Find libsndfile using pkg-config
pkg_check_modules(SNDFILE REQUIRED sndfile)

# Find mpg123 using pkg-config
pkg_check_modules(MPG123 REQUIRED libmpg123)

# Include directories
include_directories(${PORTAUDIO_INCLUDE_DIRS} ${SNDFILE_INCLUDE_DIRS} ${MPG123_INCLUDE_DIRS})

# Include project directories
include_directories(${CMAKE_SOURCE_DIR}/Core ${CMAKE_SOURCE_DIR}/Core/Adapters ${CMAKE_SOURCE_DIR}/Core/Effects)

# Add executable
add_executable(AudioEditor
    main.cpp
    Core/Logger.cpp
    Core/AudioEngine.cpp
    Core/AudioTrack.cpp
    Core/AudioClip.cpp
    Core/AudioProject.cpp
    Core/AudioProjectBuilder.cpp
    Core/EffectFactory.cpp
    Core/Adapters/Wav.cpp
    Core/Adapters/Mp3.cpp
    Core/Effects/Echo.cpp
    Core/Effects/Reverb.cpp
    Core/Effects/Fade.cpp
)

# Target-specific include directories
target_include_directories(AudioEditor PRIVATE ${PORTAUDIO_INCLUDE_DIRS} ${SNDFILE_INCLUDE_DIRS} ${MPG123_INCLUDE_DIRS})

# Link libraries
target_link_libraries(AudioEditor PRIVATE ${PORTAUDIO_LIBRARIES} ${SNDFILE_LIBRARIES} ${MPG123_LIBRARIES})
target_compile_options(AudioEditor PRIVATE ${PORTAUDIO_CFLAGS_OTHER} ${SNDFILE_CFLAGS_OTHER} ${MPG123_CFLAGS_OTHER})
target_link_directories(AudioEditor PRIVATE ${PORTAUDIO_LIBRARY_DIRS} ${SNDFILE_LIBRARY_DIRS} ${MPG123_LIBRARY_DIRS})