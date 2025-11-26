cmake_minimum_required(VERSION 3.16)
project(AudioEditor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@6" ${CMAKE_PREFIX_PATH})
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Multimedia)

# Find mpg123
find_package(PkgConfig REQUIRED)
pkg_check_modules(MPG123 REQUIRED libmpg123)

# Find LAME
find_library(LAME_LIBRARY NAMES mp3lame lame)
find_path(LAME_INCLUDE_DIR lame/lame.h)

# Source files
set(CORE_SOURCES
    src/Core/AudioClip.cpp
    src/Core/EffectFactory.cpp
    src/Core/Logging/FileLogger.cpp
    src/Core/Logging/ConsoleLogger.cpp
    src/Core/Logging/CompositeLogger.cpp
    src/Core/Adapters/Mp3Adapter.cpp
    src/Core/Effects/Echo.cpp
    src/Core/Effects/Reverb.cpp
    src/Core/Effects/Speed.cpp
    src/Core/Effects/Volume.cpp
    src/Core/Effects/Normalize.cpp
    src/Core/Commands/CommandHistory.cpp
    src/Core/Commands/ApplyEffectCommand.cpp
    src/Core/Services/CaptionParser.cpp
)

set(GUI_SOURCES
    src/GUI/MainWindow.cpp
    src/GUI/AudioEngine.cpp
    src/GUI/TransportBar.cpp
    src/GUI/WaveformWidget.cpp
    src/GUI/EffectsPanel.cpp
    src/GUI/EffectWidget.cpp
    src/GUI/CaptionPanel.cpp
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/Core
    ${CMAKE_SOURCE_DIR}/src/Core/Adapters
    ${CMAKE_SOURCE_DIR}/src/Core/Effects
    ${CMAKE_SOURCE_DIR}/src/Core/Commands
    ${CMAKE_SOURCE_DIR}/src/Core/Logging
    ${CMAKE_SOURCE_DIR}/src/Core/Services
    ${CMAKE_SOURCE_DIR}/src/GUI
    ${MPG123_INCLUDE_DIRS}
    ${LAME_INCLUDE_DIR}
)

# Executable
add_executable(AudioEditor 
    src/main.cpp
    ${CORE_SOURCES}
    ${GUI_SOURCES}
)

# Link libraries
target_link_libraries(AudioEditor
    PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Multimedia
    ${MPG123_LIBRARIES}
    ${LAME_LIBRARY}
)

# Compile options
target_compile_options(AudioEditor PRIVATE ${MPG123_CFLAGS_OTHER})
target_link_directories(AudioEditor PRIVATE ${MPG123_LIBRARY_DIRS})

# macOS bundle
if(APPLE)
    set_target_properties(AudioEditor PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.audioeditor.app"
        MACOSX_BUNDLE_BUNDLE_NAME "Audio Editor"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
    )
endif()
```

---

## Final File Structure
```
AudioEditor/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   │
│   ├── Core/
│   │   ├── AudioClip.h
│   │   ├── AudioClip.cpp
│   │   ├── EffectFactory.h
│   │   ├── EffectFactory.cpp
│   │   │
│   │   ├── Adapters/
│   │   │   ├── AudioFileAdapter.h
│   │   │   ├── Mp3Adapter.h
│   │   │   └── Mp3Adapter.cpp
│   │   │
│   │   ├── Effects/
│   │   │   ├── IEffect.h
│   │   │   ├── Echo.h / Echo.cpp
│   │   │   ├── Reverb.h / Reverb.cpp
│   │   │   ├── Speed.h / Speed.cpp
│   │   │   ├── Volume.h / Volume.cpp
│   │   │   └── Normalize.h / Normalize.cpp
│   │   │
│   │   ├── Commands/
│   │   │   ├── ICommand.h
│   │   │   ├── CommandHistory.h / CommandHistory.cpp
│   │   │   └── ApplyEffectCommand.h / ApplyEffectCommand.cpp
│   │   │
│   │   ├── Logging/
│   │   │   ├── ILogger.h
│   │   │   ├── ConsoleLogger.h / ConsoleLogger.cpp
│   │   │   ├── FileLogger.h / FileLogger.cpp
│   │   │   └── CompositeLogger.h / CompositeLogger.cpp
│   │   │
│   │   └── Services/
│   │       ├── CaptionParser.h
│   │       └── CaptionParser.cpp
│   │
│   └── GUI/
│       ├── MainWindow.h
│       ├── MainWindow.cpp
│       ├── AudioEngine.h
│       ├── AudioEngine.cpp
│       ├── TransportBar.h
│       ├── TransportBar.cpp
│       ├── WaveformWidget.h
│       ├── WaveformWidget.cpp
│       ├── EffectsPanel.h
│       ├── EffectsPanel.cpp
│       ├── EffectWidget.h
│       ├── EffectWidget.cpp
│       ├── CaptionPanel.h
│       └── CaptionPanel.cpp
