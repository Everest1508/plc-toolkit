cmake_minimum_required(VERSION 3.20)
project(plc-toolkit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(NPCAP_SDK_DIR "${CMAKE_SOURCE_DIR}/third_party/npca-sdk" CACHE PATH "Path to Npcap SDK")

if(NOT EXISTS "${NPCAP_SDK_DIR}/Include/pcap.h")
    message(FATAL_ERROR "Npcap SDK not found at ${NPCAP_SDK_DIR}. Expected Include/pcap.h.")
endif()

option(PLC_BUILD_TESTS "Build tests" ON)

add_subdirectory(src)

if(PLC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
