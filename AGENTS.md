# AGENTS

`atomic_shared_ptr<T>`: an `std::atomic`-shaped member API over the free
`std::atomic_*(std::shared_ptr*)` functions. Extracted verbatim from Xapiand.

- Header-only, zero dependencies (`<atomic>`/`<memory>`).
- Non-copyable (like `std::atomic`); movable.
- `test/test.cc` covers store/load, exchange, CAS (match + mismatch), the implicit
  conversion, and an 8-thread store/load smoke test -- run it under TSAN after changes:
  `cmake -B build-tsan -DCMAKE_CXX_FLAGS="-fsanitize=thread -g" && cmake --build build-tsan && ./build-tsan/atomic-shared-ptr_test`
- Mirrors `std::atomic<std::shared_ptr>` semantics; keep it a drop-in for that.
