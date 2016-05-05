#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/lambda/lambda.hpp>
#include <fstream>

static size_t iter = 0;

using boost::asio::ip::tcp;
using boost::lambda::var;

struct Labyrinth {
	enum MOVEMENT {
		UP = 0, DOWN, RIGHT, LEFT, NONE
	};

	char reverse_move[5] = { DOWN, UP, LEFT, RIGHT, NONE };

	Labyrinth() :
			visible_rows(7), visible_cols(7), defined_position(false) {
		init();
	}

	Labyrinth(const Labyrinth &) = delete;
	Labyrinth(Labyrinth &&) = delete;

	void init() {
		currx = MAXSIZE / 2;
		curry = MAXSIZE / 2;
		data.resize(MAXSIZE * MAXSIZE);
		visited.resize(MAXSIZE * MAXSIZE);
		row_rows.resize(MAXSIZE);
		visited_rows.resize(MAXSIZE);
		row_ptr = row_rows.data();
		visited_ptr = visited_rows.data();
		row_ptr[0] = data.data();
		visited_ptr[0] = visited.data();
		for (size_t i = 1; i < MAXSIZE; ++i) {
			row_ptr[i] = row_ptr[i - 1] + MAXSIZE;
			visited_ptr[i] = visited_ptr[i - 1] + MAXSIZE;
		}

		std::fill(visited.begin(), visited.end(), 0);
		std::fill(data.begin(), data.end(), ' ');
	}

	void refresh(const std::string labyrinth) {
		auto EOL = labyrinth.find('\n');
		if (EOL == std::string::npos) {
			throw std::runtime_error("wrong format");
		}
		auto count = std::count(labyrinth.begin(), labyrinth.end(), '\n');

		auto mypos = labyrinth.find('x');
		if (mypos == std::string::npos) {
			throw std::runtime_error("wrong format");
		}

		size_t myrow = mypos / (EOL + 1);
		size_t mycol = mypos % (EOL + 1);

		if (labyrinth.size() != (EOL + 1) * count) {
			std::cerr << labyrinth << std::endl;
			throw std::runtime_error("Wrong format");
		}

		visible_cols = EOL;
		visible_rows = count;

		auto row_beg = labyrinth.begin();
		visiblex = currx - myrow;
		visibley = curry - mycol;
		for (size_t i = 0; i < visible_rows; ++i) {
			auto row_end = row_beg + visible_cols;
			std::copy(row_beg, row_end, row_ptr[i + visiblex] + visibley);
			row_beg = ++row_end;
		}
	}

	bool won() {
		return currx == visiblex || currx == visiblex + visible_rows - 1
				|| curry == visibley || curry == visibley + visible_cols - 1;
	}

	bool solve() {
		visited_ptr[currx][curry] = 1;

		planned_movements.clear();
		int possible_move = possibleMove();
		if (possible_move != NONE) {
			planned_movements.push_back(possible_move);
		}

		return !planned_movements.empty();
	}

	int possibleMove() {
		if (curry > visibley && !visited_ptr[currx][curry - 1]
				&& row_ptr[currx][curry - 1] == ' ') {
			return LEFT;
		}
		if (currx < visiblex + visible_rows - 1
				&& !visited_ptr[currx + 1][curry]
				&& row_ptr[currx + 1][curry] == ' ') {
			return DOWN;
		}
		if (currx > visiblex && !visited_ptr[currx - 1][curry]
				&& row_ptr[currx - 1][curry] == ' ') {
			return UP;
		}
		if (curry < visibley + visible_cols - 1
				&& !visited_ptr[currx][curry + 1]
				&& row_ptr[currx][curry + 1] == ' ') {
			return RIGHT;
		}

		return NONE;
	}

	void move(int move) {
		row_ptr[currx][curry] = ' ';

		switch (move) {
		case UP:
			--currx;
			break;
		case DOWN:
			++currx;
			break;
		case LEFT:
			--curry;
			break;
		case RIGHT:
			++curry;
			break;
		default:
			throw std::runtime_error("Not an acceptable move");
			break;
		}

		row_ptr[currx][curry] = 'x';
	}

	int move() {
		int movement = planned_movements.front();
		planned_movements.pop_front();
		all_movements.push_back(movement);
		move(movement);
		return movement;
	}

	int undoMove() {
		int movement = all_movements.back();
		all_movements.pop_back();
		movement = reverse_move[movement];
		move(movement);
		return movement;
	}

	bool hasMoves() {
		return !planned_movements.empty();
	}

