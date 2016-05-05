#include <iostream>
#include <map>
#include <vector>
#include <initializer_list>
#include <deque>

struct Move {
	static const std::map<std::string, size_t> moves_id;
};

const std::map<std::string, size_t> Move::moves_id( { { "R", 1 }, { "RU", 2 }, {
		"RD", 3 }, { "D", 4 }, { "U", 5 }, { "L", 6 }, { "LU", 7 }, { "LD", 8 },
		{ "K", 9 }, { "P", 10 } });

template<typename InputIterator, typename OutputIterator>
void translateMoves(InputIterator begin, InputIterator end,
		OutputIterator out) {
	for (; begin != end; ++out, ++begin) {
		*out = Move::moves_id.find(*begin)->second;
	}
}

struct Combo {
	Combo(const std::vector<std::string> & moves) {
		id_moves.reserve(moves.size());
		translateMoves(moves.begin(), moves.end(),
				std::back_inserter(id_moves));
	}

	Combo(const std::initializer_list<std::string> & moves) :
			Combo(std::vector<std::string>(moves)) {

	}

	static size_t almostCombo(const std::vector<size_t> & moves, size_t & pos) {
		auto beg = moves.data() + pos;
		if (moves.size() > pos + 4) {
			if (beg[0] == 1) {
				// possible R-RD-D-LD-L-K
				if (beg[1] == 3 && beg[2] == 4 && beg[3] == 8 && beg[4] == 6) {
					if (moves.size() == pos + 5 || beg[5] != 9) {
						++pos;
						return 1;
					}
				}
			} else if (beg[0] == 6) {
				// possible L-LD-D-RD-R-P
				if (beg[1] == 8 && beg[2] == 4 && beg[3] == 3 && beg[4] == 1) {
					if (moves.size() == pos + 5 || beg[5] != 10) {
						++pos;
						return 1;
					}
				}
			}
		}

		if (moves.size() - pos > 2) {
			if (beg[0] == 1) {
				// possible R-D-RD-P
				if (beg[1] == 4 && beg[2] == 3) {
					if (moves.size() == pos + 3 || beg[3] != 10) {
						++pos;
						return 1;
					}
				}
			} else if (beg[0] == 4) {
				// possible D-RD-R-P
				if (beg[1] == 3 && beg[2] == 1) {
					if (moves.size() == pos + 3 || beg[3] != 10) {
						++pos;
						if (pos > 2 && beg[-1] == 8 && beg[-2] == 6) {
							// ryu was trying other combo
							return 0;
						}
						return 1;
					}
				} else if (beg[1] == 8 && beg[2] == 6) {
					// possible D-LD-L-K
					if (moves.size() == pos + 3 || beg[3] != 9) {
						++pos;
						if (pos > 2 && beg[-1] == 3 && beg[-2] == 1) {
							// ryu was trying other combo
							return 0;
						}
						return 1;
					}
				}
			}
		}

		++pos;
		return 0;
	}

	static size_t almostCombos(const std::vector<size_t> moves) {
		size_t total = 0;
		for (size_t pos = 0; pos < moves.size();) {
			total += almostCombo(moves, pos);
		}
		return total;
	}

	std::vector<size_t> id_moves;
	static const std::vector<Combo> combo_moves;
};

const std::vector<Combo> Combo::combo_moves = { Combo( { "L", "LD", "D", "RD",
		"R", "P" }), Combo( { "D", "RD", "R", "P" }), Combo( { "R", "D", "RD",
		"P" }), Combo( { "D", "LD", "L", "K" }), Combo( { "R", "RD", "D", "LD",
		"L", "K" }), };

int main() {
	size_t num_cases;
	(std::cin >> num_cases).ignore();
	std::string line;
	for (size_t num_case = 1; num_case <= num_cases; ++num_case) {
		std::getline(std::cin, line);
		std::vector<size_t> moves;
		auto next = line.find('-');
		auto pos = 0;
		while (next != std::string::npos) {
			std::string move(line, pos, next - pos);
			moves.push_back(Move::moves_id.find(move)->second);
			pos = next + 1;
			next = line.find('-', pos);
		}
		std::string move(line, pos, line.size() - pos);
		moves.push_back(Move::moves_id.find(move)->second);

		std::cout << "Case #" << num_case << ": " << Combo::almostCombos(moves)
				<< std::endl;
	}
}
