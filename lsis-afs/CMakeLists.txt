cmake_minimum_required(VERSION 3.10)
project(LunaNetGateway1 VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform-specific settings
if(UNIX AND NOT APPLE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(LIBRARY_OUTPUT_EXTENSION ".so")
elseif(APPLE)
    set(LIBRARY_OUTPUT_EXTENSION ".dylib")
elseif(WIN32)
    set(LIBRARY_OUTPUT_EXTENSION ".dll")
endif()

# Library name
set(LIBRARY_NAME lunanet_spreading_codes)

# Source files
set(SOURCE_FILES
    codes/spreading_codes.cpp
)

# Header files
set(HEADER_FILES
    codes/spreading_codes.h
)

# Create shared library
add_library(${LIBRARY_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES})

# Set output directory for library
set_target_properties(${LIBRARY_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Version information
set_target_properties(${LIBRARY_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Include directories
target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/codes
)

# Compiler options for performance
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${LIBRARY_NAME} PRIVATE
        -O3
        -march=native
        -ffast-math
        -funroll-loops
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${LIBRARY_NAME} PRIVATE
        /O2
        /arch:AVX2
    )
endif()

# Install rules
install(TARGETS ${LIBRARY_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${HEADER_FILES}
    DESTINATION include/lunanet
)

# Create config file for package
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/codes/spreading_codes.h"
    "${CMAKE_CURRENT_BINARY_DIR}/lunanet_spreading_codes_config.h"
    COPYONLY
)