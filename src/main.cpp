#include "ethernet/adapter.h"
#include "network/capture.h"

#include <iostream>
#include <limits>
#include <stdexcept>

int main() {
    try {
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

        std::cout << "\nSelect: ";
        std::size_t idx = 0;
        if (!(std::cin >> idx) || idx >= adapters.size()) {
            std::cout << "Invalid selection.\n";
            return 1;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        plc::EthernetAdapter adapter;
        if (!adapter.open(adapters[idx])) {
            std::cerr << "Failed to open adapter.\n";
            return 1;
        }

        const auto& info = adapter.info();
        std::cout << "\nAdapter opened.\n";
        std::cout << "  Name:        " << info.name << "\n";
        std::cout << "  Description: " << info.description << "\n";
        if (!info.ipv4.empty()) {
            std::cout << "  IPv4:        " << info.ipv4 << "\n";
        }
        if (!info.ipv6.empty()) {
            std::cout << "  IPv6:        " << info.ipv6 << "\n";
        }

        adapter.close();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
