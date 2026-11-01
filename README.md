# atomic-shared-ptr

Part of [Kronuz C++ Libraries](https://github.com/Kronuz/cpp-libraries), a collection of small, reusable C++ libraries.

[![CI](https://github.com/Kronuz/atomic-shared-ptr/actions/workflows/ci.yml/badge.svg)](https://github.com/Kronuz/atomic-shared-ptr/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![Header-only](https://img.shields.io/badge/header--only-yes-green.svg)

Atomic shared_ptr for C++20: `std::atomic<std::shared_ptr<T>>` where the library has it, a
drop-in wrapper elsewhere. Where the standard library defines `__cpp_lib_atomic_shared_ptr`
(libstdc++, for one), `atomic_shared_ptr<T>` is a plain alias of the standard type. Where it
does not (Apple clang's libc++), it is a small class over the free `std::atomic_*` functions
for `shared_ptr`, with the same member API. Reach for it to publish immutable snapshots to
many threads from code that must build on both.

## ✨ Highlights

- **The standard member API.** `load`, `store`, `exchange`, `compare_exchange_weak`,
  `compare_exchange_strong` and `is_lock_free`, each with its `std::memory_order`.
- **Self-retiring.** The alias is chosen by `__cpp_lib_atomic_shared_ptr`, so the wrapper
  disappears from a build as soon as its standard library gains the specialization.
- **A strict drop-in.** The wrapper is neither copyable nor movable and converts to nothing,
  so code that builds against it also builds against the standard type.
- **No dependencies.** One header, standard library only.

## 🚀 Quick start

```cpp
#include "atomic_shared_ptr.h"

#include <cstdio>
#include <memory>
#include <thread>

struct Config { int version = 0; };

int main() {
	atomic_shared_ptr<Config> current{std::make_shared<Config>()};

	std::thread writer([&] {   // publishes the next snapshot
		auto seen = current.load();
		auto next = std::make_shared<Config>(Config{seen->version + 1});
		while (!current.compare_exchange_weak(seen, next)) {
			next = std::make_shared<Config>(Config{seen->version + 1});
		}
	});
	writer.join();

	std::printf("version %d\n", current.load()->version);   // version 1
}
```

## 📦 Installation

Requires a C++20 compiler; CMake 3.16 or newer for the CMake route. CI builds it on Linux
(x86_64, arm64), macOS (arm64, x86_64), and FreeBSD, plus ASan, UBSan, and TSan builds.

```cmake
include(FetchContent)
FetchContent_Declare(atomic-shared-ptr
  GIT_REPOSITORY https://github.com/Kronuz/atomic-shared-ptr.git
  GIT_TAG        9a2582ea13a367d03a07390f1a0f8ec93bc17073)  # main tracks the tip
FetchContent_MakeAvailable(atomic-shared-ptr)

target_link_libraries(my_app PRIVATE atomic-shared-ptr::atomic-shared-ptr)
```

Then `#include "atomic_shared_ptr.h"`, or copy that one header onto your include path.

## 🎯 When to use it

It fits immutable snapshots (configurations, routing tables) published by one thread and read
by many, in code that has to build on standard libraries with and without the
specialization.

Not for:

- **Code that only targets libraries with the specialization.** Use
  `std::atomic<std::shared_ptr<T>>` directly.
- **Lock-free paths.** Neither branch is lock-free on libc++ or libstdc++; `is_lock_free()`
  returns `false` there.

## 📚 Documentation

- [ARCHITECTURE.md](ARCHITECTURE.md): the two branches and why both are drop-ins.
- [AGENTS.md](AGENTS.md): repository map.
- [Catalog entry](https://github.com/Kronuz/cpp-libraries/blob/main/AGENTS.md#atomic-shared-ptr)
  in Kronuz C++ Libraries.

## 🛠️ Building and testing

```sh
cmake -B build && cmake --build build && ctest --test-dir build
```

The test covers each operation and runs an eight-thread smoke test.

## 🤝 Contributing

Issues and pull requests are welcome. Read [AGENTS.md](AGENTS.md) and the catalog's
[rules of engagement](https://github.com/Kronuz/cpp-libraries/blob/main/AGENTS.md#rules-of-engagement)
first; CI must pass on every platform.

## 📄 License

MIT, Copyright (c) 2026 Germán Méndez Bravo and (c) 2015-2020 Dubalu LLC. See
[LICENSE](LICENSE). Extracted from [Xapiand](https://github.com/Kronuz/Xapiand).
