#pragma once

#include <string>
#include <fstream>

namespace My {

	using std::size_t;
	
#if defined(__unix__)

	using StrView = std::string_view;
	using String = std::string;
	using FileIn = std::ifstream;
	using FileOut = std::ofstream;
	
#elif defined(_WIN32) || defined(WIN32)

	using StrView = std::wstring_view;
	using String = std::wstring;
	using FileIn = std::wifstream;
	using FileOut = std::wofstream;

#endif
	
}
