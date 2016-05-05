#include <iostream>
#include <stdint.h>
#include <numeric>
#include <cmath>

inline int32_t getValue(const char c) {
	if (c == '.') {
		return 0;
	} else if (c < 97) {
		return c - 64;
	} else {
		return 96 - c;
	}
}

struct InfiniteTiles {
	void read(std::istream &is) {
		is >> N >> M;
		std::string line_string;
		for (size_t line = 0; line < N; ++line) {
			is >> line_string;
			for (size_t row = 0; row < M; ++row) {
				data[line][row] = getValue(line_string[row]);
			}
		}
	}

	uint32_t maxRectangleSize() {
		for (size_t i = 0; i <= 2 * N; ++i) {
			sum[i][0] = 0;
		}

		for (size_t i = 0; i <= 2 * M; ++i) {
			sum[0][i] = 0;
		}

		cacheRectangleSum();

		uint32_t max = 0;
		for (size_t row = 0; row < N; ++row) {
			for (size_t col = 0; col < M; ++col) {
				auto max_here = maxRectangleVertexAt(row, col);
				if (std::isinf(max_here)) {
					return max_here;
				} else if (max_here > max) {
					max = max_here;
				}
			}
		}
		return max;
	}

	static const size_t MAX_ROWS = 1000, MAX_COLS = 1000;

	char data[MAX_ROWS][MAX_COLS];
	static int32_t sum[2 * MAX_ROWS + 1][2 * MAX_COLS + 1];
	size_t N, M;

private:
	void cacheRectangleSum() {
		size_t total_length = 2 * N + 2 * M;
		size_t doubleN = 2 * N, doubleM = 2 * M;
		size_t u, v, sizeJ;
		for (size_t i = 0; i < total_length; ++i) {
			if ( i < doubleN) {
				u = i + 1;
				v = 1;
			}
			else {
				u = doubleN;
				v = i - doubleN + 1;
			}

			for (; u > 0 && v <= doubleM; --u, ++v) {
				sum[u][v] = data[ciclicRow(u - 1)][ciclicCol(v - 1)] - sum[u - 1][v - 1]
										+ sum[u - 1][v] + sum[u][v - 1];
			}
		}
	}

	uint32_t maxRectangleVertexAt(size_t row, size_t col) {
		int32_t max = 0;
		for (size_t length_row = 1; length_row <= N; ++length_row) {
			for (size_t length_col = 1; length_col <= M; ++length_col) {
				int32_t sum_rectangle = sum[row + length_row][col + length_col]
						- sum[row][col + length_col] + sum[row][col]
						- sum[row + length_row][col];

				if (sum_rectangle > 0 && sum_rectangle > max) {
					max = sum_rectangle;
				}

				if (sum_rectangle > 0 && (length_col == M || length_row == N)) {
					throw std::overflow_error("Infinity");
				}
			}
		}

		return max;
	}

	inline size_t ciclicRow(int32_t row) {
		while (row < 0) {
			row += N;
		}

		if (row >= N) {
			row = row % N;
		}

		return row;
	}

	inline size_t ciclicCol(int32_t col) {
		while (col < 0) {
			col += M;
		}

		if (col >= M) {
			col = col % M;
		}
		return col;
	}
};

int32_t InfiniteTiles::sum[2 * MAX_ROWS + 1][2 * MAX_COLS + 1];

int main() {
	size_t num_cases = 0;

	InfiniteTiles tiles;
	std::cin >> num_cases;
	for (size_t num_case = 1; num_case <= num_cases; ++num_case) {
		tiles.read(std::cin);
		std::cout << "Case #" << num_case << ": ";
		try {
			std::cout << tiles.maxRectangleSize();
		} catch (std::overflow_error & e) {
			std::cout << "INFINITY";
		}
		std::cout << std::endl;
	}

}
