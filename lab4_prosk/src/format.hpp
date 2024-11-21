#pragma once

#include <string>
#include <string_view>
#include <functional>
#include <vector>
#include <fstream>

namespace My {

	class Format {
	public:
		Format(std::string_view text);
		Format(std::ifstream& in);

		std::vector<std::string> split_by_space();
		void do_every_word(std::function<std::string(std::string_view s)> fn);
		
		static std::string cut_end(std::string_view str, std::string_view sub);
		static std::string cut_end_letters(std::string_view str, std::vector<char> chars, int range);

		inline std::string_view get_text() { return m_text; }
		
	private:
		std::string m_text;
	};
	
}
