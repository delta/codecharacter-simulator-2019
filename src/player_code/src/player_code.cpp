#include "player_code/player_code.h"

namespace player_code {

using namespace player_state;

State PlayerCode::Update(State state) {

	// Hello and Welcome to Code Character!!

	// Code Character is a strategy code writing competition, where you'll be
	// controlling some robots that will face off against each other

	// We have two kinds of units, villagers and soldiers. You start out only
	// with some villagers though. Let's print some information about your
	// villagers.
	logr << "Number of villagers : " << state.villagers.size() << "\n";

	// logr is like std::cout. Use it to print information to your console
	// As you can see, we have an object called state, that contains some info
	// State has a vector named villagers, that contains information about all
	// our villagers.

	// We can give commands to our villagers to have them perform actions.
	// Remember to check if your villager is alive before you make it perform
	// anything!

	// Villagers can mine gold from goldmines present on the map.
	// Let's have them mine from the first gold mine if we're running low on
	// gold.
	if (state.gold < 1000) {

		// Range based loops are convenient to use. We use a reference to
		// ensure that our changes are reflected and not made on a copy!
		for (auto &villager : state.villagers) {

			villager.mine(state.gold_mine_offsets[0]);
			// State has a vector that has the locations of goldmines on the
			// map. A villager asked to mine a region will automatically go
			// there using the shortest path found and start mining for gold.
		}
	} else {

		// Let's say we want two villagers to always mine gold
		// Notice we use index based loop here and check for the size of
		// villagers
		for (int i = 0; i < 2 && i < state.villagers.size(); ++i) {
			state.villagers[i].mine(state.gold_mine_offsets[0]);
		}

		// Aside from moving, mining and attacking,
		// villagers can build factories.
		// A factory can produces units - villagers and soldiers.

		// Let's check how many built factories there are in the current state
		int num_factories = 0;
		for (auto &factory : state.factories) {
			if (factory.state != FactoryState::UNBUILT) {
				num_factories++;
			}
		}

		// If we don't have any factories, let's built one
		if (num_factories == 0) {
			for (int i = 2; i < state.villagers.size(); ++i) {

				// The map has LAND, WATER and GOLD_MINE.
				// Let's say we want to build the factory at tile (2, 3)
				// Naturally, we can only build factories on land,
				// so let's check!
				if (state.map[2][3] == TerrainType::LAND) {

					// Vec2D is a handy utility class to represent 2D vectors.
					state.villagers[i].build(Vec2D(2, 3),
					                         FactoryProduction::SOLDIER);

					// Remember, the X-axis is from left to right, and
					// Y-axis is from top to bottom of the map.
				}

				// FactoryProduction lets you specify what it should produce.
				// You can switch between soldiers and villagers for the same
				// factory!
			}
		}
	}

	// Your opponent is probably building soldiers to destroy your units.
	// Quite like villagers, State has a vector named factories, that contains
	// information about all our factories.
	for (auto &factory : state.factories) {

		// You can change what the factory produces at any turn.
		if (state.villagers.size() < 5)
			factory.produce_villagers();
		else
			factory.produce_soldiers();
	}

	// Whenever our soldiers do get built, we might want to have them move
	// around and attack our opponent. Your opponent is probably using their
	// soldiers to destroy your units too!
	for (auto &soldier : state.soldiers) {

		// If the opponent has any villagers, let's attack the first one
		if (!state.enemy_villagers.empty()) {
			// Each unit has a state associated with it. SoldierState for
			// soldiers, VillagerState for villagers and FactoryState for
			// factories. Here a soldier is commanded to attack an enemy
			// soldier, only if it's in the IDLE state right now
			if (soldier.state == SoldierState::IDLE) {
				soldier.attack(state.enemy_villagers[0]);
				break;
			}
		}
	}

	// While this should give you a decent start, we highly recommend
	// reading the docs provided. It should get you up to date with
	// all there is to State and we have some helper snippets and methods
	// so you start competing right away!

	return state;
}
} // namespace player_code
