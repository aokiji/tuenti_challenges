#include <iostream>
#include <algorithm>
#include <vector>

std::vector<double> find_primes(size_t max) {
	std::vector<bool> primes(max, true);
	for (size_t i = 2; i < max / 2 + 1; ++i) {
		if (primes[i]) {
			size_t j = i * 2;
			for (size_t k = 3; j < max; ++k) {
				primes[j] = false;
				j = i * k;
			}
		}
	}

	std::vector<double> primes_vector;
	primes_vector.reserve(std::count(primes.begin(), primes.end(), true));
	for (size_t i = 2; i < max; ++i) {
		if (primes[i]) {
			primes_vector.push_back(i);
		}
	}
	return std::move(primes_vector);
}

size_t count_almost_primes(size_t min, size_t max,
		std::vector<double> & primes) {
	std::vector<bool> almost_primes(max + 1, false);
	for (size_t prime1_i = 0; prime1_i < primes.size(); ++prime1_i) {
		for (size_t prime2_i = prime1_i; prime2_i < primes.size(); ++prime2_i) {
			auto & prime1 = primes[prime1_i];
			auto & prime2 = primes[prime2_i];
			size_t almost_prime = prime1 * prime2;
			if (almost_prime > max)
				break;
			almost_primes[prime1 * prime2] = true;
		}
	}

	return std::count(almost_primes.begin() + min, almost_primes.end(), true);
}

int main() {
	size_t T;
	std::cin >> T;
	std::vector<std::pair<size_t, size_t>> pairsAB;
	pairsAB.reserve(T);
	while (T--) {
		size_t A, B;
		(std::cin >> A).ignore() >> B;
		pairsAB.push_back(std::make_pair(A, B));
	}

	auto max_pair =
			std::max_element(pairsAB.begin(), pairsAB.end(),
					[] (std::pair<size_t, size_t> & par1, std::pair<size_t, size_t> & par2) {
						return par1.second < par2.second;
					});
	auto primes = find_primes(max_pair->second / 2 + 1);
	for (auto & pairAB : pairsAB) {
		std::cout << count_almost_primes(pairAB.first, pairAB.second, primes)
				<< "\n";
	}

	return 0;
}
