# PROFINET DCP

PROFINET Discovery and Configuration Protocol (DCP) is a Layer 2 protocol used to discover and configure PROFINET devices on a network segment.

## Overview

TIA Portal uses DCP to discover PLCs on the network. It broadcasts a DCP Identify Request, and every PROFINET device responds with its identity — station name, IP, device ID, and more.

## Frame Structure

DCP runs directly on Ethernet (EtherType `0x8892`), inside the PROFINET IO frame format.

### PROFINET IO Header

```
Offset  Size  Field
0       2     Frame ID
```

Frame IDs for DCP:
- `0xFEFE` — DCP Identify Request
- `0xFEFF` — DCP Identify Response
- `0xFEFD` — DCP Get/Set

### DCP Header (after Frame ID)

```
Offset  Size  Field
0       1     Service ID
1       1     Service Type
2       4     XID (Transaction ID)
6       2     Response Delay (Identify Request only) / Reserved
8       2     DCP Data Length
```

Total: 10 bytes.

### Service IDs

| Value | Name |
|-------|------|
| 0x03 | Get |
| 0x04 | Set |
| 0x05 | Identify |
| 0x06 | Hello |

### Service Types

| Value | Name |
|-------|------|
| 0x00 | Request |
| 0x01 | Response Success |
| 0x05 | Response — Request not supported |

### DCP Blocks (TLV)

After the DCP header, the data area contains zero or more blocks:

```
Offset  Size  Field
0       1     Option
1       1     Sub-option
2       2     Block Length (N)
4       2     Block Info
6       N     Block Data
```

Block data is padded to an even byte boundary.

### Options

| Value | Name | Sub-options |
|-------|------|-------------|
| 0x01 | IP | MAC (0x01), IP Address (0x02), Full IP (0x03) |
| 0x02 | Device Properties | Manufacturer (0x01), Name of Station (0x02), Device ID (0x03), Device Role (0x04), Device Options (0x05), Alias Name (0x06), Device Instance (0x07), OEM ID (0x08) |
| 0xFF | ALL Selector | ALL (0xFF) |

### Key Blocks in Identify Response

**IP Address (Option 0x01, Sub 0x02):**
```
Offset  Size  Field
0       4     IP Address
4       4     Netmask
8       4     Gateway
```

**Name of Station (Option 0x02, Sub 0x02):**
```
Offset  Size  Field
0       N     Station name (ASCII string)
```

**Device ID (Option 0x02, Sub 0x03):**
```
Offset  Size  Field
0       2     Vendor ID
2       2     Device ID
```

**Device Role (Option 0x02, Sub 0x04):**
```
Offset  Size  Field
0       1     Role (0x01=IO-Device, 0x02=IO-Controller, 0x03=IO-Supervisor)
1       1     Reserved
```

## Multicast Address

DCP Identify Requests are sent to the PROFINET DCP multicast MAC:

```
01:0e:cf:00:00:00
```

## Scan Workflow

1. Scanner sends DCP Identify Request to multicast MAC
2. Every PROFINET device on the segment responds (unicast)
3. Response contains blocks with device identity
4. Scanner collects responses for a timeout period (typically 3 seconds)

## Minimum Frame Size

Ethernet minimum frame size is 60 bytes (excluding CRC). DCP frames must be padded to meet this requirement.

## References

- PROFINET Specification, Part 6 (Application Layer, DCP)
- IEC 61158/61784
- [PROFINET/DCP - Wireshark Wiki](https://wiki.wireshark.org/PROFINET/DCP)
- [PROFINET DCP Work Notes](https://ericwu1997.github.io/docs/protocol-analysis/ot-layer-2/profinet-dcp/)
