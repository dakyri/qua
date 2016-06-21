#pragma once
#include <mutex>
#include <chrono>

using namespace std;
using namespace chrono;
class shared_mutex
{
	mutex mut_;
	cond_var gate1_;
	cond_var gate2_;
	unsigned state_;

	static const unsigned write_entered_ = 1U << (sizeof(unsigned)*CHAR_BIT - 1);
	static const unsigned n_readers_ = ~write_entered_;

public:

	shared_mutex() : state_(0) {}

	// Exclusive ownership

	void lock();
	bool try_lock();
	bool timed_lock(nanoseconds rel_time);
	void unlock();

	// Shared ownership

	void lock_shared();
	bool try_lock_shared();
	bool timed_lock_shared(nanoseconds rel_time);
	void unlock_shared();
};
