# Architecture

`atomic_shared_ptr.h` has two branches, chosen at compile time by the standard library's
feature-test macro, and one API.

## The two branches

- **Alias.** When `<version>` defines `__cpp_lib_atomic_shared_ptr` (P0718, the C++20
  `std::atomic<std::shared_ptr<T>>` specialization), `atomic_shared_ptr<T>` is
  `using atomic_shared_ptr = std::atomic<std::shared_ptr<T>>`. The header adds nothing.
  libstdc++ 12 and later take this branch.
- **Wrapper.** Otherwise `atomic_shared_ptr<T>` is a class with one `std::shared_ptr<T>`
  member that routes every operation through the free functions that give atomic access to a
  `shared_ptr` (`std::atomic_load_explicit`, `std::atomic_store_explicit`,
  `std::atomic_exchange_explicit`, `std::atomic_compare_exchange_*_explicit`,
  `std::atomic_is_lock_free`). C++20 deprecates those functions, and they are the only working
  option on a standard library without the specialization, such as Apple clang's libc++.

## Why both are drop-ins

The wrapper exposes exactly the member API of the standard specialization: `load`, `store`,
`exchange`, `compare_exchange_weak`, `compare_exchange_strong` (each with a single-order and
a success/failure-order overload, defaulting to `seq_cst`), `is_lock_free`, and assignment
from a `std::shared_ptr<T>`. Like `std::atomic`, it is neither copyable nor movable, and it
has no conversion to `std::shared_ptr<T>`. Code that compiles against one branch therefore
compiles against the other, and the wrapper retires without source changes when a toolchain
gains P0718.

## Memory model

Every operation forwards its `std::memory_order` to the underlying call; the wrapper adds no
ordering of its own. Whether the operations are lock-free is the standard library's answer,
reported by `is_lock_free()`; on libc++ and libstdc++ it is `false`.

## Dependencies

None. Standard library only (`<atomic>`, `<memory>`, `<utility>`, `<version>`).

## Invariants

- Semantics match `std::atomic<std::shared_ptr<T>>`; keep the two branches interchangeable.
- Neither copyable nor movable, and no implicit conversion: the alias branch has none of
  them, so adding one to the wrapper would let code build on old toolchains and break on new
  ones.
