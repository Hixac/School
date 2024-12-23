#include <cassert>
#include <iostream>

#include <transport_company.hpp>
#include <transport_stream.hpp>

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
    println(swallow({"создаёт новый экземпляр ", SET_COLOUR("Грузового Транспорта", BRIGHT_RED, YELLOW), " в динамическую память."}), 5, YELLOW);
	println("отобр", 1, BRIGHT_RED);
    println(swallow({"отобразить ", SET_COLOUR("Грузовой Транспорт, помещённые в динамическую память", BRIGHT_RED, YELLOW), "."}), 5, YELLOW);
	println("принт", 1, BRIGHT_RED);
    println(swallow({"вывести ", SET_COLOUR("по указанному диапазону Грузовой Транспорт, помещённый в динамическую память,", BRIGHT_RED, YELLOW), " по площади."}), 5, YELLOW);
	println("посч", 1, BRIGHT_RED);
    println(swallow({"вывести ", SET_COLOUR("количество уникальных пунктов отправления и назначения", BRIGHT_RED, YELLOW), "."}), 5, YELLOW);
	println("зап", 1, BRIGHT_RED);
    println(swallow({"записать данные с ", SET_COLOUR("динамической памяти в Файл", BRIGHT_RED, YELLOW), "."}), 5, YELLOW);
	println("выв", 1, BRIGHT_RED);
    println(swallow({"вывести данные с ", SET_COLOUR("Файла в динамическую память", BRIGHT_RED, YELLOW), "."}), 5, YELLOW);		
	println("вых", 1, BRIGHT_RED);
    println("выйти из программы", 5, YELLOW);
}

void Create(Tran::TransportCompany& tc)
{
	std::cout << "1) Грузовик или 2) Корабль. (1/2/отм) ";
	std::string buf; std::cin >> buf;
	if (buf == "1") {
		Tran::Lorry l;
		println(swallow({"Введите следующие параметры соответственно:  ",
					SET_COLOUR("имя, пункт отправления, пункт назначения, вместимость, кол-во колёс", BRIGHT_RED, YELLOW), }),
			BRIGHT_YELLOW);
		print("", 3);
		std::cin >> l;
		tc.CreateInstanceOfLorry(l);
	} else if (buf == "2") {
		Tran::Ship s;
		println(swallow({"Введите следующие параметры соответственно:  ",
					SET_COLOUR("имя, пункт отправления, пункт назначения, вместимость, кол-во контейнеров", BRIGHT_RED, YELLOW), }),
			BRIGHT_YELLOW);
		print("", 3);
		std::cin >> s;
		tc.CreateInstanceOfShip(s);
	} else {
		println("Отменено");
	}
}

void Show(Tran::TransportCompany& tc)
{
	tc.PrintInfo();
}

void WriteFile(Tran::TransportCompany& tc)
{
	std::string name;
	print(swallow({"Введите название будущего ", SET_COLOUR("файла", BRIGHT_RED, YELLOW), SET_COLOUR(": ", WHITE, WHITE)}), YELLOW);
	std::cin >> name;
			
	std::ofstream out(name);
	tc.WriteToFile(out);
	println(swallow({"ФАЙЛ СОХРАНЁН ПОД НАЗВАНИЕМ ", SET_COLOUR(name, BRIGHT_RED, BRIGHT_YELLOW), "!", SET_COLOUR("", BRIGHT_RED, WHITE)}), BRIGHT_YELLOW);
	out.close();
}

void ReadFile(Tran::TransportCompany& tc)
{
	std::string name;
	print(swallow({"Введите название читаемого ", SET_COLOUR("файла", BRIGHT_RED, YELLOW), ": ", SET_COLOUR("", BRIGHT_RED, WHITE)}), YELLOW);
	std::cin >> name;

	std::ifstream in(name);
	if (!in.is_open()) {
		assert(false && "введён несуществующий файл либо файл, который находится в директории выше.");
	}
	tc.ReadFromFile(in);
	println("ФАЙЛ ПРОЧИТАН", BRIGHT_RED);
	in.close();
}

void Sort(Tran::TransportCompany& tc)
{
	size_t min, max;
	print(swallow({"Введите ", SET_COLOUR("минимум", BRIGHT_RED, YELLOW), " и ", SET_COLOUR("максимум", BRIGHT_RED, YELLOW)}), YELLOW);
	print(" ");
	std::cin >> min >> max;
	tc.PrintSortByCap(min, max);
}

Error interface()
{
	Error err { 0, "" };
	
	Tran::TransportCompany tc;
	std::string buf;

	while (buf != "вых") {
		print(">> ");
		std::cin >> buf;

		if (buf == "help") {
			print_help();
		} else if (buf == "созд") {
		    Create(tc);
		} else if (buf == "отобр") {
			Show(tc);
		} else if (buf == "зап") {
			WriteFile(tc);
		} else if (buf == "выв") {
			ReadFile(tc);
		} else if (buf == "принт") {
			Sort(tc);
		} else if (buf == "посч") {
			tc.PrintCountDepDest();
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
