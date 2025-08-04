# SHADE-Protocol-v1.3
Simple Header Arbitrary Data Exchange protocol

Specification document and reference C implementation of shade protocol.

Simple Header Arbitrary Data Exchange protocol is a data
exchange protocol created to exchange data and multiplex data packets between
multiple sessions and recipients over character devices such as UART, while
keeping minimal overhead. The shade packets have highly configurable structure,
and may include various advanced features. The protocol provides diverse
communication capabilities, such as discovery, numeric code communication,
payload data communication, session and session stage management and packet
checksum transmission, while maintaining low overhead for high-throughput or
low bandwidth systems. The unused features of the header can be removed from
the packet, and features with limited use can utilize minimal space. Packet header
section sizes are configurable. The smallest valid data packet is 2 bytes and is a
packet header without payload data (contains 1 byte of potentially useful data). The
smallest packet header overhead of a packet with payload data is 3 bytes (1-256
bytes of payload data), 4 bytes (1-65536 bytes of payload data) or 6 bytes (1-
4294967296 bytes of payload data). The maximum packet header size with full
feature set is 16 bytes.

Reference implementation use:
 - Supports all modes
 - Can parse received packet right away
 - Packet construction starts with setting mode/PFC.
 - Setter functions return the values actually set
