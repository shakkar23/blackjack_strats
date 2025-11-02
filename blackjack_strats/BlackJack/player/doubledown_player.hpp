#pragma once

#include "action.hpp"
#include "../hand.hpp"
#include "game_view.hpp"
#include "../value/hand_scoring.hpp"
#include "../value/hilo.hpp"
#include "../game_rules.hpp"

namespace BlackJack {
	template <typename game_rules = BlackJack::standard_game_rules>
	struct doubledown_player {
		struct datum {
			float true_count;
			Card dealer_upcard;
			Hand player_hand;
		};
		struct result {
			int amount_won;
		};

		using datas = std::variant<datum, result>;

		int get_bet_amount() const {
			return 10; // Fixed bet amount for simplicity
		}

		int get_insurance_amount(const game_view<game_rules>& view) const {
			return 0; // No insurance for this player
		}

		Action get_action(const game_view<game_rules>& view, const Hand& player_hand) {
			if (data.size() == 0 or std::holds_alternative<result>(data.back()))
				data.push_back(datum{ view.true_count(), view.dealer_upcard, player_hand });
			else
				std::get<datum>(data.back()) = datum{ view.true_count(), view.dealer_upcard, player_hand };

			int score = score_hand(player_hand);
			if (score == 21) {
				return Action::Stand;
			}

			return Action::DoubleDown;
		}

		void resolve_bet(int amount) {
			data.push_back(result{ amount });
			money += amount;
		}

		void resolve_sidebet(int amount) {
			money += amount;
		}

		int money = 0; // Starting money for the player
		std::vector<datas> data;
	};
};