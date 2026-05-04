cmake_minimum_required(VERSION 3.16)
project(LunaNetGateway1 VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(UNIX AND NOT APPLE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(LIBRARY_NAME lunanet_spreading_codes)

set(GATEWAY1_SOURCES
    codes/spreading_codes.cpp
    codes/gateway1/spreading_config.cpp
    codes/gateway1/gold_code_generator.cpp
    codes/gateway1/weil_code_generator.cpp
    codes/gateway1/tiered_code_generator.cpp
)

set(GATEWAY1_HEADERS
    codes/spreading_codes.h
    codes/gateway1/spreading_config.h
    codes/gateway1/gold_code_generator.h
    codes/gateway1/weil_code_generator.h
    codes/gateway1/tiered_code_generator.h
)

add_library(${LIBRARY_NAME} SHARED ${GATEWAY1_SOURCES} ${GATEWAY1_HEADERS})
target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/codes
    ${CMAKE_CURRENT_SOURCE_DIR}/codes/gateway1
)
set_target_properties(${LIBRARY_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
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

enable_testing()
add_test(
    NAME gateway1_foundation
    COMMAND test_engine ${CMAKE_CURRENT_SOURCE_DIR}/config/spreading_codes_config.ini
)

install(TARGETS ${LIBRARY_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${GATEWAY1_HEADERS}
    DESTINATION include/lunanet
)
