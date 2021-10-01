#pragma once

#include <chrono>

// Timer where 1 unit is 1 million CPU cycles
class Timer {
public:
	Timer();
	void set_timer();
	double get_timer();
private:
	std::chrono::high_resolution_clock::time_point start_stamp;
};
