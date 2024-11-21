#include <format.hpp>

#include <sstream>
#include <algorithm>

namespace My {

	Format::Format(std::string_view text) : m_text(text) {
		
	}

	Format::Format(std::ifstream& in) {
		m_text = "";
		
		std::string buf;
		while (in >> buf) {
			m_text += buf;
			m_text += " ";
		}
	}

	std::vector<std::string> Format::split_by_space() {
		std::stringstream ss(m_text);

		std::vector<std::string> vec;

		std::string buf;
		while (ss >> buf) {
			vec.push_back(buf);
		}

		return vec;
	}

	void Format::do_every_word(std::function<std::string(std::string_view s)> fn) {
		auto vec = split_by_space();

		for (size_t i = 0; i < vec.size(); ++i) {
			vec[i] = fn(vec[i]);
		}

		m_text.clear();
		m_text = "";

		for (auto& s : vec) {
			m_text += s;
			m_text += " ";
		}
	}
	
	std::string Format::cut_end(std::string_view str, std::string_view sub) {
		std::string ret(str);

		if (ret.length() > sub.length() && ret.substr(0, sub.length()) == sub) {
		    ret.erase(sub.length(), ret.length() - sub.length());
		}

		return ret;
	}

	std::string Format::cut_end_letters(std::string_view str, std::vector<char> chars, int range) {
		std::string ret(str);

		for (size_t i = ret.length() - 1; i > ret.length() - 4; --i) {
			if (std::any_of(chars.begin(), chars.end(), [&](char el) { return el == ret[i]; })) {
				ret.erase(i, 1);
			}
		}

		return ret;
	}
	
}
