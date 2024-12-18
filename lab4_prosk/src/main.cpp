#include <format.hpp>

#include <iostream>
#include <filesystem>
#include <fstream>

int main() {
    int ans;
	std::cout << "Вы хотите [1.ввести текст] или [2.загрузить файл]? (1/2) ";
	std::cin >> ans;

	if (ans == 1) {
		std::string buf;
		std::cout << "Введите текст: ";

		std::string text = "";
	    while (std::getline(std::cin, buf)) {
			text += buf;
			if (buf[buf.length() - 1] == '.') {
				text.erase(text.length() - 1, 1);
				break;
			}
		}
		
		My::Format fmt(text);

		std::cout << "Введите буквы (слитно), которые вам не нравятся, для дальнейшего их удаления в тексте: ";
		std::cin >> buf;
		
		auto fn = std::bind(My::Format::cut_end_letters, std::placeholders::_1, std::vector(buf.begin(), buf.end()));
		fmt.do_every_word(fn);
		
		std::cout << "Выходной текст:\n" << fmt.get_text();
	} else if (ans == 2) {
		std::filesystem::path file_path;
		std::cout << "Введите файл. ";
		std::cin >> file_path;

		std::string buf;
		std::cout << "Введите буквы (слитно), которые вам не нравятся, для дальнейшего их удаления в тексте. ";
		std::cin >> buf;
		
		std::ifstream in(file_path);
		My::Format fmt(in);
		in.close();

		auto fn = std::bind(My::Format::cut_end_letters, std::placeholders::_1, std::vector(buf.begin(), buf.end()));
		fmt.do_every_word(fn);
		
		std::cout << "Выходной текст:\n" << fmt.get_text();

		file_path.concat("_temporary.txt");
		std::ofstream out(file_path, std::ios::trunc);
		out << fmt.get_text();
	}
   
	return 0;
}
