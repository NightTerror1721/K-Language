#include "data.h"

int main(int argc, char** argv)
{
	k::mem::Heap heap;

	k::data::Value a = heap.create_array(10);

	a.array()[0] = 10;
	a.array()[1] = heap.create_string("ola k ase");

	std::cout << a.array()[0].integer() << std::endl;
	std::cout << a.array()[1].string() << std::endl;

	return 0;
}
