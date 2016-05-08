#include <png.hpp>
#include <deque>
#include <stdint.h>
#include <functional>
#include <map>
#include <algorithm>

struct PietInterpreter {
	enum DIRECTION {
		LEFT, RIGHT, UP, DOWN, NONE
	};

	enum STATUS {
		OK, ERROR, THROUGH_WHITE
	};

	static const int32_t WHITE = 0xFFFFFF;
	static const int32_t BLACK = 0x000000;

	PietInterpreter() :
			DP { RIGHT }, CC { LEFT }, current_row { 0 }, current_col { 0 } {

	}

	void run(const png::image<png::rgb_pixel> & input) {
		this->input = input;
		while (1) {
			int tries = 8;
			while (tries--) {
				auto edge = findEdge(current_row, current_col, DP, CC);
				size_t newrow = edge.first, newcol = edge.second;
				auto st = nextColorBlock(newrow, newcol, DP);
				if (st == OK) {
					// execute command!!
					std::swap(current_row, newrow);
					std::swap(current_col, newcol);
					executeCommand(newrow, newcol, current_row, current_col);
					break;
				}
				else if (st == THROUGH_WHITE) {
					std::swap(current_row, newrow);
					std::swap(current_col, newcol);
					break;
				}
				else {
					// change directions
					if (tries % 2 == 1) {
						CC = CC == LEFT ? RIGHT : LEFT;
					}
					else {
						rotateDP(DP);
					}
				}
			}

			if (tries < 0) {
				break;
			}
		}
	}

	STATUS nextColorBlock(size_t & row, size_t & col, DIRECTION DP) {
		bool white = false;
		while (move(row, col, DP) == OK) {
			auto newcolor = getColor(row, col);
			if (newcolor == BLACK) {
				break;
			} else if (newcolor != WHITE) {
				return white ? THROUGH_WHITE : OK;
			}
			else if (newcolor == WHITE) {
				white = true;
			}
		}

		if (white) {
			throw std::logic_error("Not implemented");
		}

		return ERROR;
	}

	void visitColorBlock(size_t row, size_t col,
			std::function<void(size_t, size_t)> visit_function =
					[] (size_t, size_t) {return;}) {
		std::map<std::pair<size_t, size_t>, size_t> visited;
		visit_function(row, col);
		visited[std::make_pair(row, col)] = 1;
		std::vector<DIRECTION> try_moves = { LEFT, RIGHT, UP, DOWN };
		std::vector<DIRECTION> moves;
		while (1) {
			auto new_direction = unvisitedDirection(row, col, visited);
			if (new_direction != NONE) {
				move(row, col, new_direction);
				visited[std::make_pair(row, col)] = 1;
				visit_function(row, col);
				moves.push_back(new_direction);
			} else if (!moves.empty()) {
				// undo last movement
				auto prev_direction = moves.back();
				moves.pop_back();
				move(row, col, reverse(prev_direction));
			}
			else {
				break;
			}
		}
	}

	std::pair<size_t, size_t> findEdge(size_t row, size_t col,
			DIRECTION direction, DIRECTION codel_chosen) {
		std::function<bool(size_t, size_t)> rowCompare = std::less<size_t>(),
				colCompare = std::less<size_t>();
		std::pair<size_t, size_t> edge = { row, col };

		if (direction == LEFT) {
			colCompare = std::greater<size_t>();
			if (codel_chosen == RIGHT) {
				rowCompare = std::greater<size_t>();
			}
		} else if (direction == RIGHT && codel_chosen == LEFT) {
			rowCompare = std::greater<size_t>();
		} else if (direction == DOWN && codel_chosen == RIGHT) {
			colCompare = std::greater<size_t>();
		} else if (direction == UP) {
			rowCompare = std::greater<size_t>();
			if (codel_chosen == LEFT) {
				colCompare = std::greater<size_t>();
			}
		}

		visitColorBlock(row, col,
				[&] (size_t vrow, size_t vcol) {
					if (isLess(edge.first, edge.second, vrow, vcol, rowCompare, colCompare)) {
						edge.first = vrow;
						edge.second = vcol;
					}
				});
		return edge;
	}

	int32_t getColor(size_t row, size_t col) {
		auto rgb = input[row][col];
		return (rgb.red << 16) | (rgb.green << 8) | rgb.blue;
	}

	int32_t getColor() {
		return getColor(current_row, current_col);
	}

	int32_t getInteger(size_t row, size_t col) {
		int32_t count = 0;
		visitColorBlock(row, col,
				[&] (size_t vrow, size_t vcol) {
					++count;
				});
		return count;
	}

