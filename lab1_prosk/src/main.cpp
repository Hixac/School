#include <house_sort.hpp>
#include <house_stream.hpp>

#include <iomanip>
#include <iostream>
#include <string>

#define BRIGHT_YELLOW "\033[92m"
#define YELLOW "\033[33m"
#define BRIGHT_RED "\033[91m"
#define WHITE "\033[m"

#define SET_COLOUR(s, x, res) std::string_view(std::string(x) + std::string(s) + std::string(res))

struct Error {
	int code;
	std::string msg;
};

std::string swallow(std::vector<std::string_view> v)
{
    auto buf = std::string();
	for (auto view : v) {
		buf += view;
	}

	return buf;
}

void print(std::string_view s, std::string_view colour = WHITE)
{
	std::cout << colour << s;
}

void print(std::string_view s, size_t st_w)
{
	std::cout << std::setw(st_w) << (st_w > 0 ? " " : "") << s;
}

void println(std::string_view s, size_t st_w = 0, std::string_view colour = WHITE)
{
	std::cout << colour << std::setw(st_w) << (st_w > 0 ? " " : "") << s << '\n';
	print(""); // RELOAD COLOUR
}

void println(std::string_view s, std::string_view colour)
{
	std::cout << colour << s << '\n';
	print(""); // RELOAD COLOUR
}

void print_help()
{
	println("КОМАНДЫ:", BRIGHT_YELLOW);
    println("созд", 1, BRIGHT_RED);
    println(swallow({"создаёт новый экземпляр ", SET_COLOUR("Квартиры", BRIGHT_RED, YELLOW), " в динамическую память."}), 5, YELLOW);
	println("отобр", 1, BRIGHT_RED);
    println(swallow({"отобразить ", SET_COLOUR("Квартиры, помещённые в динамическую память", BRIGHT_RED, YELLOW), "."}), 5, YELLOW);
	println("сорт", 1, BRIGHT_RED);
    println(swallow({"сортировать ", SET_COLOUR("Квартиры, помещённые в динамическую память", BRIGHT_RED, YELLOW), " по площади."}), 5, YELLOW);
	println("зап", 1, BRIGHT_RED);
    println(swallow({"записать данные с ", SET_COLOUR("динамической памяти в Файл", BRIGHT_RED, YELLOW), "."}), 5, YELLOW);
	println("выв", 1, BRIGHT_RED);
    println(swallow({"вывести данные с ", SET_COLOUR("Файла в динамическую память", BRIGHT_RED, YELLOW), "."}), 5, YELLOW);
	println("вых", 1, BRIGHT_RED);
    println("выйти из программы", 5, YELLOW);
}

Error interface()
{
	Error err { 0, "" };
	
	std::vector<Man::House> dyn;
	std::string buf;

	println("");
	while (buf != "вых") {
		std::cin >> buf;

		if (buf == "help") {
			print_help();
		} else if (buf == "созд") {
			Man::House h;
			println(swallow({"Введите следующие параметры соответственно:  ",
				   SET_COLOUR("адрес, кол-во комнаты, этаж, площадь, вид здания", BRIGHT_RED, YELLOW), }),
				BRIGHT_YELLOW);
			print("", 3);
			std::cin >> h;
			
			dyn.push_back(h);
		} else if (buf == "отобр") {
			println("Квартиры слева-направо: ", BRIGHT_YELLOW);
			for (auto h : dyn) {
				print("", 3);
				std::cout << h << '\n';
			}
		} else if (buf == "зап") {
			std::string name;
			print(swallow({"Введите название будущего ", SET_COLOUR("файла", BRIGHT_RED, YELLOW), SET_COLOUR("", WHITE, WHITE)}), YELLOW);
			std::cin >> name;
			
			std::ofstream out(name);
			Man::save_houses_in_file(out, dyn);
			println(swallow({"ФАЙЛ СОХРАНЁН ПОД НАЗВАНИЕ ", SET_COLOUR(name, BRIGHT_RED, BRIGHT_YELLOW), "!", SET_COLOUR("", BRIGHT_RED, WHITE)}), BRIGHT_YELLOW);
		} else if (buf == "выв") {
			std::string name;
			print(swallow({"Введите название читаемого ", SET_COLOUR("файла", BRIGHT_RED, YELLOW), ": ", SET_COLOUR("", BRIGHT_RED, WHITE)}), YELLOW);
			std::cin >> name;

			std::ifstream in(name);
			if (!in.is_open()) {
				err.code = 1;
			    err.msg = "введён несуществующий файл либо файл, который находится в директории выше.";
				return err;
			}
			dyn = Man::get_houses_from_file(in);
			println("ФАЙЛ ПРОЧИТАН", BRIGHT_RED);
		} else if (buf == "сорт") {
			dyn = Man::sort_houses_by_space(dyn);
			println("ДАННЫЕ СОРТИРОВАНЫ!", BRIGHT_RED);
		}
	}

	return err;
}

int main(void)
{
	Error err = interface();
	if (err.code > 0) {
		std::cout << "FATAL: " << err.msg << std::endl;
	}
	
	return 0;
}
