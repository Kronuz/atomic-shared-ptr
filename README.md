# atomic-shared-ptr

`atomic_shared_ptr<T>` -- thread-safe atomic operations over a `std::shared_ptr<T>`,
with the same member API as `std::atomic` (`load`/`store`/`exchange`/
`compare_exchange_weak`/`_strong`/`is_lock_free`). Extracted from
[Xapiand](https://github.com/Kronuz/Xapiand).

```cpp
#include "atomic_shared_ptr.h"

atomic_shared_ptr<Node> head{std::make_shared<Node>()};
auto cur = head.load();
auto next = std::make_shared<Node>();
while (!head.compare_exchange_weak(cur, next)) { /* retry */ }
```

## Self-retiring

C++20 (P0718) adds a `std::atomic<std::shared_ptr<T>>` specialization that does exactly
this. This header **retires itself gracefully**:

- Where the standard library implements it (feature-test macro
  `__cpp_lib_atomic_shared_ptr`), `atomic_shared_ptr<T>` is a plain alias for
  `std::atomic<std::shared_ptr<T>>` and this header adds nothing.
- Where it is not yet implemented (notably Apple clang's libc++ as of clang 17, which
  lacks the specialization but still provides the pre-C++20 free
  `std::atomic_*(std::shared_ptr*)` functions), it falls back to a small wrapper over
  those functions, exposing the **same** `std::atomic<std::shared_ptr>` member API.

Both branches are strict drop-ins for `std::atomic<std::shared_ptr>` (non-copyable,
non-movable, no implicit conversion), so a consumer that sticks to that member API builds
unchanged on either, and the wrapper disappears automatically once the toolchain gains
P0718. Header-only.

## Dependencies

**None** — header-only, standard library only.

## Build / test
```sh
cmake -B build && cmake --build build && ctest --test-dir build   # incl. an 8-thread smoke test
```

## Use (FetchContent)
```cmake
FetchContent_Declare(atomic-shared-ptr GIT_REPOSITORY https://github.com/Kronuz/atomic-shared-ptr.git GIT_TAG main)
FetchContent_MakeAvailable(atomic-shared-ptr)
target_link_libraries(your_app PRIVATE atomic-shared-ptr::atomic-shared-ptr)
```