	void writeToFile(const std::string & name) {
		// findout real limits
		size_t minx = MAXSIZE, maxx = 0, miny = MAXSIZE, maxy = 0;
		for (size_t i = 0; i < MAXSIZE; ++i) {
			for (size_t j = 0; j < miny; ++j) {
				if (row_ptr[i][j] != ' ') {
					miny = j;
					break;
				}
			}

			for (size_t j = MAXSIZE - 1; j > maxy; --j) {
				if (row_ptr[i][j] != ' ') {
					maxy = j;
					break;
				}
			}
		}

		for (size_t j = 0; j < MAXSIZE; ++j) {
			for (size_t i = 0; i < minx; ++i) {
				if (row_ptr[i][j] != ' ') {
					minx = i;
					break;
				}
			}

			for (size_t i = MAXSIZE - 1; i > maxx; --i) {
				if (row_ptr[i][j] != ' ') {
					maxx = i;
					break;
				}
			}
		}

		std::ofstream file {name};
		for (size_t i = minx; i <= maxx; ++i) {
			for (size_t j = miny; j <= maxy; ++j) {
				file << row_ptr[i][j] << " ";
			}
			file << std::endl;
		}

	}

	static const int MAXSIZE = 300;
	size_t visible_rows, visible_cols;
	char ** row_ptr;
	std::vector<char *> row_rows;
	std::vector<char> data;
	char ** visited_ptr;
	std::vector<char *> visited_rows;
	std::vector<char> visited;
	bool defined_position;
	size_t currx, curry;
	size_t visiblex, visibley;
	std::deque<char> planned_movements;
	std::deque<char> all_movements;
};

namespace std {
std::ostream & operator<<(std::ostream & os, const Labyrinth & lab) {
	std::cout << lab.visible_rows << ", " << lab.visible_cols << "\n";
	for (size_t i = 0; i < lab.visible_rows; ++i) {
		for (size_t j = 0; j < lab.visible_cols; ++j) {
			char chr = lab.row_ptr[i + lab.visiblex][j + lab.visibley];
			if (chr == ' ' && lab.visited_ptr[i + lab.visiblex][j + lab.visibley]) {
				chr = 'v';
			}
			std::cout << chr;
		}
		std::cout << std::endl;
	}
	return os;
}
}

class Client {
public:
	Client(boost::asio::io_service& io_service,
			tcp::resolver::iterator endpoint_iterator) :
			io_service_(io_service), socket_(io_service) {
		boost::asio::connect(socket_, endpoint_iterator);
	}

	void readLabyrinth(Labyrinth & labyrinth) {
		labyrinth.refresh(read());
	}

	void writeLabyrinthBuffer(const Labyrinth & labyrinth, std::string & line) {
		for (size_t i = 0; i < labyrinth.visible_rows; ++i) {
			line.append(labyrinth.row_ptr[i],
					labyrinth.row_ptr[i] + labyrinth.visible_cols);
			line.append("\n");
		}
	}

	void run() {
		Labyrinth labyrinth;
		readLabyrinth(labyrinth);

		try {
			while (true) {

				while(labyrinth.hasMoves() || labyrinth.solve()) {
					auto movement = labyrinth.move();
					write_data(commands[movement]);
					readLabyrinth(labyrinth);
				}

				if (labyrinth.won()) {
					return;
				}

				// undocommands movements until new path found
				while (!labyrinth.solve() && !labyrinth.all_movements.empty()) {
					auto movement = labyrinth.undoMove();
					write_data(commands[movement]);
					readLabyrinth(labyrinth);
				}
				if (!labyrinth.hasMoves() && labyrinth.all_movements.empty()) {
					labyrinth.writeToFile("labyrinth");
					break;
				}
			}
		} catch (...) {
			std::ofstream file { "movimientos" };
			for (auto & move : labyrinth.all_movements) {
				file << commands[move] << "\n";
			}
			file << std::endl;
			throw;
		}
	}

private:
	std::string read() {
		size_t length = socket_.read_some(boost::asio::buffer(io_buffer));
		std::string line(std::begin(io_buffer), std::begin(io_buffer) + length);

		return line;
	}

	void write_data(const std::string& data) {
		size_t length = socket_.write_some(boost::asio::buffer(data));
		if (length != data.size()) {
			throw std::runtime_error("Message sent incomplete");
		}
		std::cout << ++iter << " sent " << data;
	}

	std::vector<std::string> commands = { "u\n", "d\n", "r\n", "l\n" };
	boost::asio::io_service& io_service_;
	tcp::socket socket_;
	static char io_buffer[10000];
};

char Client::io_buffer[10000];

int main() {
	try {

		boost::asio::io_service io_service;

		tcp::resolver resolver(io_service);
		auto endpoint_iterator = resolver.resolve( { "52.49.91.111", "1986" });

		Client client(io_service, endpoint_iterator);

		client.run();

	} catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
