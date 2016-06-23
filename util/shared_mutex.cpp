#include "shared_mutex.h"

void
shared_mutex::lock()
{
	std::this_thread::disable_interruption _;
	unique_lock<mutex> lk(mut_);
	while (state_ & write_entered_)
		gate1_.wait(lk);
	state_ |= write_entered_;
	while (state_ & n_readers_)
		gate2_.wait(lk);
}

bool
shared_mutex::try_lock()
{
	unique_lock<mutex> lk(mut_, try_to_lock);
	if (lk.owns_lock() && state_ == 0)
	{
		state_ = write_entered_;
		return true;
	}
	return false;
}

void
shared_mutex::unlock()
{
	{
		unique_lock<mutex> _(mut_);
		state_ = 0;
	}
	gate1_.notify_all();
}

// Shared ownership

void
shared_mutex::lock_shared()
{
	std::this_thread::disable_interruption _;
	unique_lock<mutex> lk(mut_);
	while ((state_ & write_entered_) || (state_ & n_readers_) == n_readers_)
		gate1_.wait(lk);
	unsigned num_readers = (state_ & n_readers_) + 1;
	state_ &= ~n_readers_;
	state_ |= num_readers;
}

bool
shared_mutex::try_lock_shared()
{
	unique_lock<mutex> lk(mut_, try_to_lock);
	unsigned num_readers = state_ & n_readers_;
	if (lk.owns_lock() && !(state_ & write_entered_) && num_readers != n_readers_)
	{
		++num_readers;
		state_ &= ~n_readers_;
		state_ |= num_readers;
		return true;
	}
	return false;
}

void
shared_mutex::unlock_shared()
{
	scoped_lock<mutex> _(mut_);
	unsigned num_readers = (state_ & n_readers_) - 1;
	state_ &= ~n_readers_;
	state_ |= num_readers;
	if (state_ & write_entered_)
	{
		if (num_readers == 0)
			gate2_.notify_one();
	}
	else
	{
		if (num_readers == n_readers_ - 1)
			gate1_.notify_one();
	}
}