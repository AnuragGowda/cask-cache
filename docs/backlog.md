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

1. **Reproducible baseline:** #1
2. **Ownership and first vertical slice:** #2, then #3
3. **Protocol core:** #6, then #7
4. **Multi-client correctness:** #4, then #5
5. **Useful Redis subset:** #8, then #10, then #9
6. **Production-shaped I/O:** #11 and #12
7. **Expiration and verification:** #13, #14, and #15
8. **Resume-ready finish:** #16 and #17
9. **Optional post-v1 experiment:** #18

Issues in the same step may be worked independently when their stated
dependencies are satisfied.

## Priority meaning

- **P0:** the only issue to start now
- **P1:** required core path; choose the first unblocked item in the order above
- **P2:** v1 correctness and engineering hardening
- **P3:** finish work or post-v1 exploration

Only one open issue should carry `status:next`. Move that label when the current
issue closes so the answer to “what should I do now?” stays visible.
