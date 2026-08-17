# Roadmap

## Definition of v1

### Networking

- TCP listener with clean connection teardown
- Multiple simultaneous clients using `poll`
- Per-connection input and output buffers
- Correct partial reads, partial writes, pipelining, and slow-client handling
- No obvious memory or file-descriptor leaks

### Protocol and commands

- Incremental, binary-safe RESP2 parser
- Split requests and multiple requests in one read
- Useful malformed-request and invalid-command errors
- Basic `redis-cli` compatibility
- `PING`, `ECHO`, `GET`, `SET`, `DEL`, `EXISTS`, `INCR`, `DECR`, `EXPIRE`, and `TTL`

### Engineering quality

- Parser and command unit tests
- End-to-end integration tests
- CI with strict warnings, ASan, and UBSan
- Reproducible benchmark with limitations stated
- Build instructions, architecture notes, command table, and known limitations

## Milestones

1. **M1 — Network loop:** build scaffold, RAII file descriptor ownership, one
   client, multiple clients with `poll`, and clean teardown.
2. **M2 — RESP and core commands:** incremental parsing, command dispatch,
   initial commands, fragmented requests, and `redis-cli` compatibility.
3. **M3 — Production-shaped correctness:** pipelining, partial writes,
   bounded buffering, expiration, integration tests, and sanitizer coverage.
4. **v1.0 — Resume ready:** benchmark, architecture and usage documentation,
   stable CI, supported-command table, and known limitations.

`epoll` is a post-v1 comparison unless the `poll` version is already correct,
tested, and documented.

## Decision gates

### After M1

- Can every major object's owner be explained?
- Do multiple clients connect and disconnect without descriptor leaks?
- Are focused project hours sustainable alongside sleep, work, and exercise?

### After M2

- Does the parser survive arbitrary read boundaries?
- Can `redis-cli` execute the implemented commands?
- Can the architecture be explained without hand-waving?

### After v1

Choose the next systems direction from actual experience: networking/proxies,
storage/caching, low-latency infrastructure, or runtimes/operating systems.

