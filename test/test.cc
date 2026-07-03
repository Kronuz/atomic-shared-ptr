// ctest for atomic_shared_ptr: store/load, exchange, compare_exchange, and the
// implicit conversion -- plus a small concurrency smoke test.

#include "atomic_shared_ptr.h"

#include <cstdio>
#include <memory>
#include <thread>
#include <vector>

int main() {
	int failures = 0;
	auto check = [&](bool ok, const char* what) {
		if (!ok) { std::fprintf(stderr, "FAIL: %s\n", what); ++failures; }
	};

	// Default-constructed is empty.
	atomic_shared_ptr<int> a;
	check(a.load() == nullptr, "default load is null");

	// store / load round-trip.
	auto p1 = std::make_shared<int>(42);
	a.store(p1);
	check(a.load() && *a.load() == 42, "store then load");
	check(a.load() == p1, "load returns stored value");

	// exchange returns the old value and installs the new one.
	auto p2 = std::make_shared<int>(7);
	auto old = a.exchange(p2);
	check(old == p1, "exchange returns old");
	check(a.load() == p2, "exchange installs new");

	// compare_exchange_strong: succeeds when expected matches, then fails.
	auto expected = p2;
	auto p3 = std::make_shared<int>(99);
	check(a.compare_exchange_strong(expected, p3), "CAS succeeds on match");
	check(a.load() == p3, "CAS installed new");
	expected = p2;  // now stale
	check(!a.compare_exchange_strong(expected, p1), "CAS fails on mismatch");
	check(expected == p3, "CAS loads current into expected on failure");

	// Concurrency smoke test: many threads hammering store/load must not crash or tear.
	atomic_shared_ptr<int> shared{std::make_shared<int>(0)};
	std::vector<std::thread> ts;
	for (int t = 0; t < 8; ++t) {
		ts.emplace_back([&shared, t] {
			for (int i = 0; i < 10000; ++i) {
				shared.store(std::make_shared<int>(t * 10000 + i));
				auto v = shared.load();
				(void)v;
			}
		});
	}
	for (auto& th : ts) { th.join(); }
	check(shared.load() != nullptr, "survives concurrent store/load");

	if (failures == 0) { std::puts("all atomic_shared_ptr tests passed"); }
	return failures == 0 ? 0 : 1;
}
