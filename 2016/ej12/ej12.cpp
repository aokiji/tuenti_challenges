#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include <algorithm>
#include <deque>

struct InfectedCity {
	InfectedCity(const std::string & name) :
			name(name), infected_from(nullptr), level(0), total_descendants(0), depth(
					0) {
	}

	void infectedFrom(InfectedCity *city) {
		if (infected_from != nullptr) {
			throw std::logic_error("Ya tiene padre");
		}
		infected_from = city;
		if (infected_from != nullptr) {
			infected_from->spread_to.push_back(this);
		}

		level = city->level + 1;
		size_t visit_depth = 1;
		for (auto curr_city = city; curr_city != nullptr;
				curr_city = curr_city->infected_from) {
			curr_city->depth = std::max(curr_city->depth, visit_depth);
			curr_city->total_descendants++;
			visit_depth++;
		}
	}

	std::string name;
	InfectedCity *infected_from;
	std::vector<InfectedCity *> spread_to;
	size_t level, total_descendants, depth;
};

struct Virus {
	Virus(std::istream & is, size_t jumps) {
		read(is, jumps);
	}

	Virus(const Virus & original, InfectedCity * root) {
		cities.reserve(root->total_descendants + 1);
		std::deque<InfectedCity *> stack;
		for (auto & child : root->spread_to) {
			stack.push_back(child);
		}
		while (!stack.empty()) {
			auto curr_city = stack.front();
			for (auto & child : curr_city->spread_to) {
				stack.push_back(child);
			}
			auto id1 = registerCity(curr_city->infected_from->name);
			auto id2 = registerCity(curr_city->name);
			id2->infectedFrom(id1);
			stack.pop_front();
		}
	}

	void read(std::istream & is, size_t jumps) {
		cities.reserve(jumps + 1);
		for (size_t jump = 0; jump < jumps; ++jump) {
			std::string city1, city2;
			is >> city1 >> city2;
			auto id1 = registerCity(city1);
			auto id2 = registerCity(city2);
			id2->infectedFrom(id1);
		}
	}

	std::map<std::string, std::string> mapTo(Virus & virus) {
		std::map<std::string, std::string> mapping;
		std::map<InfectedCity *, char> already_mapped;

		if (!this->compare(virus)) {
			return mapping;
		}

		mapping[root()->name] = virus.root()->name;
		already_mapped[root()] = 1;

		for (auto & city : city_pointer) {
			if (mapping.find(city.first) != mapping.end()) {
				continue;
			}

			auto candidates_second_virus = virus.findSimilarCities(
					*city.second);

			if (candidates_second_virus.size() == 1) {
				mapping[city.first] = candidates_second_virus.front()->name;
				already_mapped[candidates_second_virus.front()] = 1;
				continue;
			}

			Virus city_virus(*this, city.second);
			for (auto & candidate : candidates_second_virus) {
				if (already_mapped.find(candidate) == already_mapped.end()) {
					Virus second_city_virus(virus, candidate);
					if (city_virus.compare(second_city_virus)) {
						mapping[city.first] = candidate->name;
						already_mapped[candidate] = 1;
						break;
					}
				}
			}

			// if no candidate found then there is no mapping
			if (mapping.find(city.first) == mapping.end()) {
				return std::map<std::string, std::string>();
			}
		}
		return mapping;
	}

private:
	struct State {
		size_t alternatives_begin;
		size_t alternatives_end;
	};

public:
	bool compare(Virus & virus) {
		orderCities();
		virus.orderCities();

		std::deque<State> stack;
		std::map<InfectedCity *, InfectedCity *> mapping = {
				{ nullptr, nullptr } };

		for (size_t position = 0; position < ordered_cities.size();
				++position) {
			auto & city1 = ordered_cities[position];
			auto & city2 = virus.ordered_cities[position];
			size_t alternatives = position;
			for (;
					alternatives < ordered_cities.size()
							&& similarCitiesFromDifferentVirus(
									*ordered_cities[position],
									*virus.ordered_cities[alternatives]);
					++alternatives) {
			}

			bool rollback = false;
			if (alternatives > position + 1) {
				// more than one match, try permutations
				if (validPermutation(this->ordered_cities, virus.ordered_cities,
						position, alternatives, mapping, false)) {
					stack.push_front(State { position, alternatives });
					position = alternatives - 1;
					continue;
				} else {
					rollback = true;
				}
			}

			bool equalCities = alternatives != position
					&& city1->infected_from == mapping[city2->infected_from];

			if (rollback || !equalCities) {
				// cities are different, wrong match!
				while (!stack.empty()) {
					auto & state = stack.front();
					// get next permutation if any
					if (validPermutation(this->ordered_cities,
							virus.ordered_cities, state.alternatives_begin,
							state.alternatives_end, mapping)) {
						position = state.alternatives_end - 1;
						break;
					}
					stack.pop_front();
				}

				// all possibilites explored already
				if (stack.empty()) {
					return false;
				}
			} else {
				mapping[city2] = city1;
			}
		}

		return !mapping.empty();
	}

private:
	InfectedCity * root() {
		auto it = std::find_if(cities.begin(), cities.end(),
				[] (InfectedCity & city) {
					return city.infected_from == nullptr;
				});
		if (it == cities.end()) {
			throw std::logic_error("Couldn't find root");
		}
		return &(*it);
	}

