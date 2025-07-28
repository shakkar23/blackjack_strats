#pragma once

#include "../card.hpp"
#include "../hand.hpp"

#include <vector>

namespace BlackJack {
	template <typename game_rules>
	struct game_view {
		const Card &dealer_upcard;
		const std::vector<std::vector<Hand>> &player_hands;
	};
};
