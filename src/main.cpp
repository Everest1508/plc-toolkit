#include "ethernet/adapter.h"
#include "network/capture.h"
#include "profinet/scanner.h"

#include <cstring>
#include <iostream>
#include <limits>
#include <stdexcept>

static void printUsage() {
    std::cout << "plc-toolkit\n\n";
    std::cout << "Usage:\n";
    std::cout << "  plcscan          List network adapters\n";
    std::cout << "  plcscan scan     Discover PROFINET devices\n";
}

static int listAdapters() {
    plc::Capture capture;
    auto adapters = capture.enumerate();

    if (adapters.empty()) {
        std::cout << "No adapters found.\n";
        return 1;
    }

    std::cout << "Available adapters:\n\n";
    for (std::size_t i = 0; i < adapters.size(); ++i) {
        const auto& a = adapters[i];
        std::cout << "  [" << i << "] " << a.description;
        if (!a.ipv4.empty()) {
            std::cout << " (" << a.ipv4 << ")";
        }
        if (a.isLoopback) {
            std::cout << " [loopback]";
        }
        std::cout << "\n";
    }

    return 0;
}

static std::size_t selectAdapter() {
    std::cout << "\nSelect: ";
    std::size_t idx = 0;
    if (!(std::cin >> idx)) return static_cast<std::size_t>(-1);
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return idx;
}

static const char* vendorName(uint16_t vendorId) {
    if (vendorId == 0x002a) return "Siemens AG";
    if (vendorId == 0x0011) return "Schneider Electric";
    if (vendorId == 0x004d) return "Mitsubishi Electric";
    if (vendorId == 0x0056) return "Rockwell Automation";
    if (vendorId == 0x0086) return "ABB";
    if (vendorId == 0x00a8) return "Phoenix Contact";
    if (vendorId == 0x0e22) return "Hilscher";
    return nullptr;
}

static const char* deviceRoleName(uint8_t role) {
    switch (role) {
        case 0x00: return "IO-Device (unset)";
        case 0x01: return "IO-Device";
        case 0x02: return "IO-Controller";
        case 0x03: return "IO-Supervisor";
        default:   return "Unknown";
    }
}

static int scanProfinet() {
    plc::Capture capture;
    auto adapters = capture.enumerate();

    if (adapters.empty()) {
        std::cout << "No adapters found.\n";
        return 1;
    }

    std::cout << "Available adapters:\n\n";
    for (std::size_t i = 0; i < adapters.size(); ++i) {
        const auto& a = adapters[i];
        std::cout << "  [" << i << "] " << a.description;
        if (!a.ipv4.empty()) {
            std::cout << " (" << a.ipv4 << ")";
        }
        if (a.isLoopback) {
            std::cout << " [loopback]";
        }
        std::cout << "\n";
    }

    auto idx = selectAdapter();
    if (idx >= adapters.size()) {
        std::cout << "Invalid selection.\n";
        return 1;
    }

    plc::EthernetAdapter adapter;
    if (!adapter.open(adapters[idx])) {
        std::cerr << "Failed to open adapter.\n";
        return 1;
    }

    std::cout << "\nAdapter opened.\n";

    plc::ProfinetScanner scanner(adapter);
    std::cout << "Scanning for PROFINET devices...\n\n";

    auto devices = scanner.scan(3000);

    if (devices.empty()) {
        std::cout << "No PROFINET devices found.\n";
        return 0;
    }

    std::cout << "Found " << devices.size() << " device(s):\n\n";

    for (std::size_t i = 0; i < devices.size(); ++i) {
        const auto& d = devices[i];
        std::cout << "  [" << i << "] " << d.stationName << "\n";
        std::cout << "      MAC:        " << d.mac.toString() << "\n";
        if (!d.ipv4.empty()) {
            std::cout << "      IP:         " << d.ipv4 << "\n";
        }
        if (!d.netmask.empty()) {
            std::cout << "      Netmask:    " << d.netmask << "\n";
        }
        if (!d.gateway.empty()) {
            std::cout << "      Gateway:    " << d.gateway << "\n";
        }

        std::cout << "      Vendor:     0x"
                  << std::hex << d.vendorId << std::dec;
        const char* vname = vendorName(d.vendorId);
        if (vname) std::cout << " (" << vname << ")";
        std::cout << "\n";

        std::cout << "      Device:     0x"
                  << std::hex << d.deviceId << std::dec << "\n";

        std::cout << "      Role:       " << deviceRoleName(d.deviceRole) << "\n";

        std::cout << "      Instance:   "
                  << ((d.deviceInstance >> 8) & 0xFF) << "."
                  << (d.deviceInstance & 0xFF) << "\n";

        std::cout << "\n";
    }

    adapter.close();
    return 0;
}

int main(int argc, char* argv[]) {
    try {
        if (argc >= 2 && std::strcmp(argv[1], "scan") == 0) {
            return scanProfinet();
        }

        if (argc >= 2) {
            printUsage();
            return 1;
        }

        return listAdapters();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
