#pragma once

#include "deck.hpp"
#include "hand.hpp"
#include "game_rules.hpp"
#include "./player/random_player.hpp"
#include "./player/normal_player.hpp"
#include "./player/stand_player.hpp"
#include "./player/basic_strat_player.hpp"

#include "./player/game_view.hpp"
#include "hand_scoring.hpp"

#include <random>
#include <ranges>
#include <vector>
#include <variant>
#include <source_location>

namespace BlackJack {
	template <typename game_rules>
	struct game {
		using player_variant = std::variant<random_player<game_rules>, normal_player<game_rules>, stand_player<game_rules>, BS_player<game_rules>>;
		game(int seed = 0)
			: rng(seed) {
			shoe.new_shoe(game_rules::num_decks);
			shoe.shuffle(rng);
		}
		~game() {}
		game(const game&) = delete;
		game& operator=(const game&) = delete;
		game(game&&) = default;
		game& operator=(game&&) = default;

		inline void play_round() {
			// get all bets from players
			std::vector<std::vector<int>> player_bets;
			{
				for (auto& player : players) {
					int bet_amount = std::visit([](auto&& p) { return p.get_bet_amount(); }, player);
					std::visit([bet_amount](auto&& p) { p.money -= bet_amount; }, player);
					player_bets.push_back(std::vector<int>{ bet_amount });
				}
			}

			// re-shuffle deck if needed
			size_t deck_cutter_thingy = game_rules::num_decks * 52 * game_rules::reshuffle_threshold;
			if (shoe.num_cards() <= deck_cutter_thingy) {
				shoe.new_shoe(game_rules::num_decks);
				shoe.shuffle(rng);
			}
			
			// draw dealer cards
			Hand dealer_hand;
			for (size_t j = 0; j < 2; j++)
				draw_card(dealer_hand);

			// Draw 2 initial cards for each player
			std::vector<std::vector<Hand>> player_hands;

			for (size_t i = 0; i < players.size(); ++i) {
				player_hands.push_back({ Hand{} });
				for (size_t j = 0; j < 2; j++)
					draw_card(player_hands[i][0]);
			}

			// get insurance bets from each player
			std::vector<int> insurance_bets;
			if (int score = score_card(dealer_hand[0]); score == 11) {
				// construct game view
				game_view<game_rules> view{ dealer_hand[0], player_hands };
				for (auto& player : players) {
					int insurance_bet = std::visit([view](auto&& p) { return p.get_insurance_amount(view); }, player);
					insurance_bets.push_back(insurance_bet);
					std::visit([insurance_bet](auto&& p) { p.money -= insurance_bet; }, player);
				}
			}

			game_view<game_rules> view{ dealer_hand[0], player_hands };

			std::vector<bool> natural_blackjack(players.size(), false);

			for (size_t player_iter = 0; player_iter < players.size(); ++player_iter) {

				// skip all natural blackjacks
				if (score_hand(player_hands[player_iter][0]) == 21) {
					natural_blackjack[player_iter] = true;
					continue;
				}
				play_player_hands(player_hands, player_bets, view, player_iter);
				
			}

			play_dealer_hand(dealer_hand);
			int dealer_score = score_hand(dealer_hand);
			bool dealer_natural_blackjack = (dealer_score == 21 and dealer_hand.size() == 2);
			bool dealer_busted = (dealer_score > 21);

			// Resolve bets
			for (size_t player_iter = 0; player_iter < players.size(); ++player_iter) {
				// player has a natural blackjack and also the dealer has a natural blackjack
				if (player_hands[player_iter].size() == 1 and score_hand(player_hands[player_iter][0]) == 21) {
					if (!dealer_natural_blackjack) {
						// give pay ratio to player for natural bj
						std::visit([&](auto&& p) {
							p.money += static_cast<int>(player_bets[player_iter][0] * (game_rules::natural_blackjack_payout + 1));
						}, players[player_iter]);
					}
					continue;
				}

				if (insurance_bets.size() > 0) {
					if (dealer_natural_blackjack) {
						std::visit([&](auto&& p) {
							p.money += static_cast<int>(insurance_bets[player_iter] * game_rules::insurance_payout);
						}, players[player_iter]);
					}
					else {
						std::visit([&](auto&& p) {
							p.money -= insurance_bets[player_iter];
						}, players[player_iter]);
					}
				}

				if (!dealer_natural_blackjack)
					for (auto [player_hand, player_bet] : std::views::zip(player_hands[player_iter], player_bets[player_iter])) {
						int score = score_hand(player_hand);
						if (score > 21)
							continue;

						if (score == dealer_score) {
							std::visit([&](auto&& p) {
								p.money += player_bet;
							}, players[player_iter]);
						}
						else if (dealer_score > 21 || score > dealer_score) {
							std::visit([&](auto&& p) {
								p.money += player_bet * 2;
							}, players[player_iter]);
						}

					}
			}

		}

