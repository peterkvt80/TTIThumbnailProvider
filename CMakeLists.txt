cmake_minimum_required(VERSION 3.15)
project(TTIThumbnailProvider)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    DllMain.cpp
    TTIThumbnailProvider.cpp
    TeletextRenderer.cpp
)

set(HEADERS
    TTIThumbnailProvider.h
    TeletextRenderer.h
)

# Create the DLL
add_library(TTIThumbnailProvider SHARED
    ${SOURCES}
    ${HEADERS}
    TTIThumbnailProvider.def
)

# Set output name
set_target_properties(TTIThumbnailProvider PROPERTIES
    OUTPUT_NAME "TTIThumbnailProvider"
    SUFFIX ".dll"
)

# Link libraries
target_link_libraries(TTIThumbnailProvider
    shlwapi
    gdi32
)

# Windows-specific settings
if(WIN32)
    target_compile_definitions(TTIThumbnailProvider PRIVATE
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
    )
    
    # Set subsystem
    set_target_properties(TTIThumbnailProvider PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS"
    )
endif()

# Installation
install(TARGETS TTIThumbnailProvider
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES
    Install.bat
    Uninstall.bat
    README.md
    sample.tti
    DESTINATION .
)
