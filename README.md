# plc-toolkit

A C++ library and toolset for PLC communication over Ethernet. Built from scratch — no vendor libraries, no protocol wrappers. Every byte understood.

## Current Status

**Milestone 1 — Network Foundation** (complete)

- Enumerate system network adapters via Npcap
- Select and open an adapter
- `MacAddress` and `EthernetFrame` types
- Catch2 unit tests

## Requirements

- Windows 10/11
- [Npcap](https://npcap.com/#download) installed (runtime driver)
- MSYS2 UCRT64 toolchain (GCC 16+, CMake 3.20+, Ninja)

## Build

```bash
# From MSYS2 UCRT64 terminal
cd /d/plc-rnd/plc-toolkit_v1.0.1

cmake -B build -G Ninja -DPLC_BUILD_TESTS=ON
cmake --build build
```

Npcap SDK is included in `third_party/npca-sdk/` — no separate download needed.

## Usage

```bash
./build/src/plcscan.exe
```

```
Available adapters:

  [0] WAN Miniport (Network Monitor)
  [1] Hyper-V Virtual Ethernet Adapter (172.24.176.1)
  [5] Killer E2600 Gigabit Ethernet Controller (192.168.0.1)
  [6] Killer Wi-Fi 6 AX1650i (192.168.1.17)
  ...

Select: 5

Adapter opened.
  Name:        \Device\NPF_{...}
  Description: Killer E2600 Gigabit Ethernet Controller
  IPv4:        192.168.0.1
```

## Tests

```bash
./build/tests/plc-tests.exe
# or
cd build && ctest
```

## Project Structure

```
include/
  common/       shared types (ByteVec, Error)
  ethernet/     MAC addresses, Ethernet frames, adapter abstraction
  network/      raw pcap wrapper (Capture)
  profinet/     PROFINET DCP (planned)
  s7/           S7 communication (planned)

src/
  main.cpp      plcscan executable
  ethernet/     library implementations
  network/      pcap wrapper implementation

tests/          Catch2 unit tests
docs/           protocol documentation
third_party/    Npcap SDK
```

## Roadmap

See [docs/roadmap.md](docs/roadmap.md).

## License

MIT
