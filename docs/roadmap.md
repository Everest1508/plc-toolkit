# Roadmap

## Milestone 1 — Network Foundation ✓

**Status:** Complete

- [x] Project scaffolding, CMake build, Npcap SDK
- [x] `MacAddress` class (parse, format, compare)
- [x] `EthernetFrame` struct (serialize/deserialize)
- [x] `Capture` class (enumerate adapters via Npcap)
- [x] `EthernetAdapter` class (open/close adapter)
- [x] `plcscan` executable (list, select, open)
- [x] Catch2 unit tests
- [x] Protocol documentation

## Milestone 2 — PROFINET DCP

**Status:** Planned

- [ ] Implement DCP frame format (headers, TLVs)
- [ ] DCP Identify Request builder
- [ ] DCP Identify Response parser
- [ ] `ProfinetScanner` class (broadcast scan, collect responses)
- [ ] `plcscan scan` command
- [ ] Display discovered PLCs (name, IP, MAC, device ID)

## Milestone 3 — S7 Communication

**Status:** Planned

- [ ] TCP connection management
- [ ] COTP (ISO 8073) protocol
- [ ] S7comm protocol layer
- [ ] `S7Client` class (connect, read/write DB)
- [ ] `plcscan read-db` command

## Milestone 4 — TIA Integration

**Status:** Future

- [ ] Station Name resolution
- [ ] IP configuration via DCP Set
- [ ] Device state monitoring
