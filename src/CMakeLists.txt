add_library(plc-netlib STATIC
    ethernet/mac.cpp
    ethernet/frame.cpp
    ethernet/adapter.cpp
    network/capture.cpp
    profinet/dcp_builder.cpp
    profinet/dcp_parser.cpp
    profinet/scanner.cpp
)

target_include_directories(plc-netlib PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(plc-netlib PUBLIC ${NPCAP_SDK_DIR}/Include)
target_link_directories(plc-netlib PUBLIC ${NPCAP_SDK_DIR}/Lib/x64)
target_link_libraries(plc-netlib PUBLIC wpcap ws2_32 iphlpapi)

add_executable(plcscan main.cpp)
target_link_libraries(plcscan PRIVATE plc-netlib)
