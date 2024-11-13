#include <fgen.hpp>

int main()
{
	// Прямое слияние
	{
		My::String in_out = "txt.txt";
		My::String f1 = "txt1.txt";
		My::String f2 = "txt2.txt";

	    My::generate_file(in_out);
		My::generate_file(f1);
		My::generate_file(f2);
	
		My::FileOut out_file;
	}
}
