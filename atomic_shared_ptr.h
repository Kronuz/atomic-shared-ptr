/*
 * Copyright (c) 2015-2018 Dubalu LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <atomic>       // for std::atomic and the free std::atomic_*(std::shared_ptr*) functions
#include <memory>       // for std::shared_ptr
#include <utility>      // for std::move
#include <version>      // for __cpp_lib_atomic_shared_ptr

/*
 * atomic_shared_ptr<T>: thread-safe atomic operations over a std::shared_ptr<T>.
 *
 * C++20 (P0718) adds a std::atomic<std::shared_ptr<T>> specialization that does
 * exactly this. Where the standard library implements it (feature-test macro
 * __cpp_lib_atomic_shared_ptr), atomic_shared_ptr<T> retires to a plain alias for
 * that type and this header adds nothing.
 *
 * Where it is not yet implemented (notably Apple clang's libc++ as of clang 17,
 * where the specialization is missing but the pre-C++20 free
 * std::atomic_*(std::shared_ptr*) functions still work), we fall back to a small
 * wrapper over those free functions, exposing the SAME member API as
 * std::atomic<std::shared_ptr>. The two branches are interchangeable: a consumer
 * that sticks to the std::atomic member API (load / store / exchange /
 * compare_exchange_* / is_lock_free) compiles unchanged on either. This makes the
 * eventual full retirement a no-op the moment the toolchain gains P0718.
 */

#if defined(__cpp_lib_atomic_shared_ptr)

template <typename T>
using atomic_shared_ptr = std::atomic<std::shared_ptr<T>>;

#else

template <typename T>
class atomic_shared_ptr {
	std::shared_ptr<T> ptr;

public:
	constexpr atomic_shared_ptr() noexcept = default;

	constexpr atomic_shared_ptr(std::shared_ptr<T> desired) noexcept
		: ptr(std::move(desired)) { }

	// Non-copyable and non-movable, exactly like std::atomic<std::shared_ptr>.
	atomic_shared_ptr(const atomic_shared_ptr&) = delete;
	atomic_shared_ptr& operator=(const atomic_shared_ptr&) = delete;

	void operator=(std::shared_ptr<T> desired) noexcept {
		store(std::move(desired));
	}

	bool is_lock_free() const noexcept {
		return std::atomic_is_lock_free(&ptr);
	}

	void store(std::shared_ptr<T> desired, std::memory_order order = std::memory_order_seq_cst) noexcept {
		std::atomic_store_explicit(&ptr, std::move(desired), order);
	}

	std::shared_ptr<T> load(std::memory_order order = std::memory_order_seq_cst) const noexcept {
		return std::atomic_load_explicit(&ptr, order);
	}

	std::shared_ptr<T> exchange(std::shared_ptr<T> desired, std::memory_order order = std::memory_order_seq_cst) noexcept {
		return std::atomic_exchange_explicit(&ptr, std::move(desired), order);
	}

	bool compare_exchange_weak(std::shared_ptr<T>& expected, std::shared_ptr<T> desired, std::memory_order success, std::memory_order failure) noexcept {
		return std::atomic_compare_exchange_weak_explicit(&ptr, &expected, std::move(desired), success, failure);
	}

	bool compare_exchange_weak(std::shared_ptr<T>& expected, std::shared_ptr<T> desired, std::memory_order order = std::memory_order_seq_cst) noexcept {
		return compare_exchange_weak(expected, std::move(desired), order, order);
	}

	bool compare_exchange_strong(std::shared_ptr<T>& expected, std::shared_ptr<T> desired, std::memory_order success, std::memory_order failure) noexcept {
		return std::atomic_compare_exchange_strong_explicit(&ptr, &expected, std::move(desired), success, failure);
	}

	bool compare_exchange_strong(std::shared_ptr<T>& expected, std::shared_ptr<T> desired, std::memory_order order = std::memory_order_seq_cst) noexcept {
		return compare_exchange_strong(expected, std::move(desired), order, order);
	}
};

#endif
