#pragma once

#include "action.hpp"
#include "../hand.hpp"
#include "game_view.hpp"
#include "../value/hand_scoring.hpp"
#include "../value/hilo.hpp"

namespace BlackJack {
	template <typename game_rules>
	struct split_player {

		struct datum {
			float true_count;
			Card dealer_upcard;
			Hand player_hand;
		};

		int get_bet_amount() const {
			return 10; // Fixed bet amount for simplicity
		}

		int get_insurance_amount(const game_view<game_rules>& view) const {
			return 0; // No insurance for this player
		}

		Action get_action(const game_view<game_rules>& view, const Hand& player_hand) {
			if (player_hand.can_split())
			{
				if (data.size() > view.stats.rounds_played_count())
					data[view.stats.rounds_played_count()] = datum{ view.true_count(), view.dealer_upcard, player_hand };
				else
					data.push_back(datum{ view.true_count(), view.dealer_upcard, player_hand });
				return Action::Split;
			}

			return Action::Stand;
		}

		void resolve_bet(int amount) {
			money += amount;
		}

		void resolve_sidebet(int amount) {
			money += amount;
		}

		int money = 0; // Starting money for the player
		std::vector<datum> data;
	};
};