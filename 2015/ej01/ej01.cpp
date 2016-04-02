#include <iostream>

int main() {
	size_t cases;
	std::cin >> cases;
	while (cases--) {
		size_t urinals;
		std::cin >> urinals;
		urinals = (urinals + 1) >> 1;
		std::cout << urinals << "\n";
	}
	return 0;
}
