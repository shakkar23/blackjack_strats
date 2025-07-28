#pragma once

#include "game_view.hpp"
#include "../hand.hpp"
#include "action.hpp"

namespace BlackJack {
	template <typename game_rules>
	struct random_player {
		int get_bet_amount() const {
			return 10; // Fixed bet amount for simplicity
		}

		int get_insurance_amount(const game_view<game_rules>& view) const {
			return 0; // No insurance for this player
		}

		Action get_action(const game_view<game_rules>& view, const Hand& player_hand) const {
			// Randomly choose an action
			int action = rand() % 5; // 5 possible actions
			switch (action) {
				case 0: return Action::Hit;
				case 1: return Action::Stand;
				case 2: return Action::Surrender;
				case 3: return Action::DoubleDown;
				case 4: return Action::Split;
				default: return Action::Stand; // Fallback
			}
		}

		int money = 1000; // Starting money for the player
	};
};