	STATUS move(size_t &current_row, size_t &current_col, DIRECTION direction) {
		switch (direction) {
		case LEFT:
			if (current_col == 0) {
				return ERROR;
			}
			--current_col;
			break;
		case RIGHT:
			if (current_col == input.get_width() - 1) {
				return ERROR;
			}
			++current_col;
			break;
		case UP:
			if (current_row == 0) {
				return ERROR;
			}
			--current_row;
			break;
		case DOWN:
			if (current_row == input.get_height() - 1) {
				return ERROR;
			}
			++current_row;
			break;
		default:
			throw std::logic_error("Unknown direction");
			break;
		}

		return OK;
	}

	STATUS move(DIRECTION direction) {
		return move(current_row, current_col, direction);
	}

	DIRECTION reverse(DIRECTION direction) {
		switch (direction) {
		case LEFT:
			return RIGHT;
			break;
		case RIGHT:
			return LEFT;
			break;
		case UP:
			return DOWN;
			break;
		case DOWN:
			return UP;
			break;
		default:
			throw std::logic_error("Unexpected direction to reverse");
		}
	}

private:

	DIRECTION unvisitedDirection(size_t row, size_t col,
			const std::map<std::pair<size_t, size_t>, size_t> & visited) {
		static std::vector<DIRECTION> try_moves = { LEFT, RIGHT, UP, DOWN };
		auto current_row = row;
		auto current_col = col;
		auto color = getColor(row, col);
		for (auto & direction : try_moves) {
			row = current_row;
			col = current_col;
			if (move(row, col, direction) == OK) {
				if (color == getColor(row, col)
						&& visited.find(std::make_pair(row, col))
								== visited.end()) {
					return direction;
				}
			}
		}
		return NONE;
	}

	bool isLess(size_t row1, size_t col1, size_t row2, size_t col2,
			std::function<bool(size_t, size_t)> comp1,
			std::function<bool(size_t, size_t)> comp2) {
		if (comp1(row1, row2)) {
			return true;
		} else if (!comp1(row2, row1)) {
			if (comp2(col1, col2)) {
				return true;
			}
		}

		return false;
	}

	void rotateDP(DIRECTION & DP) {
		if (DP == RIGHT) {
			DP = DOWN;
		}
		else if (DP == DOWN) {
			DP = LEFT;
		}
		else if (DP == LEFT) {
			DP = UP;
		}
		else {
			DP = RIGHT;
		}
	}

	typedef void (PietInterpreter::*COMMAND) (size_t, size_t, size_t, size_t);

	void executeCommand(size_t prevrow, size_t prevcol, size_t newrow, size_t newcol) {
		auto prevcolor = getColor(prevrow, prevcol);
		auto newcolor = getColor(newrow, newcol);
		auto prev_color_pos = colorPosition(prevcolor);
		auto new_color_pos = colorPosition(newcolor);

		static COMMAND commands[6][3] = {
				{&PietInterpreter::none, &PietInterpreter::push, &PietInterpreter::pop},
				{&PietInterpreter::add, &PietInterpreter::substract, &PietInterpreter::multiply},
				{&PietInterpreter::divide, &PietInterpreter::mod, &PietInterpreter::negate},
				{&PietInterpreter::greater, &PietInterpreter::pointer, &PietInterpreter::switchCC},
				{&PietInterpreter::duplicate, &PietInterpreter::roll, &PietInterpreter::inNumber},
				{&PietInterpreter::inChar, &PietInterpreter::outNumber, &PietInterpreter::outChar}
		};

		auto dark_diff = (new_color_pos.first - prev_color_pos.first + 3) % 3;
		auto hue_diff =  (new_color_pos.second - prev_color_pos.second + 6) % 6;
		auto command = commands[hue_diff][dark_diff];
		(this->*(command))(prevrow, prevcol, newrow, newcol);
	}

	std::pair<int, int> colorPosition(int32_t color) {
		static int32_t colors[] = {
				0xFFC0C0, 0xFFFFC0, 0xC0FFC0, 0xC0FFFF, 0xC0C0FF, 0xFFC0FF,
				0xFF0000, 0xFFFF00, 0x00FF00, 0x00FFFF, 0x0000FF, 0xFF00FF,
				0xC00000, 0xC0C000, 0x00C000, 0x00C0C0, 0x0000C0, 0xC000C0
		};

		auto it = std::find(std::begin(colors), std::end(colors), color);
		if (it == std::end(colors)) {
			throw std::runtime_error("Unknown color");
		}
		auto color1_pos = std::distance(std::begin(colors), it);

		return std::make_pair(color1_pos / 6, color1_pos % 6);
	}

