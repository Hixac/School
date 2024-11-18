#pragma once

#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <filesystem>
#include <queue>

#include <math.h>
#include <signleton_counter.hpp>

namespace My {

	static std::vector<int> read_block(std::ifstream& file, size_t block_size) {
		std::vector<int> block;
		int num;
		size_t count = 0;
		while (count < block_size && file >> num) {
			block.push_back(num);
			++count;
		}
		return block;
	}

	static void write_block(const std::vector<int>& block, const std::filesystem::path& filename) {
		std::ofstream file(filename);
		for (int num : block) {
			Counter::Get().IncMov();
			file << num << ' ';
		}
	}

	static std::vector<std::filesystem::path> create_sorted_blocks(const std::filesystem::path& input_filename) {
		std::ifstream input_file(input_filename);
		std::vector<std::filesystem::path> temp_files;
		size_t file_index = 0;

		while (!input_file.eof()) {
			auto block = read_block(input_file, My::Counter::Get().GetBlockSize());

			if (block.empty()) break;

			std::sort(block.begin(), block.end());
			Counter::Get().AddComp(block.size() * log2(block.size()));
			Counter::Get().AddMov(log2(block.size()));

			std::filesystem::path temp_filename = "temp_" + std::to_string(file_index++) + ".txt";
			write_block(block, temp_filename);
			temp_files.push_back(temp_filename);
		}

		return temp_files;
	}

	static void multiway_merge(const std::vector<std::filesystem::path>& input_files, const std::filesystem::path& output_filename) {
		std::vector<std::ifstream> file_streams;
		for (const auto& file : input_files) {
			file_streams.emplace_back(file);
		}

		std::ofstream output_file(output_filename);

		auto cmp = [](const std::pair<int, size_t>& a, const std::pair<int, size_t>& b) {
			return a.first > b.first; Counter::Get().IncComp();
		};
		std::priority_queue<std::pair<int, size_t>, std::vector<std::pair<int, size_t>>, decltype(cmp)> min_heap(cmp);

		for (size_t i = 0; i < file_streams.size(); ++i) {
			int num;
			if (file_streams[i] >> num) {
				min_heap.emplace(num, i);
			}
		}

		while (!min_heap.empty()) {
			auto [min_val, file_index] = min_heap.top();
			min_heap.pop();
			output_file << min_val << ' ';

			int next_val;
			if (file_streams[file_index] >> next_val) {
				min_heap.emplace(next_val, file_index);
			}
		}

		for (auto& fs : file_streams) {
			fs.close();
		}
		for (const auto& file : input_files) {
			std::filesystem::remove(file);
		}
	}
	
	static void merge_files(const std::filesystem::path& file1, const std::filesystem::path& file2, const std::filesystem::path& output_filename) {
		std::ifstream f1(file1), f2(file2);
		std::ofstream output_file(output_filename);

		int num1, num2;
		bool has_num1 = static_cast<bool>(f1 >> num1);
		bool has_num2 = static_cast<bool>(f2 >> num2);

		while (has_num1 && has_num2) {
			Counter::Get().IncComp();
			Counter::Get().IncMov();
			if (num1 < num2) {
				output_file << num1 << ' ';
				has_num1 = static_cast<bool>(f1 >> num1);
			} else {
				output_file << num2 << ' ';
				has_num2 = static_cast<bool>(f2 >> num2);
			}
		}

		while (has_num1) {
			Counter::Get().IncMov();
			output_file << num1 << ' ';
			has_num1 = static_cast<bool>(f1 >> num1);
		}

		while (has_num2) {
			Counter::Get().IncMov();
			output_file << num2 << ' ';
			has_num2 = static_cast<bool>(f2 >> num2);
		}

		f1.close();
		f2.close();
		if (file1 != output_filename)
			std::filesystem::remove(file1);
		if (file2 != output_filename)
			std::filesystem::remove(file2);
	}

	static void merge_all_files(std::vector<std::filesystem::path>& temp_files) {
		while (temp_files.size() > 1) {
			std::vector<std::filesystem::path> new_temp_files;

			for (size_t i = 0; i < temp_files.size(); i += 2) {
				if (i + 1 < temp_files.size()) {
					std::filesystem::path merged_file = "temp_merged_" + std::to_string(i / 2) + ".txt";
					merge_files(temp_files[i], temp_files[i + 1], merged_file);
					new_temp_files.push_back(merged_file);
				} else {
					new_temp_files.push_back(temp_files[i]);
				}
			}

			temp_files = new_temp_files;
		}
	}

	static void external_multiway_sort(const std::filesystem::path& input_filename, const std::filesystem::path& output_filename) {
		auto temp_files = create_sorted_blocks(input_filename);

		multiway_merge(temp_files, output_filename);
	}
	
	static void external_merge_sort(const std::filesystem::path& input_filename, const std::filesystem::path& output_filename) {
		auto temp_files = create_sorted_blocks(input_filename);

		merge_all_files(temp_files);

		if (!temp_files.empty()) {
			std::filesystem::rename(temp_files[0], output_filename);
		}
	}

}
