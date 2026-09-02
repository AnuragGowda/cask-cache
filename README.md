# Cask Cache

A small Redis-compatible cache server in C++23, built from first principles to
learn network programming, wire protocols, and storage-engine design.

> [!IMPORTANT]
> Cask Cache is an active learning project, not a production Redis replacement.
> The TCP server and RESP command parser are in progress; command execution and
> the in-memory store are not wired up yet.

## Current state

| Layer | Status |
| --- | --- |
| TCP server | Non-blocking clients managed with `poll` |
| Socket lifetime | Move-only RAII wrapper |
| RESP parsing | Arrays, bulk strings, and incremental-input errors |
| Command decoding | `PING`, `SET`, `GET`, `DEL`, `INCR`, `EXPIRE`, and `TTL` |
| Command execution | Not yet connected to the server loop |
| In-memory store | Not yet implemented |

The current executable accepts multiple clients on port `8080` and echoes
received bytes. The RESP parser and command model live under `src/store` while
the next milestone connects them to the network loop.

## Build

The server currently targets Linux and requires CMake 3.20+ with a compiler
that supports C++23.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/cache_server
```

In another terminal, connect to the work-in-progress server:

```bash
nc localhost 8080
```

## Roadmap

- Connect incremental RESP parsing to each client connection
- Execute commands against an in-memory key-value store
- Add expiration and TTL handling
- Add protocol and integration tests
- Benchmark only after command behavior is correct and reproducible
