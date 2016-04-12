#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <memory>
#include <deque>

struct Island {
	Island(std::string name, size_t cost, size_t order) :
			name(std::move(name)), cost(cost), order(order) {

	}

	bool operator<(const Island & is) const {
		return order < is.order;
	}

	bool operator==(const Island & is) const {
		return order == is.order;
	}

	bool operator!=(const Island & is) const {
		return order != is.order;
	}

	std::string name;
	int cost;
	size_t order;
};

struct Route {
	Route(Island is1, Island is2, size_t cost) :
			island1(is1), island2(is2), cost(cost) {

	}

	const Island & DestinationIsland() const {
		return island2;
	}

	Island island1, island2;
	size_t cost;
};

struct Ship {
	Ship(size_t num, std::string name, size_t gold) :
			num(num), name(std::move(name)), gold(gold) {

	}

	bool operator<(const Ship & s) const {
		return this->num < s.num;
	}

	bool operator==(const Ship & s) const {
		return num == s.num;
	}

	bool operator!=(const Ship & s) const {
		return num != s.num;
	}

	void Pillage() {
		gold += 10;
	}

	size_t num;
	std::string name;
	int gold;
};

struct Map {
	void InsertIsland(std::string name, size_t cost) {
		islands_by_name.insert(
				std::make_pair(name,
						Island(name, cost, islands_by_name.size() + 1)));
	}

	Island & GetIsland(const std::string & name) {
		return islands_by_name.at(name);
	}

	void Insert(Route route) {
		routes_by_origin[route.island1.name].push_back(std::move(route));
	}

	std::vector<Route> RoutesFrom(Island & island) {
		return routes_by_origin[island.name];
	}

	std::map<std::string, Island> islands_by_name;
	std::map<std::string, std::vector<Route> > routes_by_origin;
};

struct Journey {
	Journey(Ship ship, Island location) :
			ship(std::move(ship)), positions(1, std::move(location)) {

	}

	bool Visited(const Island & is) const {
		return std::find(positions.begin(), positions.end(), is)
				!= positions.end();
	}

	void Stay() {
		positions.push_back(positions.back());
	}

	void Move(Route & r) {
		if (r.island1 != positions.back()) {
			throw std::logic_error("Cannot move using this route");
		}
		positions.push_back(r.island2);
	}

	bool Stayed() const {
		return positions.size() > 2
				&& positions.back() == positions[positions.size() - 2];
	}

	Island & CurrentPosition() {
		return positions.back();
	}

	Ship ship;
	std::vector<Island> positions;
};

struct Game {
	void Insert(Ship ship, Island position) {
		ships.insert(std::make_pair(ship.num, Journey(ship, position)));
		if (ships.size() == 1) {
			my_ship = ships.begin()->first;
		}
	}

	Ship & UserShip() {
		return ships.find(my_ship)->second.ship;
	}

	/**
	 * Finds possible routes for a ship
	 */
	std::vector<Route> PossibleRoutes(Journey & journey) {
		auto possible_dest = map.RoutesFrom(journey.positions.back());
		for (auto it = possible_dest.begin(); it != possible_dest.end(); ++it) {
			// 3. A Pirate never goes back! The ships can’t travel to an island where they have been previously.
			if (journey.Visited(it->DestinationIsland())) {
				it = possible_dest.erase(it);
			}
		}
		return possible_dest;
	}

