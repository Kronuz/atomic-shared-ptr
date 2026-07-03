# AGENTS

`atomic_shared_ptr<T>`: a **self-retiring** `std::atomic`-shaped API over
`std::shared_ptr<T>`. Extracted from Xapiand.

- Header-only, zero dependencies (`<atomic>`/`<memory>`/`<utility>`/`<version>`).
- **Two branches, one API.** When `__cpp_lib_atomic_shared_ptr` is defined,
  `atomic_shared_ptr<T>` is a plain alias for `std::atomic<std::shared_ptr<T>>`. Otherwise
  it is a small wrapper over the pre-C++20 free `std::atomic_*(std::shared_ptr*)` functions
  (deprecated in C++20 but still the only working option on libc++, which as of clang 17 /
  LLVM 22 has not shipped P0718). Keep the two branches interchangeable.
- **Strict `std::atomic<std::shared_ptr>` drop-in.** Non-copyable AND non-movable; no
  implicit conversion to `std::shared_ptr`. Consumers must use the member API only
  (`load`/`store`/`exchange`/`compare_exchange_weak`/`_strong`/`is_lock_free`). Do NOT
  re-add a move ctor or an `operator std::shared_ptr` — the alias branch has neither, so
  adding them would make code compile on old toolchains and break on new ones.
- `test/test.cc` covers store/load, exchange, CAS (match + mismatch), and an 8-thread
  smoke test. Verify the fallback on Apple clang (`-std=c++20`) and, when a P0718-capable
  stdlib (libstdc++ 12+ / a future libc++) is available, force the alias branch to confirm
  it compiles: `-D__cpp_lib_atomic_shared_ptr=201711L`.
- **TSAN caveat (macOS):** running the smoke test under `-fsanitize=thread` segfaults on
  macOS due to a TSAN interceptor vs libc++ `shared_ptr`-atomics issue, NOT a data race in
  this code (the original header segfaults identically; the non-TSAN run is clean). Validate
  concurrency on a platform where TSAN + libstdc++ cooperate, or rely on the consuming app's
  cluster tests.
