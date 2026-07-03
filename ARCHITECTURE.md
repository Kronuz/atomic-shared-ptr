# Architecture

`atomic_shared_ptr<T>` is a thin, single-member class: it holds one `std::shared_ptr<T>`
and routes every operation through the free `std::atomic_*` functions that specialize
atomic access for `shared_ptr` (`atomic_load_explicit`, `atomic_store_explicit`,
`atomic_exchange_explicit`, `atomic_compare_exchange_*_explicit`).

## The shape

The value is the *interface*: those free functions are awkward and easy to misuse
(pass the wrong address, forget the memory order), so this wraps them behind the same
member API as `std::atomic` — `load` / `store` / `exchange` / `compare_exchange_weak` /
`compare_exchange_strong`, each taking a `std::memory_order` (default
`seq_cst`), plus an implicit conversion to `std::shared_ptr<T>` for read sites.

It is non-copyable (like `std::atomic`) and movable. It predates and mirrors the C++20
`std::atomic<std::shared_ptr<T>>` specialization, so it is a drop-in for toolchains or
codebases that cannot yet rely on that.

## Memory model

Every operation forwards its `std::memory_order` straight to the underlying
`std::atomic_*_explicit` call; the class adds no ordering of its own. Whether the
operations are lock-free is the standard library's call — `is_lock_free()` reports it.

## Dependencies

**None.** Standard library only (`<atomic>`, `<memory>`).

## Invariants

- Semantics match `std::atomic<std::shared_ptr>`; keep it a drop-in for that.
- Non-copyable, movable. Do not add a copy — atomics are not copyable.
- Verbatim from Xapiand.
