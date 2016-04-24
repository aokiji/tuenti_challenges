#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <deque>

std::vector<std::string> split(const std::string & str) {
	std::vector<std::string> tokens;
	std::istringstream iss(str);
	std::copy(std::istream_iterator<std::string>(iss),
			std::istream_iterator<std::string>(), std::back_inserter(tokens));
	return std::move(tokens);
}

struct Element {
	std::string name;
	size_t id;
	size_t gold;
	std::vector<std::string> composition;
	std::vector<size_t> composition_ids;
};

struct Book {

	void insertElement(std::string element, size_t gold,
			std::vector<std::string> composition) {
		size_t id = elements.size();
		elements.push_back(
				Element { element, id, gold, std::move(composition) });
		elements_id.insert(std::make_pair(element, id));
	}

	const Element & get(const std::string & name) const {
		return elements[elements_id.find(name)->second];
	}

	Element & get(const std::string & name) {
		return elements[elements_id.find(name)->second];
	}

	static Book read(const std::string filename) {
		std::ifstream file { filename };
		Book book;

		std::string line;
		while (std::getline(file, line)) {
			auto tokens = split(line);
			book.insertElement(tokens[0], std::atoi(tokens[1].c_str()),
					std::vector<std::string>(tokens.begin() + 2, tokens.end()));
		}

		book.cacheCompositions();

		return std::move(book);
	}

	size_t maximumGold(const std::vector<std::string> & elements_to_combine) {
		std::vector<size_t> my_elements_id;
		my_elements_id.reserve(elements_to_combine.size());
		for (auto element : elements_to_combine) {
			my_elements_id.push_back(elements_id.find(element)->second);
		}
		std::sort(my_elements_id.begin(), my_elements_id.end());

		// find out all combinations
		std::deque<std::vector<size_t> > left_to_combine;
		left_to_combine.push_back(my_elements_id);

		size_t max_gold = 0;
		while (!left_to_combine.empty()) {
			auto next = std::move(left_to_combine.front());
			left_to_combine.pop_front();

			// calculate this options gold
			size_t gold = 0;
			for (auto & element : next) {
				gold += elements[element].gold;
			}

			if (gold > max_gold) {
				max_gold = gold;
			}

			// look for combinations
			for (auto & element : elements) {
				// check if I can create this element with what I got
				auto & element_composition = element.composition_ids;
				if (!element_composition.empty()
						&& std::includes(next.begin(), next.end(),
								element_composition.begin(),
								element_composition.end())) {
					// new combination found
					std::vector<size_t> new_combination;
					std::set_difference(next.begin(), next.end(),
							element_composition.begin(),
							element_composition.end(),
							std::back_inserter(new_combination));
					new_combination.push_back(element.id);
					std::sort(new_combination.begin(), new_combination.end());
					left_to_combine.push_back(std::move(new_combination));
				}
			}
		}

		return max_gold;
	}

	std::vector<Element> elements;
	std::map<std::string, size_t> elements_id;

private:
	void cacheCompositions() {
		for (auto & element : elements) {
			element.composition_ids.reserve(element.composition.size());
			for (auto component : element.composition) {
				element.composition_ids.push_back(
						elements_id.find(component)->second);
			}
			std::sort(element.composition_ids.begin(),
					element.composition_ids.end());
		}
	}
};

int main() {
	auto book = Book::read("book.data");

	size_t N;
	std::cin >> N;
	std::cin.ignore();
	while (N--) {
		std::string line;
		std::getline(std::cin, line);
		auto elements = split(line);
		std::cout << book.maximumGold(elements) << std::endl;
	}
}
