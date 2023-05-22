#pragma once
#pragma once

// Framework.h is the lowest-level include header to be used by anything and everything

#pragma region STL Includes
#define _USE_MATH_DEFINES

#include <algorithm>
#include <bitset>
#include <cassert>
#include <chrono>
#include <deque>
#include <fstream>
#include <functional>
#include <immintrin.h>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stack>
#include <stdint.h>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Remove need for std namespace scope for very common datatypes
using std::vector;
using std::map;
using std::unordered_map;
using std::set;
using std::multiset;
using std::unordered_set;
using std::list;
using std::stack;
using std::deque;
using std::string;
using std::wstring;
using std::pair;

typedef uint8_t byte;
#pragma endregion

// Current millisecond time
#define CUR_MS() (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count())

#pragma region Logic Macros
#define MAX(a, b) ((a > b) ? a : b)
#define MIN(a, b) ((a < b) ? a : b)

#define CLAMP(val, min, max) MIN(MAX(val, min), max)
#pragma endregion

#pragma region Printing and Strings
#define LOG(s) { std::cout << STR(s << std::endl); }
#define STR(s) ([&]{ std::stringstream __macroStream; __macroStream << s; return __macroStream.str(); }())

// DLOG (debug-log) only works in debug builds
#ifdef _DEBUG
#define DLOG(s) LOG(s)
#else
#define DLOG(s) {}
#endif

#define ERR_CLOSE(s) { \
	LOG("FATAL ERROR: " << s); \
	assert(false); \
	exit(EXIT_FAILURE); \
}

#pragma endregion

#pragma region Assertions
#define ASSERT assert
#define SASSERT static_assert
#pragma endregion

#define THROW(s) throw std::runtime_error(STR(s))

#define FINLINE inline

// Make sure we are 64-bit
SASSERT(sizeof(void*) == 8, "GridentifyBot can only run as a 64-bit program");

// Get a bit of an integer
#define INTBIT(x, i) (((uint64_t)x >> (uint64_t)i) & 1)

inline vector<string> __SPLIT_STR(const string& str, const string& delim) {
	vector<string> results;

	size_t startPos = 0;
	size_t endPos;

	string tokenBuffer;
	while ((endPos = str.find(delim, startPos)) != string::npos) {
		tokenBuffer = str.substr(startPos, endPos - startPos);
		startPos = endPos + delim.size();
		results.push_back(tokenBuffer);
	}

	results.push_back(str.substr(startPos));
	return results;
}

#define SPLIT_STR __SPLIT_STR