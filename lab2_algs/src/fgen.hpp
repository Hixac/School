#pragma once

#include <base.hpp>
#include <filesystem>
#include <random>

namespace My {

	static void generate_file(std::filesystem::path path)
	{
		FileOut out(path, std::ios::trunc);
		
	    std::random_device r;
		std::default_random_engine e1(r());
		std::uniform_int_distribution<int> uniform_dist(1, 5001);

		for (int i = 0; i < 5000; ++i) {
			out << uniform_dist(e1) << ' ';
		}
	}
	
}
