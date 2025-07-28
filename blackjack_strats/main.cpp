// blackjack_strats.cpp : Defines the entry point for the application.
//

#include "BlackJack/game.hpp"

#include <iostream>
#include <unordered_map>

int main()
{
	std::unordered_map<int, int> money_to_count;
	BlackJack::game<BlackJack::standard_game_rules> gamington(time(0));
	for (int epoch = 0; epoch < 300; epoch++) {
		for (int i = 0; i < 10; ++i)
			gamington.add_player(BlackJack::normal_player<BlackJack::standard_game_rules>{});

		for (int i = 0; i < 100; ++i) {
			gamington.play_round();
			for (int j = 0; j < gamington.get_players().size(); j++)
				money_to_count[std::visit([](auto&& p) { return p.money; }, gamington.get_players()[j])]++;
		}
		gamington.reset();
	}
	struct thingy {
		int money;
		int count;
	};
	std::vector<thingy> sorted_data;
	for (auto& [key, value] : money_to_count) {
		sorted_data.push_back(thingy{ key, value });
	}
	
	std::ranges::sort(sorted_data, std::less{}, &thingy::money);
	for (auto& [key, value] : sorted_data)
		std::cout << "Money: " << key << ", Count: " << value << '\n';

	std::cout << std::endl;

	// count the number of players who have less than 1000 money
	int bad_count = 0;
	for (auto& [money, count] : sorted_data) {
		if (money < 1000)
			bad_count += count;
	}

	int good_count = 0;
	for (auto& [money, count] : sorted_data) {
		if (money >= 1000)
			good_count += count;
	}

	std::cout << "Bad players: " << bad_count << ", Good players: " << good_count << '\n';

	return 0;
}