	void PlayTurn(std::shared_ptr<Route> user_route) {
		// 6. Ships are moved following the ship number in order.
		for (auto & ship_journey : ships) {
			auto & journey = ship_journey.second;
			auto & ship = journey.ship;
			auto possible_routes = PossibleRoutes(journey);
			if (ship.num == my_ship) {
				if (!user_route) {
					journey.Stay();
					ship.Pillage();
				} else {
					journey.Move(*user_route);
					// When you take a route, subtract the route cost and node cost from your gold. The node cost may be negative.
					ship.gold -= user_route->island2.cost;
					ship.gold -= user_route->cost;
				}
			} else if (ship.num % 2 == 0) {
				// The other even numbered ships will always take the route to the next island with the maximum route cost. If there is more than one, choose the first following the input order. If there are zero, the ship skips its move.
				if (possible_routes.empty()) {
					journey.Stay();
				} else {
					auto max_route =
							std::max_element(possible_routes.begin(),
									possible_routes.end(),
									[] (const Route & r1, const Route & r2) {return r1.cost < r2.cost;});
					journey.Move(*max_route);
				}
			} else {
				// The other odd numbered ships will always take the route to the next island with the minimum route cost. If there is more than one, choose the first following the input order. If there are zero, the ship skips its move.
				if (possible_routes.empty()) {
					journey.Stay();
				} else {
					auto min_route =
							std::min_element(possible_routes.begin(),
									possible_routes.end(),
									[] (const Route & r1, const Route & r2) {return r1.cost < r2.cost;});
					journey.Move(*min_route);
				}
			}

			// Subtract from your gold the sum of all the gold from the other ships that when they take a route end their movement on your node. Do not subtract this gold if you reached Raftel.
			auto my_current_position = ships.at(my_ship).CurrentPosition();
			if (ship != UserShip() && my_current_position != FinalIsland()) {
				if (journey.CurrentPosition() == my_current_position
						&& !journey.Stayed()) {
					UserShip().gold -= ship.gold;
				}
			}

			if (!winner && journey.CurrentPosition() == FinalIsland()) {
				winner = std::make_shared < Ship > (ship);
			}
		}

		// 7. After all ships have moved, if you have less than zero gold, you are instead considered to have zero gold.
		if (UserShip().gold < 0)
			UserShip().gold = 0;
	}

	Island & FinalIsland() {
		static Island island = map.GetIsland("Raftel");
		return island;
	}

	bool Ended() {
		return static_cast<bool>(winner);
	}

	Ship & Winner() {
		return *winner;
	}

	Game Fork() {
		Game game = *this;
		game.PlayTurn(std::shared_ptr<Route>());
		return game;
	}

	Game Fork(Route & route) {
		Game game = *this;
		game.PlayTurn(std::make_shared < Route > (route));
		return game;
	}

	/**
	 * Plays a turn returning spawn games with all possible moves
	 */
	std::vector<Game> SpawnTurn() {
		// move all ships
		std::vector<Game> spawn_games;

		if (Ended()) {
			return spawn_games;
		}

		if (UserShip().gold == 0) {
			// 8. You can’t travel a route with zero gold.
			spawn_games.push_back(Fork());
		} else {
			auto my_routes = PossibleRoutes(ships.at(my_ship));
			spawn_games.reserve(my_routes.size() + 1);

			for (auto & possible_route : my_routes) {
				spawn_games.push_back(Fork(possible_route));
			}
			// we can also stay
			spawn_games.push_back(Fork());
		}

		return spawn_games;
	}

	/**
	 * Runs all possible escenarios and returns the best outcome
	 */
	int Run() {
		std::deque<Game> running_games = { *this };
		std::vector<Game> won_games;

		while (!running_games.empty()) {
			auto next = running_games.front();
			running_games.pop_front();

			auto spawn_games = next.SpawnTurn();
			for (auto & spawned_game : spawn_games) {
				if (spawned_game.Ended()
						&& spawned_game.Winner() == UserShip()) {
					won_games.push_back(spawned_game);
				} else {
					running_games.push_back(spawned_game);
				}
			}
		}

		if (won_games.empty()) {
			std::cerr << "No winning game" << std::endl;
			exit(-1);
		}
		auto max = std::max_element(won_games.begin(), won_games.end(),
				[] (Game & g1, Game & g2) {return g1.UserShip().gold < g2.UserShip().gold;});
		return max->UserShip().gold;
	}

	Map map;
	size_t my_ship;
	std::map<size_t, Journey> ships;
	std::shared_ptr<Ship> winner;
};

int main() {
	Game game;
	size_t num_of_islands;
	std::cin >> num_of_islands;
	while (num_of_islands--) {
		std::string island_name;
		size_t cost;
		std::cin >> island_name >> cost;
		game.map.InsertIsland(island_name, cost);
	}

	size_t num_of_routes;
	std::cin >> num_of_routes;
	while (num_of_routes--) {
		std::string is1, is2;
		size_t cost;
		std::cin >> is1 >> is2 >> cost;
		game.map.Insert(
				Route(game.map.GetIsland(is1), game.map.GetIsland(is2), cost));
	}

	size_t num_of_ships;
	std::cin >> num_of_ships;
	while (num_of_ships--) {
		std::string ship_name, starting_at;
		size_t initial_gold, ship_num;
		std::cin >> ship_num >> ship_name >> initial_gold >> starting_at;
		game.Insert(Ship(ship_num, ship_name, initial_gold),
				game.map.GetIsland(starting_at));
	}

	std::cout << game.Run() << std::endl;
}
