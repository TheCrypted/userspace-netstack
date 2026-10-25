# User-space network stack

A C++20 learning project implementing an IPv4 network stack on Linux through a TUN interface.

The goal is to support ICMP ping, UDP echo on port 9000, and a minimal TCP echo server on port 8080 at `10.0.0.2`. The host-side TUN address will be `10.0.0.1/24`.

## Build

Requires Linux, a C++20 compiler, CMake 3.20 or newer, and GoogleTest/GoogleMock. On Ubuntu, install the test dependencies with `sudo apt install libgtest-dev libgmock-dev`.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The executable is `build/network_stack`. Use `-DBUILD_TESTING=OFF` when configuring to build without test dependencies.

## Run

Create a persistent TUN interface owned by your user (repeat after reboot or WSL shutdown):

```sh
sudo ip tuntap add dev tun0 mode tun user "$USER"
sudo ip addr add 10.0.0.1/24 dev tun0
sudo ip link set tun0 up
./build/network_stack
```

In separate terminals, run `sudo tcpdump -ni tun0 -vv -XX` and `ping -c 3 10.0.0.2`. The program logs packet lengths and IP versions. Ping replies are not implemented yet.

## Formatting

VS Code's Microsoft C/C++ extension formats C++ files on save using `.clang-format`: four-space indentation, attached braces, and a 100-column wrapping target. Include order is preserved. Use Format Document to format manually.

For command-line formatting, install `clang-format` (`sudo apt install clang-format` on Ubuntu), then run:

```sh
clang-format -i include/*.hpp src/*.cpp tests/unit/*.cpp tests/support/*.hpp
```

## Current status

TUN packet reads and writes use RAII descriptor ownership and injectable syscalls, including cleanup. Unit tests cover descriptor moves, packet I/O, retries, and errors.

Milestone 2 is in progress: endian read/write helpers, fixed IPv4 header extraction, IHL/length validation, and a separate header checksum verifier are implemented. The executable currently reads TUN traffic and logs length/version; it does not yet call the full parser or checksum verifier. All 41 unit tests pass; live interoperability evidence remains to be recorded.

## Next steps

Finish IPv4 version and unsupported-feature rejection, integrate checksum verification, and add header serialization and general checksum tests including odd-length inputs. Then implement ICMP, UDP, an epoll/timer runtime, and TCP handshake, data, close, and retransmission. Record tests and packet-capture evidence for each stage, followed by reproducible latency and CPU measurements.

IPv6, Ethernet/TAP, routing, fragmentation support, and advanced TCP features are outside the initial scope. Intended for a local lab environment.
