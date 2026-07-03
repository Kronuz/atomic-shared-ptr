# atomic-shared-ptr

`atomic_shared_ptr<T>` -- thread-safe atomic operations over a `std::shared_ptr<T>`,
wrapping the free `std::atomic_*` shared_ptr functions behind the same member API as
`std::atomic` (`load`/`store`/`exchange`/`compare_exchange_weak`/`_strong`, plus an
implicit conversion to `std::shared_ptr<T>`). Extracted from
[Xapiand](https://github.com/Kronuz/Xapiand).

```cpp
#include "atomic_shared_ptr.h"

atomic_shared_ptr<Node> head{std::make_shared<Node>()};
auto cur = head.load();
auto next = std::make_shared<Node>();
while (!head.compare_exchange_weak(cur, next)) { /* retry */ }
```

Predates (and mirrors) the C++20 `std::atomic<std::shared_ptr<T>>`, so it is a drop-in
for toolchains/codebases that cannot yet rely on that specialization. Header-only.

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
