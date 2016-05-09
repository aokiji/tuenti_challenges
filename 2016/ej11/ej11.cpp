#include <iostream>
#include <stdint.h>

uint64_t secondsToToast(uint64_t N, uint64_t M, uint64_t K) {
	if (K < M * N || K % M != 0) {
		throw std::runtime_error("IMPOSSIBLE");
	}

	// its equivalent to finding the remaining with N=1
	K -= M * (N - 1);
	K /= M;

	if (K == 1) {
		return 0;
	}

	// create as many new piles as bits - 1 has K/M
	// and duplicate a pile untile max power of 2 for K/M
	size_t bits = 0, pow2 = 0, count = 0;
	while (K > 0) {
		if (K & 1) {
			bits++;
			pow2 = count;
		}
		K >>= 1;
		count++;
	}
	return bits + pow2 - 1;
}

int main() {
	size_t num_cases;
	std::cin >> num_cases;
	for (size_t num_case = 1; num_case <= num_cases; ++num_case ) {
		int64_t N, M, K;
		std::cin >> N >> M >> K;
		std::cout << "Case #" << num_case << ": ";
		int64_t secs;
		try {
			secs = secondsToToast(N, M, K);
			std::cout << secs;
		}
		catch(std::exception & e) {
			std::cout << e.what();
		}
		std::cout << "\n";
	}
}
