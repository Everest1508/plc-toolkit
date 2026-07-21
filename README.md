# plc-toolkit

A C++ library and toolset for PLC communication over Ethernet. Built from scratch — no vendor libraries, no protocol wrappers. Every byte understood.

## Current Status

**Milestone 2 — PROFINET DCP** (complete)

- Discover PROFINET devices on the network (like TIA Portal does)
- DCP Identify Request builder and response parser
- Full device info display: station name, IP, MAC, vendor, device ID, role
- 24 unit tests covering protocol parsing

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
# List adapters
./build/src/plcscan.exe

# Discover PROFINET devices
./build/src/plcscan.exe scan
```

### List adapters

```
Available adapters:

  [0] WAN Miniport (Network Monitor)
  [5] Killer E2600 Gigabit Ethernet Controller (192.168.0.1)
  [6] Killer Wi-Fi 6 AX1650i (192.168.1.17)
  ...
```

### Scan for PROFINET devices

```
> plcscan scan

Available adapters:

  [5] Killer E2600 Gigabit Ethernet Controller (192.168.0.1)

Select: 5

Adapter opened.
Scanning for PROFINET devices...

Found 1 device(s):

  [0] plc_1
      MAC:        00:1e:c0:xx:xx:xx
      IP:         192.168.0.2
      Netmask:    255.255.255.0
      Gateway:    192.168.0.1
      Vendor:     0x002a (Siemens AG)
      Device:     0x0301
      Role:       IO-Controller
      Instance:   0.0
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
  profinet/     PROFINET DCP (DCP builder, parser, scanner)
  s7/           S7 communication (planned)

src/
  main.cpp      plcscan executable
  ethernet/     library implementations
  network/      pcap wrapper implementation
  profinet/     DCP builder, parser, scanner

tests/          Catch2 unit tests
docs/           protocol documentation
third_party/    Npcap SDK
```

## Architecture

```
plcscan (CLI)
  └── ProfinetScanner
        └── DcpBuilder (builds Identify Request)
        └── DcpParser (parses Identify Response)
        └── EthernetAdapter (raw frame send/receive)
              └── Capture (Npcap wrapper)
                    └── Npcap → Windows kernel → Ethernet NIC
```

## Roadmap

See [docs/roadmap.md](docs/roadmap.md).

- Milestone 1: Network Foundation ✓
- Milestone 2: PROFINET DCP ✓
- Milestone 3: S7 Communication (planned)
- Milestone 4: TIA Integration (future)

## License

MIT
