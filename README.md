# Cask Cache

A Redis-compatible in-memory server built in C++20 to develop and demonstrate
systems-programming depth.

The first release is intentionally narrow: a correct `poll`-based TCP server,
incremental RESP2 parsing, a useful core command set, expiration, tests,
sanitizers, documentation, and reproducible benchmarks. Replication, clustering,
Lua, transactions, persistence, authentication, and TLS are not v1 goals.

## Working agreement

- GitHub Issues describe shippable repository work and acceptance criteria.
- Milestones describe product increments, not calendar weeks.
- The current milestone and one concrete issue define project focus.
- Learning sessions, LeetCode, applications, health, and daily planning stay in
  POL rather than becoming repository issues.
- `poll` remains the baseline until fragmented reads, pipelining, partial writes,
  disconnects, and slow clients are tested reliably.

See [the roadmap](docs/roadmap.md) for scope and the [execution
dashboard](docs/backlog.md) for the current dependency-ordered backlog.

## Status

The local working tree has early store, parser, executor, socket, and `poll`
server code plus three passing unit-test executables. The immediate gate is
issue #1: make a fresh macOS build produce the server, add basic Linux CI, keep
generated artifacts out of version control, and commit the implementation. No
end-to-end server claim is made yet.
