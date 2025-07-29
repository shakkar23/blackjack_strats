#pragma once
#include "action.hpp"
#include "../hand.hpp"
#include "game_view.hpp"
#include "../hand_scoring.hpp"

namespace BlackJack {
	template <typename game_rules>
	struct normal_player {
		int get_bet_amount() const {
			return 10; // Fixed bet amount for simplicity
		}

		int get_insurance_amount(const game_view<game_rules>& view) const {
			return 0; // No insurance for this player
		}

		Action get_action(const game_view<game_rules>& view, const Hand& player_hand) const {
			int score = score_hand(player_hand);

			if (score < 17)
				return Action::Hit;
			return Action::Stand;
		}

		int money = 1000; // Starting money for the player
	};
};