#include <iostream>

size_t numberOfTables(size_t num_diners) {
	if (num_diners == 0) {
		return 0;
	}
	else if (num_diners < 5) {
		return 1;
	}

	return (num_diners+1)/2 -1;
}

int main() {
	size_t num_cases, num_diners;
	std::cin >> num_cases;

	for (size_t num_case = 1; num_case <= num_cases; ++num_case) {
		std::cin >> num_diners;
		std::cout << "Case #" << num_case << ": " << numberOfTables(num_diners) << "\n";
	}
}
