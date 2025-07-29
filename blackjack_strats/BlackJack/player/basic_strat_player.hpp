#pragma once

#include "action.hpp"
#include "../hand.hpp"
#include "game_view.hpp"
#include "../hand_scoring.hpp"

#include <stdexcept>

namespace BlackJack {
	template <typename game_rules>
	struct BS_player {
		int get_bet_amount() const {
			return 10; // Fixed bet amount for simplicity
		}

		int get_insurance_amount(const game_view<game_rules>& view) const {
			return 0; // No insurance for this player
		}

		Action get_action(const game_view<game_rules>& view, const Hand& player_hand) const {
			int score = score_hand(player_hand);

			if (player_hand.can_split()) {
				Rank duplicate_card = player_hand[0].rank;
				
				if (duplicate_card == Rank::Ace || duplicate_card == Rank::Eight)
					return Action::Split;

				if(duplicate_card == Rank::Ten ||
					duplicate_card == Rank::Jack ||
					duplicate_card == Rank::Queen ||
					duplicate_card == Rank::King
					)
					return Action::Stand;

				if(duplicate_card == Rank::Nine) {
					Action r = Action::Split;
					if (view.dealer_upcard.rank == Rank::Seven ||
						view.dealer_upcard.rank == Rank::Ten ||
						view.dealer_upcard.rank == Rank::Ace) {
						r = Action::Stand;
					}
					return r;
				}

				if(duplicate_card == Rank::Four) {
					if (view.dealer_upcard.rank == Rank::Five ||
						view.dealer_upcard.rank == Rank::Six) {
						return Action::Split;
					} else {
						return Action::Hit;
					}
				}

				if (duplicate_card == Rank::Five) {
					if (view.dealer_upcard.rank == Rank::Ten ||
						view.dealer_upcard.rank == Rank::Ace) {
						return Action::Hit;
					}
					else {
						return Action::DoubleDown;
					}
					
				}
				
				if (duplicate_card == Rank::Six) {
					if (view.dealer_upcard.rank == Rank::Two   ||
						view.dealer_upcard.rank == Rank::Three ||
						view.dealer_upcard.rank == Rank::Four  ||
						view.dealer_upcard.rank == Rank::Five  ||
						view.dealer_upcard.rank == Rank::Six) {
						return Action::Split;
					}
					else {
						return Action::Hit;
					}

				}

				if (duplicate_card == Rank::Two || duplicate_card == Rank::Three || duplicate_card == Rank::Seven) {
					if (view.dealer_upcard.rank == Rank::Eight ||
						view.dealer_upcard.rank == Rank::Nine ||
						view.dealer_upcard.rank == Rank::Ten ||
						view.dealer_upcard.rank == Rank::Ace) {
						return Action::Hit;
					} else {
						return Action::Split;
					}
				}
			}

			if (player_hand.is_soft()) {
				int other_card_value = score - 11;

				switch (other_card_value)
				{
				case 2:
				case 3:
					switch (view.dealer_upcard.rank) {
					case BlackJack::Rank::Five:
					case BlackJack::Rank::Six:
						return Action::DoubleDown;
					case BlackJack::Rank::Seven:
					case BlackJack::Rank::Eight:
						return Action::Stand;
					case BlackJack::Rank::Two:
					case BlackJack::Rank::Three:
					case BlackJack::Rank::Four:
					case BlackJack::Rank::Nine:
					case BlackJack::Rank::Ten:
					case BlackJack::Rank::Jack:
					case BlackJack::Rank::Queen:
					case BlackJack::Rank::King:
					case BlackJack::Rank::Ace:
						return Action::Hit;
					}
				case 4:
				case 5:
					switch (view.dealer_upcard.rank) {
					case BlackJack::Rank::Four:
					case BlackJack::Rank::Five:
					case BlackJack::Rank::Six:
						return Action::DoubleDown;
					case BlackJack::Rank::Seven:
					case BlackJack::Rank::Eight:
						return Action::Stand;
					case BlackJack::Rank::Two:
					case BlackJack::Rank::Three:
					case BlackJack::Rank::Nine:
					case BlackJack::Rank::Ten:
					case BlackJack::Rank::Jack:
					case BlackJack::Rank::Queen:
					case BlackJack::Rank::King:
					case BlackJack::Rank::Ace:
						return Action::Hit;
					}
				case 6:
					switch (view.dealer_upcard.rank) {
					case BlackJack::Rank::Three:
					case BlackJack::Rank::Four:
					case BlackJack::Rank::Five:
					case BlackJack::Rank::Six:
						return Action::DoubleDown;
					case BlackJack::Rank::Seven:
					case BlackJack::Rank::Eight:
						return Action::Stand;
					case BlackJack::Rank::Two:
					case BlackJack::Rank::Nine:
					case BlackJack::Rank::Ten:
					case BlackJack::Rank::Jack:
					case BlackJack::Rank::Queen:
					case BlackJack::Rank::King:
					case BlackJack::Rank::Ace:
						return Action::Hit;
					}
					break;
				case 7:
					switch (view.dealer_upcard.rank) {
					case BlackJack::Rank::Two:
					case BlackJack::Rank::Seven:
					case BlackJack::Rank::Eight:
						return Action::Stand;
					case BlackJack::Rank::Three:
					case BlackJack::Rank::Four:
					case BlackJack::Rank::Five:
					case BlackJack::Rank::Six:
						return Action::DoubleDown;
					case BlackJack::Rank::Nine:
					case BlackJack::Rank::Ten:
					case BlackJack::Rank::Jack:
					case BlackJack::Rank::Queen:
					case BlackJack::Rank::King:
					case BlackJack::Rank::Ace:
						return Action::Hit;
					}
				case 8:
				case 9:
				case 10:
					return Action::Stand;
					break;

				default:
					throw std::runtime_error("Unexpected soft hand value: " + std::to_string(other_card_value));
					break;
				}
			}

			switch (score) {
			case 5:
			case 6:
			case 7:
			case 8:
				return Action::Hit;
			case 9:
				switch (view.dealer_upcard.rank) {
				case BlackJack::Rank::Three:
				case BlackJack::Rank::Four:
				case BlackJack::Rank::Five:
				case BlackJack::Rank::Six:
					return Action::DoubleDown;
				case BlackJack::Rank::Two:
				case BlackJack::Rank::Seven:
				case BlackJack::Rank::Eight:
				case BlackJack::Rank::Nine:
				case BlackJack::Rank::Ten:
				case BlackJack::Rank::Jack:
				case BlackJack::Rank::Queen:
				case BlackJack::Rank::King:
				case BlackJack::Rank::Ace:
					return Action::Hit;
				}
			case 10:
				switch (view.dealer_upcard.rank) {
				case BlackJack::Rank::Two:
				case BlackJack::Rank::Three:
				case BlackJack::Rank::Four:
				case BlackJack::Rank::Five:
				case BlackJack::Rank::Six:
				case BlackJack::Rank::Seven:
				case BlackJack::Rank::Eight:
				case BlackJack::Rank::Nine:
					return Action::DoubleDown;
				case BlackJack::Rank::Ten:
				case BlackJack::Rank::Jack:
				case BlackJack::Rank::Queen:
				case BlackJack::Rank::King:
				case BlackJack::Rank::Ace:
					return Action::Hit;
				}
			case 11:
				switch (view.dealer_upcard.rank) {
				case BlackJack::Rank::Two:
				case BlackJack::Rank::Three:
				case BlackJack::Rank::Four:
				case BlackJack::Rank::Five:
				case BlackJack::Rank::Six:
				case BlackJack::Rank::Seven:
				case BlackJack::Rank::Eight:
				case BlackJack::Rank::Nine:
				case BlackJack::Rank::Ten:
				case BlackJack::Rank::Jack:
				case BlackJack::Rank::Queen:
				case BlackJack::Rank::King:
					return Action::DoubleDown;
				case BlackJack::Rank::Ace:
					return Action::Hit;
				}
			case 12:
				switch (view.dealer_upcard.rank) {
				case BlackJack::Rank::Four:
				case BlackJack::Rank::Five:
				case BlackJack::Rank::Six:
					return Action::Stand;
				case BlackJack::Rank::Two:
				case BlackJack::Rank::Three:
				case BlackJack::Rank::Seven:
				case BlackJack::Rank::Eight:
				case BlackJack::Rank::Nine:
				case BlackJack::Rank::Ten:
				case BlackJack::Rank::Jack:
				case BlackJack::Rank::Queen:
				case BlackJack::Rank::King:
				case BlackJack::Rank::Ace:
					return Action::Hit;
				}
			case 13:
			case 14:
			case 15:
			case 16:
				switch (view.dealer_upcard.rank) {
				case BlackJack::Rank::Two:
				case BlackJack::Rank::Three:
				case BlackJack::Rank::Four:
				case BlackJack::Rank::Five:
				case BlackJack::Rank::Six:
					return Action::Stand;
				case BlackJack::Rank::Seven:
				case BlackJack::Rank::Eight:
				case BlackJack::Rank::Nine:
				case BlackJack::Rank::Ten:
				case BlackJack::Rank::Jack:
				case BlackJack::Rank::Queen:
				case BlackJack::Rank::King:
				case BlackJack::Rank::Ace:
					return Action::Hit;
				}
			case 17:
			case 18:
			case 19:
			case 20:
			case 21:
				return Action::Stand;
			default:
				break;


			}
			

			throw std::runtime_error("Unexpected case");
			return Action::Stand;
		}

		int money = 1000; // Starting money for the player
	};
};