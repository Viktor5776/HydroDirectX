#pragma once
#include <chrono>

class HydroTimer
{
public:
	HydroTimer()
	{
		last = std::chrono::steady_clock::now();
	}
	float Mark()
	{
		const auto old = last;
		last = std::chrono::steady_clock::now();
		return std::chrono::duration<float>( last - old ).count();
	}
	float Peek() const
	{
		return std::chrono::duration<float>( std::chrono::steady_clock::now() - last ).count();
	}
private:
	std::chrono::steady_clock::time_point last;
};
