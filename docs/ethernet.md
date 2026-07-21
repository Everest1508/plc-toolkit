# Ethernet Frame Format

## Frame Structure

An Ethernet II frame (RFC 894) is the foundation of all local network communication.

```
Offset  Size    Field
------  ------  ---------------------------
0       6       Destination MAC
6       6       Source MAC
12      2       EtherType
14      46-1500 Payload (data)
+4      4       FCS/CRC (added by NIC, not visible to software)
```

Minimum frame size: 64 bytes (including header, excluding CRC).
Maximum frame size: 1518 bytes (including header and CRC).

## Byte Layout

### Destination MAC (bytes 0-5)

6 bytes. Target device MAC address.

- Unicast: specific device
- Broadcast: `ff:ff:ff:ff:ff:ff` — reaches all devices on the segment

### Source MAC (bytes 6-11)

6 bytes. Sender device MAC address.

### EtherType (bytes 12-13)

2 bytes, big-endian. Identifies the protocol in the payload.

| EtherType | Protocol |
|-----------|----------|
| 0x0800    | IPv4 |
| 0x0806    | ARP |
| 0x86DD    | IPv6 |
| **0x8892** | **PROFINET DCP** |

### Payload (bytes 14+)

Variable length. The data carried by the frame. For PROFINET, this contains the PROFINET DCP header and payload.

## MAC Address Format

48-bit address, written as `xx:xx:xx:xx:xx:xx` where each `x` is a hex digit.

```
Example: 00:11:22:33:44:55
         ^^^^^^^^^^^^^^^^^^^^
         6 bytes = 12 hex digits
```

First 3 bytes (OUI): identify the manufacturer.
Last 3 bytes: device-specific.

## PROFINET on Ethernet

PROFINET uses EtherType `0x8892`. When a PLC scanner broadcasts a DCP Identify Request:

```
Destination: ff:ff:ff:ff:ff:ff (broadcast)
Source:      [scanner MAC]
EtherType:   0x8892 (PROFINET)
Payload:     DCP Identify Request
```

The PLC responds with its identity:

```
Destination: [scanner MAC]
Source:      [PLC MAC]
EtherType:   0x8892 (PROFINET)
Payload:     DCP Identify Response
```

## References

- IEEE 802.3
- RFC 894
- [PROFINET Specification](https://www.profibus.com/technical/profinet/)
