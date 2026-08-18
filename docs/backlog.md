# Execution dashboard

GitHub Issues are the source of truth. This page explains the intended order;
the pinned dashboard issue and priority labels provide the live view.

## Do this next

**#1 — Restore a clean, reproducible C++20 build.**

The unit-test executables pass, but the server target does not link in a fresh
build, the local implementation has no commit, and generated artifacts are
mixed into the working tree. Do not add another command or event-loop feature
until this gate is green.

## Dependency-ordered path

1. **Reproducible baseline and basic CI:** #1
2. **Ownership and first vertical slice:** #2, then #3
3. **Start ongoing TCP integration coverage:** begin #14 after #3 and extend it
   alongside every later networking and protocol issue
4. **Multi-client network correctness:** #4
5. **Protocol and command boundary:** #6, then #7
6. **Incremental buffered input:** #5
7. **Useful Redis subset:** #8, then #10, then #9
8. **Production-shaped I/O:** #11 and #12
9. **Expiration and verification:** #13, finish #14, then #15
10. **Resume-ready finish:** #16 and #17
11. **Optional post-v1 experiment:** #18

Issues in the same step may be worked independently when their stated
dependencies are satisfied.

## Priority meaning

- **P0:** the only issue to start now
- **P1:** required core path or ongoing integration infrastructure
- **P2:** v1 correctness and engineering hardening
- **P3:** finish work or post-v1 exploration

Only one open issue should carry `status:next`. Move that label when the current
issue closes so the answer to “what should I do now?” stays visible.
