#pragma once

#include "../card.hpp"
#include "../hand.hpp"
#include "../value/hilo.hpp"
#include "../stats.hpp"
#include "../game_rules.hpp"

#include <vector>

namespace BlackJack {
	template <typename game_rules = BlackJack::standard_game_rules>
	struct game_view {
		const Card &dealer_upcard;
		const std::vector<std::vector<Hand>> &player_hands;
		const statistics& stats;

		[[nodiscard]] float true_count() const noexcept {
			float count = static_cast<float>(stats.hilo_count());
			int counted_cards = stats.cards_counted_so_far();

			return count / (static_cast<float>(52 * game_rules::num_decks - counted_cards) / 52.f);
		}
	};
};
