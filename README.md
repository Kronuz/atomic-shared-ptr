# atomic-shared-ptr

Part of [Kronuz C++ Libraries](https://github.com/Kronuz/cpp-libraries), a collection of small, reusable C++ libraries.

[![CI](https://github.com/Kronuz/atomic-shared-ptr/actions/workflows/ci.yml/badge.svg)](https://github.com/Kronuz/atomic-shared-ptr/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![Header-only](https://img.shields.io/badge/header--only-yes-green.svg)

`std::atomic<std::shared_ptr<T>>` on every toolchain, in C++20. Where your standard
library has it, this header is a plain alias. Where it doesn't (Apple clang's libc++),
a small wrapper provides the same member API until it does.

## ✨ Highlights

- 🔁 **The standard API**: `load`, `store`, `exchange`, `compare_exchange_weak` and
  `_strong`, `is_lock_free`.
- 🪂 **Self-retiring**: it selects the alias by `__cpp_lib_atomic_shared_ptr`, so the
  wrapper disappears once the toolchain gains P0718.
- 🧷 **A strict drop-in**: non-copyable, non-movable, no implicit conversions, so code
  that builds against it also builds against the standard type.
- 🪶 **No dependencies**: one header, standard library only.

## 📦 Installation

Requires a C++20 compiler; CMake 3.14 or newer for the CMake route. CI builds it on
Linux (x86_64, arm64), macOS (arm64, x86_64), and FreeBSD, plus ASan, UBSan, and TSan
builds.

```cmake
include(FetchContent)
FetchContent_Declare(atomic-shared-ptr
  GIT_REPOSITORY https://github.com/Kronuz/atomic-shared-ptr.git
  GIT_TAG        main)
FetchContent_MakeAvailable(atomic-shared-ptr)

target_link_libraries(my_app PRIVATE atomic-shared-ptr::atomic-shared-ptr)
```

Or copy `atomic_shared_ptr.h` onto your include path.

## 🚀 Quick start

```cpp
#include "atomic_shared_ptr.h"

#include <memory>

struct Config { int version = 0; };

atomic_shared_ptr<Config> current{std::make_shared<Config>()};

void publish(int version) {   // any thread
	auto cur = current.load();
	auto next = std::make_shared<Config>(Config{version});
	while (!current.compare_exchange_weak(cur, next)) {}
}

int read_version() {          // any thread, never sees a torn pointer
	return current.load()->version;
}
```

## 🎯 When to use it

- ✅ Publishing immutable snapshots (configs, routing tables) to many reader threads.
- ❌ Code that only targets standard libraries with the specialization: use
  `std::atomic<std::shared_ptr<T>>` directly.

## 📚 Documentation

- [ARCHITECTURE.md](ARCHITECTURE.md): the two branches and why both are drop-ins.
- [AGENTS.md](AGENTS.md): repository map.

## 🛠️ Building and testing

```sh
cmake -B build && cmake --build build && ctest --test-dir build   # includes an 8-thread smoke test
```

## 🤝 Contributing

Issues and pull requests are welcome. Read [AGENTS.md](AGENTS.md) first. CI must
pass on every platform.

## 📄 License

MIT, Copyright (c) 2015-2018 Dubalu LLC. See [LICENSE](LICENSE). Extracted from
[Xapiand](https://github.com/Kronuz/Xapiand).
