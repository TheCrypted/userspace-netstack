# User-space network stack

A C++20 learning project implementing an IPv4 network stack on Linux through a TUN interface.

The goal is to support ICMP ping, UDP echo on port 9000, and a minimal TCP echo server on port 8080 at `10.0.0.2`. The host-side TUN address will be `10.0.0.1/24`.

## Build

Requires Linux, a C++20 compiler, and CMake 3.20 or newer.

```sh
cmake -S . -B build
cmake --build build
```

The executable is `build/app`. TUN access requires `/dev/net/tun` and suitable interface permissions.

## Formatting

VS Code's Microsoft C/C++ extension formats C++ files on save using `.clang-format`: four-space indentation, attached braces, and a 100-column wrapping target. Include order is preserved. Use Format Document to format manually.

For command-line formatting, install `clang-format` (`sudo apt install clang-format` on Ubuntu), then run:

```sh
clang-format -i include/*.hpp src/*.cpp tests/unit/*.cpp tests/support/*.hpp
```

## Current status

Initial implementation: descriptor ownership, TUN attachment and packet reading, and IPv4 version extraction. Packet writing is a placeholder. The executable is not yet connected to the TUN code; it currently prints a startup message and loops until interrupted.

## Next steps

Connect packet reading to the executable, then add IPv4 validation and checksums, ICMP, UDP, an epoll/timer runtime, and TCP handshake, data, close, and retransmission. Each stage will include tests and packet-capture evidence, followed by reproducible latency and CPU measurements.

IPv6, Ethernet/TAP, routing, fragmentation support, and advanced TCP features are outside the initial scope. Intended for a local lab environment.
