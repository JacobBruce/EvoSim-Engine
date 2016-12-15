#pragma once
#include <cstdlib>
#include <cstdint>
#include <string>
#include <cmath>
#include <array>
#include <random>
#include <sstream>

using namespace std;

inline string IntToStr(int number)
{
	ostringstream s;
	s << number;
	return s.str();
}

inline string FltToStr(float number)
{
	ostringstream s;
	s << fixed << number;
	return s.str();
}

inline string DblToStr(double number)
{
	ostringstream s;
	s << number;
	return s.str();
}

inline string LngToStr(long number)
{
	ostringstream s;
	s << fixed << number;
	return s.str();
}

inline wstring StrToWstr(string str) 
{
	return wstring(str.begin(), str.end());
}

inline wstring IntToWstr(int number)
{
	return StrToWstr(IntToStr(number));
}

inline int64_t RoundDbl(double num)
{
	return (num > 0.0) ? floor(num + 0.5) : ceil(num - 0.5);
}

inline int rand_range(int lowest, int highest)
{
	return lowest + (rand() % (highest-lowest+1i32));
}

inline mt19937 SeededRandomEngine() {
    static std::random_device r;
    std::array<unsigned int, mt19937::state_size> seed_data;
    std::generate_n(seed_data.data(), seed_data.size(), std::ref(r));
    std::seed_seq seeds(std::begin(seed_data), std::end(seed_data));
    mt19937 seededEngine(seeds);
    return seededEngine;
}