	InfectedCity * registerCity(const std::string & city_name) {
		auto it = city_pointer.find(city_name);
		if (it == city_pointer.end()) {
			cities.push_back(std::move(InfectedCity(city_name)));
			city_pointer[city_name] = &cities.back();
			return &cities.back();
		}
		return it->second;
	}

	void orderCities() {
		if (ordered_cities.empty()) {
			std::transform(cities.begin(), cities.end(),
					std::back_inserter(ordered_cities),
					[] (InfectedCity & c) {return &c;});
			std::sort(ordered_cities.begin(), ordered_cities.end(),
					[] (InfectedCity * c1, InfectedCity * c2) {
						if (c1->level == c2->level) {
							if (c1->total_descendants == c2->total_descendants) {
								if (c1->depth == c2->depth) {
									if (c1->infected_from != nullptr) {
										if (c1->infected_from->total_descendants != c2->infected_from->total_descendants) {
											return c1->infected_from->total_descendants < c2->infected_from->total_descendants;
										}
									}
									return c1->name < c2->name;
								}
								return c1->depth < c2->depth;
							}
							return c1->total_descendants < c2->total_descendants;
						}
						return c1->level < c2->level;
					});
		}
	}

	std::vector<InfectedCity *> findSimilarCities(const InfectedCity & city) {
		std::vector<InfectedCity *> similar_cities;
		for (auto & c : city_pointer) {
			auto city2 = c.second;
			// if two cities have same number of descendants, depth and children at the
			// same level they can be candidates for equivalency
			if (city.level == city2->level && city.depth == city2->depth
					&& city.total_descendants == city2->total_descendants
					&& city.spread_to.size() == city2->spread_to.size()) {
				similar_cities.push_back(city2);
			}
		}
		return similar_cities;
	}

	static bool similarCitiesFromDifferentVirus(const InfectedCity & c1,
			const InfectedCity & c2) {
		return c1.level == c2.level
				&& c1.total_descendants == c2.total_descendants
				&& (c1.infected_from == nullptr
						|| c1.infected_from->total_descendants
								== c2.infected_from->total_descendants)
				&& c1.spread_to.size() == c2.spread_to.size()
				&& c1.depth == c2.depth;
	}

	static bool nextPermutation(std::vector<InfectedCity *> & ordered_cities,
			size_t alternatives_begin, size_t alternatives_end) {
		if (!std::next_permutation(ordered_cities.begin() + alternatives_begin,
				ordered_cities.begin() + alternatives_end,
				compareCitiesInPermutation)) {
			std::sort(ordered_cities.begin() + alternatives_begin,
					ordered_cities.begin() + alternatives_end,
					compareCitiesInPermutation);
			return false;
		}
		return true;
	}

	static bool compareCitiesInPermutation(InfectedCity *c1, InfectedCity *c2) {
		return c1->name < c2->name;
	}

	static bool validPermutation(
			const std::vector<InfectedCity *> & ordered_cities1,
			std::vector<InfectedCity *> & ordered_cities2,
			size_t alternatives_begin, size_t alternatives_end,
			std::map<InfectedCity *, InfectedCity *> & mapping,
			bool firstPermute = true) {
		if (firstPermute) {
			if (!nextPermutation(ordered_cities2, alternatives_begin,
					alternatives_end)) {
				return false;
			}
		}
		do {
			// update mappings
			bool valid = true;
			for (size_t i = alternatives_begin; valid && i < alternatives_end;
					++i) {
				if (ordered_cities1[i]->infected_from
						== mapping[ordered_cities2[i]->infected_from]) {
					mapping[ordered_cities2[i]] = ordered_cities1[i];
				} else {
					valid = false;
				}
			}
			if (valid) {
				return true;
			}
		} while (nextPermutation(ordered_cities2, alternatives_begin,
				alternatives_end));

		return false;
	}

	std::vector<InfectedCity *> ordered_cities;
	std::vector<InfectedCity> cities;
	std::map<std::string, InfectedCity*> city_pointer;
};

int main() {
	size_t N;
	std::cin >> N;

	Virus original_virus { std::cin, N - 1 };

	size_t other_viruses;
	std::cin >> other_viruses;
	for (size_t num_case = 1; num_case <= other_viruses; ++num_case) {
		Virus other_virus { std::cin, N - 1 };
		std::cout << "Case #" << num_case << ":";
		auto mapping = original_virus.mapTo(other_virus);
		if (mapping.empty()) {
			std::cout << " NO";
		} else {
			for (auto & map : mapping) {
				std::cout << " " << map.first << "/" << map.second;
			}
		}
		std::cout << "\n";
	}
}
