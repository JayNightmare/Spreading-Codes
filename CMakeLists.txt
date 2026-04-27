# ==============================================================================
# CMake Configuration for LunaNetGateway1
# 
# Updates:
# - Applies conditional generator expressions ($<CONFIG:Release>) to optimization 
#   flags to resolve MSVC /RTC1 debug flag collisions.
# - Removes -ffast-math to prevent floating-point inaccuracies in signal generation.
# - Removes -march=native to ensure cross-platform reproducibility (TC-4).
# ==============================================================================

cmake_minimum_required(VERSION 3.10)
project(LunaNetGateway1 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(UNIX AND NOT APPLE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(LIBRARY_OUTPUT_EXTENSION ".so")
elseif(APPLE)
    set(LIBRARY_OUTPUT_EXTENSION ".dylib")
elseif(WIN32)
    set(LIBRARY_OUTPUT_EXTENSION ".dll")
endif()

set(LIBRARY_NAME lunanet_spreading_codes)
set(GATEWAY2_LIBRARY_NAME lunanet_message_encoders)

set(SOURCE_FILES
    codes/spreading_codes.cpp
)

set(HEADER_FILES
    codes/spreading_codes.h
)

add_library(${LIBRARY_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES})
set_target_properties(${LIBRARY_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/codes
)

add_library(${GATEWAY2_LIBRARY_NAME} SHARED
    codes/gateway2/message_encoders.cpp
    codes/gateway2/message_encoders.h
)
set_target_properties(${GATEWAY2_LIBRARY_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_include_directories(${GATEWAY2_LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/codes/gateway2
)

add_executable(test_engine
    codes/test_engine.cpp
)
target_link_libraries(test_engine PRIVATE ${LIBRARY_NAME})
target_include_directories(test_engine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/codes
)
set_target_properties(test_engine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${LIBRARY_NAME} PRIVATE
        $<$<CONFIG:Release>:-O3 -funroll-loops>
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${LIBRARY_NAME} PRIVATE
        $<$<CONFIG:Release>:/O2 /arch:AVX2>
    )
endif()

install(TARGETS ${LIBRARY_NAME} ${GATEWAY2_LIBRARY_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${HEADER_FILES}
    DESTINATION include/lunanet
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/codes/spreading_codes.h"
    "${CMAKE_CURRENT_BINARY_DIR}/lunanet_spreading_codes_config.h"
    COPYONLY
)