	// COMMANDS
	void none(size_t, size_t, size_t, size_t) {
		return;
	}

	void push(size_t row0, size_t col0, size_t row1, size_t col1) {
		auto value = getInteger(row0, col0);
		stack.push_back(value);
	}

	void pop(size_t, size_t, size_t, size_t) {
		stack.pop_back();
	}

	void add(size_t, size_t, size_t, size_t) {
		if (stack.size() < 2) {
			throw std::runtime_error("2 values needed in the stack");
		}
		auto val1 = stack.back();
		stack.pop_back();
		stack.back() += val1;
	}

	void substract(size_t, size_t, size_t, size_t) {
		if (stack.size() < 2) {
			throw std::runtime_error("2 values needed in the stack");
		}
		auto val1 = stack.back();
		stack.pop_back();
		stack.back() -= val1;
	}

	void multiply(size_t, size_t, size_t, size_t) {
		if (stack.size() < 2) {
			throw std::runtime_error("2 values needed in the stack");
		}
		auto val1 = stack.back();
		stack.pop_back();
		stack.back() *= val1;
	}

	void divide(size_t, size_t, size_t, size_t) {
		if (stack.size() < 2) {
			throw std::runtime_error("2 values needed in the stack");
		}
		auto val1 = stack.back();
		stack.pop_back();
		stack.back() /= val1;
	}

	void mod(size_t, size_t, size_t, size_t) {
		if (stack.size() < 2) {
			throw std::runtime_error("2 values needed in the stack");
		}
		auto val1 = stack.back();
		stack.pop_back();
		stack.back() = stack.back() % val1;
	}

	void negate(size_t, size_t, size_t, size_t) {
		if (stack.size() < 1) {
			throw std::runtime_error("1 value needed in the stack");
		}

		stack.back() = stack.back() != 0 ? 0 : 1;
	}

	void greater(size_t, size_t, size_t, size_t) {
		if (stack.size() < 2) {
			throw std::runtime_error("2 values needed in the stack");
		}
		auto val1 = stack.back();
		stack.pop_back();
		stack.back() = stack.back() > val1 ? 1 : 0;
	}

	void pointer(size_t, size_t, size_t, size_t) {
		if (stack.size() < 1) {
			throw std::runtime_error("1 value needed in the stack");
		}
		auto val1 = (stack.back() + 4) % 4;
		stack.pop_back();

		for (size_t i = 0; i < val1; ++i) {
			rotateDP(DP);
		}
	}

	void switchCC(size_t, size_t, size_t, size_t) {
		if (stack.size() < 1) {
			throw std::runtime_error("1 value needed in the stack");
		}
		auto val1 = std::abs(stack.back());
		stack.pop_back();

		if (val1 % 2 == 1) {
			CC = CC == LEFT ? RIGHT : LEFT;
		}
	}

	void duplicate(size_t, size_t, size_t, size_t) {
		if (stack.size() < 1) {
			throw std::runtime_error("1 value needed in the stack");
		}

		stack.push_back(stack.back());
	}

	void roll(size_t, size_t, size_t, size_t) {
		throw std::runtime_error("roll not implemented");
	}

	void inNumber(size_t, size_t, size_t, size_t) {
		int32_t value;
		std::cin >> value;
		stack.push_back(value);
	}

	void inChar(size_t, size_t, size_t, size_t) {
		char value;
		std::cin.read(&value, 1);
		stack.push_back(value);
	}

	void outNumber(size_t, size_t, size_t, size_t) {
		std::cout << stack.back();
		stack.pop_back();
	}

	void outChar(size_t, size_t, size_t, size_t) {
		char value = stack.back();
		std::cout.write(&value, 1);
		stack.pop_back();
	}

	DIRECTION DP;
	DIRECTION CC;
	std::deque<int> stack;
	size_t current_row, current_col;
	png::image<png::rgb_pixel> input;
};

png::image<png::rgb_pixel> readMirrorImage(const std::string filename) {
	png::image<png::rgb_pixel> image(filename);
	for (size_t y = 0; y < image.get_height(); ++y) {
		for (size_t x = 0, u = image.get_width() - 1; x < image.get_width() / 2;
				++x, --u) {
			std::swap(image[y][x], image[y][u]);
		}
	}
	return image;
}

int main() {

	auto input = readMirrorImage("alice_shocked.png");

	PietInterpreter piet;
	piet.run(input);
}

