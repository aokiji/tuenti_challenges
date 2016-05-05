#include <iostream>
#include <stdint.h>
#include <deque>
#include <limits>
#include <array>
#include <vector>

struct ZeroOne {
	size_t zeroes, ones;

	bool operator<(const ZeroOne & zo) {
		if (zeroes + ones == zo.zeroes + zo.ones) {
			return ones < zo.ones;
		}
		return zeroes + ones < zo.zeroes + zo.ones;
	}
};

struct ZeroOneImmiscibleState {
	uint64_t number;
	size_t overflow;
	ZeroOne result;
};

struct SingleDigitProduct {
	uint64_t product;
	size_t last_digit;
};

std::array<SingleDigitProduct, 10> calculateSingleDigitProduct(
		uint32_t number) {
	std::array<SingleDigitProduct, 10> products;
	for (size_t i = 0; i < 10; i++) {
		uint64_t product = number * i;
		products[i] = SingleDigitProduct { product, product % 10 };

	}

	return products;
}

std::vector<uint64_t> candidatesToSum(uint64_t value, size_t expected_digit,
		const std::array<SingleDigitProduct, 10> & product_cache) {
	std::vector<uint64_t> candidates;
	candidates.reserve(10);

	size_t value_last_digit = value % 10;
	for (auto & prod_cache : product_cache) {
		size_t digitsum = prod_cache.last_digit + value_last_digit;
		if ((digitsum % 10) == expected_digit) {
			candidates.push_back(prod_cache.product);
		}
	}

	return candidates;
}

ZeroOne zero_one_immiscible(uint32_t number) {
	std::deque<ZeroOneImmiscibleState> queue;

	size_t zeroes = 0;
	for (const auto & divisor : {10, 5, 2}) {
		while (number % divisor == 0) {
			number/= divisor;
			zeroes++;
		}
	}

	queue.push_back(ZeroOneImmiscibleState { 0, 0, ZeroOne { zeroes, 0} });

	auto sd_product = calculateSingleDigitProduct(number);

	ZeroOne min { std::numeric_limits<size_t>::max(), 0 };
	while (queue.size() > 0) {
		auto state = queue.front();
		queue.pop_front();

		if (min < state.result) {
			continue;
		}

		auto candidates = candidatesToSum(state.number, 1, sd_product);
		for (auto candidate : candidates) {
			if (candidate == 0 && state.result.zeroes == 0 && state.result.ones == 0) {
				continue;
			}

			auto new_state = state;
			new_state.result.ones++;

			new_state.number += candidate;
			new_state.number /= 10;

			if (new_state.number == 0 && new_state.result.ones > 0) {
				if (new_state.result < min) {
					min = new_state.result;
				}
				continue;
			}

			queue.push_back(new_state);
		}
	}

	return min;
}

int main() {
	size_t num_cases;
	std::cin >> num_cases;

	for (size_t num_case = 1; num_case <= num_cases; num_case++) {
		uint32_t num;
		std::cin >> num;
		auto zero_ones = zero_one_immiscible(num);
		std::cout << "Case #" << num_case << ": " << zero_ones.ones << " "
				<< zero_ones.zeroes << "\n";
	}
}
