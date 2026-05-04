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

set(GATEWAY2_SOURCES
    codes/gateway2/bch_codec.cpp
    codes/gateway2/crc24.cpp
    codes/gateway2/interleaver.cpp
    codes/gateway2/ldpc_encoder.cpp
)

set(GATEWAY2_HEADERS
    codes/gateway2/bch_codec.h
    codes/gateway2/crc24.h
    codes/gateway2/interleaver.h
    codes/gateway2/ldpc_encoder.h
)

add_library(lunanet_gateway2 STATIC ${GATEWAY2_SOURCES} ${GATEWAY2_HEADERS})
target_include_directories(lunanet_gateway2 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/codes
    ${CMAKE_CURRENT_SOURCE_DIR}/codes/gateway2
)

set(GATEWAY1_SOURCES
    codes/spreading_codes.cpp
    codes/gateway1/spreading_config.cpp
    codes/gateway1/gold_code_generator.cpp
    codes/gateway1/weil_code_generator.cpp
    codes/gateway1/tiered_code_generator.cpp
    codes/c_api.cpp
)

set(GATEWAY1_HEADERS
    codes/spreading_codes.h
    codes/gateway1/spreading_config.h
    codes/gateway1/gold_code_generator.h
    codes/gateway1/weil_code_generator.h
    codes/gateway1/tiered_code_generator.h
    codes/c_api.h
)

add_library(${LIBRARY_NAME} SHARED ${GATEWAY1_SOURCES} ${GATEWAY1_HEADERS})
target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/codes
    ${CMAKE_CURRENT_SOURCE_DIR}/codes/gateway1
    ${CMAKE_CURRENT_SOURCE_DIR}/codes/gateway2
)
target_compile_definitions(${LIBRARY_NAME} PRIVATE LUNANET_BUILDING_DLL)
target_link_libraries(${LIBRARY_NAME} PRIVATE lunanet_gateway2)
set_target_properties(${LIBRARY_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

set(TESTING_SOURCES
    codes/testing/test_reporter.cpp
    codes/testing/test_annex3_loader.cpp
    codes/testing/test_validators.cpp
)

add_library(lunanet_testing STATIC ${TESTING_SOURCES})
target_include_directories(lunanet_testing PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/codes
    ${CMAKE_CURRENT_SOURCE_DIR}/codes/testing
)
target_link_libraries(lunanet_testing PUBLIC ${LIBRARY_NAME})

add_executable(test_engine
    codes/test_engine.cpp
)
target_link_libraries(test_engine PRIVATE lunanet_testing lunanet_gateway2)
target_include_directories(test_engine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/codes
)
set_target_properties(test_engine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

enable_testing()
add_test(
    NAME gateway1_foundation
    COMMAND test_engine
        ${CMAKE_CURRENT_SOURCE_DIR}/config/spreading_codes_config.ini
)

install(TARGETS ${LIBRARY_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${GATEWAY1_HEADERS}
    DESTINATION include/lunanet
)
