#include "Utils.hpp"

Timer::Timer()
{
	set_timer();
}

void Timer::set_timer()
{
	start_stamp = std::chrono::high_resolution_clock::now();
}

double Timer::get_timer()
{
	std::chrono::high_resolution_clock::time_point end_stamp
		= std::chrono::high_resolution_clock::now();

	return (end_stamp - start_stamp).count() / 1'000'000.0;
}