		inline void play_player_hands(
			std::vector<std::vector<Hand>> &player_hands, 
			std::vector<std::vector<int>> &player_bets, 
			game_view<game_rules> &view,
			size_t player_iter
		) {
			for (size_t hand_iter = 0; hand_iter < player_hands[player_iter].size(); ++hand_iter) {
				Hand* player_hand = &player_hands[player_iter][hand_iter];
				int* bet_amount = &player_bets[player_iter][hand_iter];

				bool continue_playing = true;

				while (continue_playing) {
					Action player_action = std::visit([view, player_hand](auto&& p) {
						return p.get_action(view, *player_hand);
					}, players[player_iter]);

					switch (player_action) {
					case Action::Hit:
						draw_card(*player_hand);
						continue_playing = BlackJack::score_hand(*player_hand) < 21;
						break;
					case Action::Stand:
						continue_playing = false;
						break;
					case Action::Surrender:
						// Player loses half their bet
						std::visit([&](auto&& p) { p.money -= (*bet_amount) / 2; }, players[player_iter]);
						// Player forfeits the hand
						(*bet_amount) = 0;
						continue_playing = false;
						break;
					case Action::DoubleDown:
						(*bet_amount) *= 2;
						draw_card(*player_hand);
						continue_playing = false;
						break;
					case Action::Split:
						if (player_hand->can_split()) {
							auto split_hand = player_hand->split();
							if (split_hand.has_value()) {
								// draw the cards for the split hand and the original hand
								draw_card(*player_hand);
								draw_card(split_hand.value());

								// remove the bet amount from the player
								std::visit([bet_amount](auto&& p) { p.money -= *bet_amount; }, players[player_iter]);

								// add to the player hands and bets
								player_bets[player_iter].push_back(*bet_amount);
								player_hands[player_iter].push_back(std::move(split_hand.value()));

								// update the player hand and bet amount pointers for safe access
								player_hand = &player_hands[player_iter][hand_iter];
								bet_amount = &player_bets[player_iter][hand_iter];
							}
							else {
								throw std::runtime_error("Failed to split hand");
							}
						}
						break;
					}
				} // while(continue_playing)
			} // for(hand_iter)
		}

		inline void draw_card(Hand& hand, const std::source_location& location = std::source_location::current()) {
			auto card = shoe.draw();
			if (card.has_value()) {
				hand.addCard(card.value());
			}
			else {
				throw std::runtime_error(std::string("Failed to draw card at line ") + std::to_string(location.line()) + ": " + card.error());
			}
		}

		inline void play_dealer_hand(Hand& dealer_hand) {
			while(score_hand(dealer_hand) < 17) {
				draw_card(dealer_hand);
			}
		}

		inline void add_player(player_variant player) {
			players.push_back(std::move(player));
		}

		inline auto& get_players() {
			return players;
		}

		inline void reset() {
			shoe.new_shoe(game_rules::num_decks);
			shoe.shuffle(rng);
			players.clear();
		}
	private:
		Deck shoe;
		std::vector<player_variant> players;
		std::linear_congruential_engine<std::uint32_t, 0x5d588b65, 0x269ec3, 0> rng;
	};
};