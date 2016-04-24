#include <stdint.h>
#include <iostream>
#include <vector>
#include <limits>
#include <map>
#include <set>
#include <deque>

enum {
	A = 0, B, C, D, E
};

template<typename D>
class Matrix {
public:
	Matrix(int32_t N, int32_t M, D d) :
			data(N * M, d), N(N), M(M) {

	}

	typename std::vector<D>::reference get(int32_t n, int32_t m) {
		return data[n * N + m];
	}

	typename std::vector<D>::const_reference get(int32_t n, int32_t m) const {
		return data[n * N + m];
	}

private:
	std::vector<D> data;
public:
	int32_t N, M;
};

bool Connection(size_t girl, size_t second_girl, const Matrix<bool> & answers,
		const std::map<size_t, std::set<size_t>> & friendships) {
	static std::vector<bool> visited;
	if (girl == second_girl) {
		return true;
	}

	visited.resize(answers.N);
	std::fill(visited.begin(), visited.end(), false);

	// find if there is a path such that
	std::deque<int32_t> to_visit;
	to_visit.push_back(girl);
	while (!to_visit.empty()) {
		size_t visit_girl = to_visit.front();
		to_visit.pop_front();
		if (visit_girl == second_girl) {
			return true;
		}

		// if already visited move to next
		if (visited[visit_girl]) {
			continue;
		}

		visited[visit_girl] = true;

		// put this girls friends in queue
		auto it_girls_friends = friendships.find(visit_girl);
		if (it_girls_friends != friendships.end()) {
			const auto & girl_friends = it_girls_friends->second;
			to_visit.insert(to_visit.end(), girl_friends.begin(),
					girl_friends.end());
		}
	}

	return visited[second_girl];
}

size_t betterMatch(const Matrix<bool> & answers,
		const std::map<size_t, std::set<size_t>> & friendships) {
	size_t better_score = 0;
	for (size_t girl = 0; girl < answers.N; ++girl) {
		size_t score = 0;
		// 7 points if G likes naughty, dirty games
		if (answers.get(girl, A)) {
			score += 7;
		}

		// 5 points for every girl H who likes to go shopping and has no possible connection with G through a chain of friends (friends, friends of friends, friends of friends of friends, etc.)
		for (size_t second_girl = 0; second_girl < answers.N; ++second_girl) {
			if (answers.get(second_girl, E)
					&& !Connection(girl, second_girl, answers, friendships)) {
				score += 5;
			}
		}

		auto it_girl_friends = friendships.find(girl);
		if (it_girl_friends != friendships.end()) {
			for (auto & girl_friend : it_girl_friends->second) {
				const auto & girl_friends = it_girl_friends->second;
				// 3 points for every friend of G who likes super hero action figures.
				if (answers.get(girl_friend, B)) {
					score += 3;
				}

				// 6 points for every friend of a friend of G, not including the friends of G and G herself, who likes men in leisure suits
				for (auto & girl_friend_of_a_friend : friendships.at(
						girl_friend)) {
					if (girl_friend_of_a_friend != girl
							&& answers.get(girl_friend_of_a_friend, C)
							&& girl_friends.find(girl_friend_of_a_friend)
									== girl_friends.end()) {
						score += 6;
					}
				}

				// 4 points if G has any friend H who likes cats, and no friend of H (except perhaps G) likes cats (4 points at most, not 4 for every friend).
				if (answers.get(girl_friend, D)) {
					bool cats = false;
					for (auto & girl_friend_of_a_friend : friendships.at(
							girl_friend)) {
						if (girl_friend_of_a_friend != girl
								&& answers.get(girl_friend_of_a_friend, D)) {
							cats = true;
							break;
						}
					}
					if (!cats) {
						score += 4;
					}
				}
			}
		}

		if (score > better_score) {
			better_score = score;
		}
	}

	return better_score;
}

int main() {
	int32_t M, N;
	const size_t num_questions = 5;

	std::cin >> N >> M;
	Matrix<bool> answers { N, num_questions, false };
	std::map<std::string, size_t> girls;
	std::map<size_t, std::set<size_t>> friends;

// read input
	for (auto line = 0; line < N; ++line) {
		char answer;
		std::string girl;
		std::cin >> girl;
		girls[girl] = line;
		for (size_t i = 0; i < num_questions; ++i) {
			std::cin >> answer;
			if (answer == 'Y') {
				answers.get(line, i) = true;
			}
		}
	}

	std::cin.ignore();
	while (M--) {
		std::vector<size_t> girls_in_line;
		std::string line;
		std::getline(std::cin, line);
		int pos = -1;
		while (true) {
			auto prev = ++pos;
			pos = line.find(' ', prev);
			if (pos == std::string::npos) {
				girls_in_line.push_back(
						girls[std::string(line, prev, line.length() - prev)]);
				break;
			}
			girls_in_line.push_back(girls[std::string(line, prev, pos - prev)]);
		}

		for (size_t i = 0; i < girls_in_line.size(); ++i) {
			for (size_t j = i; j < girls_in_line.size(); ++j) {
				if (i != j) {
					friends[girls_in_line[i]].insert(girls_in_line[j]);
					friends[girls_in_line[j]].insert(girls_in_line[i]);
				}
			}
		}
	}

// find better option
	std::cout << betterMatch(answers, friends) << std::endl;
}
