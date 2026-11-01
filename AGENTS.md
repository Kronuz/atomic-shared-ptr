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

## Verification and benchmarks

The shared method is the catalog's
[PERFORMANCE.md](https://github.com/Kronuz/cpp-libraries/blob/main/docs/PERFORMANCE.md). Here it
comes down to one test and the sanitizers, from a fresh clone:

```sh
cmake -B build && cmake --build build && ctest --test-dir build --output-on-failure
```

- **Test.** `atomic-shared-ptr` (`test/test.cc`) checks store and load, `exchange`, both outcomes
  of `compare_exchange_strong` (the expected value reloaded on a mismatch), and eight threads
  storing and loading one pointer 10,000 times each. It prints `all atomic_shared_ptr tests passed`.
- **CI.** `.github/workflows/ci.yml` calls the catalog's shared
  [`cmake.yml`](https://github.com/Kronuz/cpp-libraries/blob/main/.github/workflows/cmake.yml):
  Linux x86_64 (GCC, Clang) and arm64 (GCC), macOS arm64 and x86_64, and FreeBSD, at `-O2 -g` with
  asserts and the standard library's assertions on, plus ASan+UBSan and TSan. The Linux jobs build
  the alias branch (libstdc++), the macOS and FreeBSD jobs the wrapper (libc++).
- **Sanitizers**, as the CI jobs run them, on Linux with Clang. TSan is the concurrency check. It
  runs on Linux, so it covers the alias branch; on macOS the wrapper's test crashes inside libc++
  under TSan (the caveat above), and the wrapper's threads run in the plain jobs only.

  ```sh
  CC=clang CXX=clang++ cmake -B build-asan -DCMAKE_BUILD_TYPE=Debug \
    "-DCMAKE_CXX_FLAGS=-fsanitize=address,undefined -fno-sanitize-recover=undefined -fno-omit-frame-pointer -D_GLIBCXX_ASSERTIONS" \
    -DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address,undefined
  cmake --build build-asan && ASAN_OPTIONS=detect_leaks=1 ctest --test-dir build-asan --output-on-failure
  CC=clang CXX=clang++ cmake -B build-tsan -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    "-DCMAKE_CXX_FLAGS_RELWITHDEBINFO=-O1 -g -fsanitize=thread -fno-omit-frame-pointer" \
    -DCMAKE_EXE_LINKER_FLAGS=-fsanitize=thread
  cmake --build build-tsan && TSAN_OPTIONS="halt_on_error=1 second_deadlock_stack=1" ctest --test-dir build-tsan --output-on-failure
  ```

- **Not here.** No differential test, mutation script, fuzz target or benchmark, and no
  performance claim to back: every call forwards to the standard library.
