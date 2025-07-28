

namespace BlackJack {
	struct standard_game_rules {
		static constexpr int min_bet = 1;
		static constexpr int max_bet = 1000;
		static constexpr int dealer_stand_threshold = 17;
		static constexpr float natural_blackjack_payout = 1.5f; // 3:2 payout for natural blackjack
		static constexpr float blackjack_payout = 3.f / 2.f; // 3:2 payout for blackjack
		static constexpr float insurance_payout = 2.f; // 2:1 payout for insurance
		static constexpr float surrender_penalty = 0.50f; // Player loses half their bet on surrender
		static constexpr float reshuffle_threshold = 0.75f; // Reshuffle when shoe is 75% depleted
		static constexpr int num_decks = 6; // Standard game uses 6 decks
	};
};