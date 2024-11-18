#pragma once

#include <base.hpp>
#include <filesystem>
#include <random>

namespace My {

	static void generate_file(const std::filesystem::path& path, int from, int to, size_t size) {
		FileOut out(path, std::ios::trunc);
		
	    std::random_device r;
		std::default_random_engine e1(r());
		std::uniform_int_distribution<int> uniform_dist(from, to);

		for (int i = 0; i < size; ++i) {
			out << uniform_dist(e1) << ' ';
		}
	}

	static void generate_sort_file(const std::filesystem::path& path, size_t size) {
		FileOut out(path, std::ios::trunc);

		for (int i = 0; i < size; ++i) {
			out << i << ' ';
		}
	}

	static void generate_versa_sort_file(const std::filesystem::path& path, size_t size) {
		FileOut out(path, std::ios::trunc);

		for (int i = size; i > 0; --i) {
			out << i << ' ';
		}
	}
	
}
