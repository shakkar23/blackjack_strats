// blackjack_strats.cpp : Defines the entry point for the application.
//
#include <glaze/glaze.hpp>

#include "BlackJack/game.hpp"
#include "BlackJack/card.hpp"

#include <iostream>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <variant>


struct data_stuff {
	BlackJack::Action action;
	float true_count;
	BlackJack::Card dealer_upcard;
	BlackJack::Hand player_hand;
	int amount_won;
};

// glaze stuff for serde
template <>
struct glz::meta<data_stuff> {
	using T = data_stuff;
	static constexpr auto value = object(
		&T::action,
		&T::true_count,
		&T::dealer_upcard,
		&T::player_hand,
		&T::amount_won
	);
};

template <>
struct glz::meta<BlackJack::Card> {
	using T = BlackJack::Card;
	static constexpr auto value = object(
		&T::suit,
		&T::rank
	);
};

// suit meta 
template<>
struct glz::meta<BlackJack::Suit> {
	using T = BlackJack::Suit;
	static constexpr auto value = glz::enumerate(
		BlackJack::Suit::Hearts,
		BlackJack::Suit::Diamonds,
		BlackJack::Suit::Clubs,
		BlackJack::Suit::Spades
	);
};

// rank meta
template<>
struct glz::meta<BlackJack::Rank> {
	using T = BlackJack::Rank;
	static constexpr auto value = glz::enumerate(
		BlackJack::Rank::Ace,
		BlackJack::Rank::Two,
		BlackJack::Rank::Three,
		BlackJack::Rank::Four,
		BlackJack::Rank::Five,
		BlackJack::Rank::Six,
		BlackJack::Rank::Seven,
		BlackJack::Rank::Eight,
		BlackJack::Rank::Nine,
		BlackJack::Rank::Ten,
		BlackJack::Rank::Jack,
		BlackJack::Rank::Queen,
		BlackJack::Rank::King
	);
};

template<>
struct glz::meta<BlackJack::Action> {
	using T = BlackJack::Action;
	static constexpr auto value = glz::enumerate(
		BlackJack::Action::Hit,
		BlackJack::Action::Stand,
		BlackJack::Action::DoubleDown,
		BlackJack::Action::Split,
		BlackJack::Action::Surrender
	);
};

template <>
struct glz::meta<BlackJack::Hand> {
	using T = BlackJack::Hand;
	static constexpr auto value = object(
		&T::cards
	);
};

std::vector<data_stuff> data;

int datagen()
{
	BlackJack::game<BlackJack::standard_game_rules> gamington((int)time(0));

	for (int epoch = 0; epoch < 20'000 * 3; epoch++) {
		gamington.add_player(BlackJack::stand_player<BlackJack::standard_game_rules>{});
		gamington.add_player(BlackJack::hit_player<BlackJack::standard_game_rules>{});
		gamington.add_player(BlackJack::doubledown_player<BlackJack::standard_game_rules>{});
		gamington.add_player(BlackJack::stand_player<BlackJack::standard_game_rules>{});
		gamington.add_player(BlackJack::hit_player<BlackJack::standard_game_rules>{});
		gamington.add_player(BlackJack::doubledown_player<BlackJack::standard_game_rules>{});

		for (int i = 0; i < 100; ++i) {
			gamington.play_round();
		}

		for (int j = 0; j < gamington.get_players().size(); j++) {
			std::visit([&](auto&& player) {
				using PlayerType = std::decay_t<decltype(player)>;
				if constexpr (std::is_same_v<PlayerType, BlackJack::stand_player<BlackJack::standard_game_rules>>) {
					data_stuff results = { BlackJack::Action::Surrender, 0,{BlackJack::Suit::Spades, BlackJack::Rank::Ace},{}};
					for (const auto& d : player.data) {
						std::visit([&](auto&& p) {
							using U = std::decay_t<decltype(p)>;
							if constexpr (std::is_same_v<U, PlayerType::datum>) {
								results = { BlackJack::Action::Stand, p.true_count, p.dealer_upcard, p.player_hand };
							}
							else if constexpr (std::is_same_v<U, PlayerType::result>) {
								results.amount_won = p.amount_won;
								data.push_back(results);
							}
						}, d);
					}
				} else if constexpr (std::is_same_v<PlayerType, BlackJack::hit_player<BlackJack::standard_game_rules>>) {
					data_stuff results = { BlackJack::Action::Surrender, 0,{BlackJack::Suit::Spades, BlackJack::Rank::Ace},{} };
					for (const auto& d : player.data) {
						std::visit([&](auto&& p) {
							using U = std::decay_t<decltype(p)>;
							if constexpr (std::is_same_v<U, PlayerType::datum>) {
								results = { BlackJack::Action::Hit, p.true_count, p.dealer_upcard, p.player_hand };
							}
							else if constexpr (std::is_same_v<U, PlayerType::result>) {
								results.amount_won = p.amount_won;
								data.push_back(results);
							}
							}, d);
					}
				} else if constexpr (std::is_same_v<PlayerType, BlackJack::doubledown_player<BlackJack::standard_game_rules>>) {
					data_stuff results = { BlackJack::Action::Surrender, 0,{BlackJack::Suit::Spades, BlackJack::Rank::Ace},{} };
					for (const auto& d : player.data) {
						std::visit([&](auto&& p) {
							using U = std::decay_t<decltype(p)>;
							if constexpr (std::is_same_v<U, PlayerType::datum>) {
								results = { BlackJack::Action::Stand, p.true_count, p.dealer_upcard, p.player_hand };
							}
							else if constexpr (std::is_same_v<U, PlayerType::result>) {
								results.amount_won = p.amount_won;
								data.push_back(results);
							}
							}, d);
					}
				}
			}, gamington.get_players()[j]);
		}

		gamington.reset();
	}


	std::string buffer{};
	auto ec = glz::write_file_beve(data, "data.beve", buffer);

	if (ec) {
		std::string descriptive_error = glz::format_error(ec, buffer);
		std::cerr << "Error writing file: " << descriptive_error << std::endl;
		return 1;
	}
	return 0;
}

int rank_to_chart_index(BlackJack::Rank rank) {
	switch (rank) {
	case BlackJack::Rank::Two: return 0;
	case BlackJack::Rank::Three: return 1;
	case BlackJack::Rank::Four: return 2;
	case BlackJack::Rank::Five: return 3;
	case BlackJack::Rank::Six: return 4;
	case BlackJack::Rank::Seven: return 5;
	case BlackJack::Rank::Eight: return 6;
	case BlackJack::Rank::Nine: return 7;
	case BlackJack::Rank::Ten: return 8;
	case BlackJack::Rank::Jack: return 8;
	case BlackJack::Rank::Queen: return 8;
	case BlackJack::Rank::King: return 8;
	case BlackJack::Rank::Ace: return 9;
	default: throw std::runtime_error("Invalid rank");
	}
}

int main() {
	// read the beve file, and deserialize it
	// assume the data is already created
	//datagen();
	std::vector<data_stuff> deserialized_data{};
	std::string buffer{};
	auto ec = glz::read_file_beve(deserialized_data, "data.beve", buffer);
	if (ec) {
		std::string descriptive_error = glz::format_error(ec, buffer);
		std::cerr << "Error reading beve: " << descriptive_error << std::endl;
		return 1;
	}
	std::cout << deserialized_data.size();
	// make a basic strategy chart for non splitting and accumulate all of the winnings to this chart
	std::array<std::array<int, 21>, 10> hard_chart_hit{};
	std::array<std::array<int, 21>, 10> hard_chart_double{};
	std::array<std::array<int, 21>, 10> hard_chart_stand{};
	// hard_chart[dealer_upcard][player_hand_score] = winnings
	std::array<std::array<int, 10>, 10> soft_chart_hit{};
	std::array<std::array<int, 10>, 10> soft_chart_double{};
	std::array<std::array<int, 10>, 10> soft_chart_stand{};
	// soft_chart[dealer_upcard][player_hand_score] = winnings

	for (const auto& d : deserialized_data) {
		int score = BlackJack::score_hand(d.player_hand);
		int dealer_upcard_index = rank_to_chart_index(d.dealer_upcard.rank);
		if(d.action == BlackJack::Action::Hit) {
			if (d.player_hand.is_soft()) {
				soft_chart_hit[dealer_upcard_index][score] += d.amount_won;
			}
			else {
				hard_chart_hit[dealer_upcard_index][score] += d.amount_won;
			}
		}
		else if (d.action == BlackJack::Action::DoubleDown) {
			if (d.player_hand.is_soft()) {
				soft_chart_double[dealer_upcard_index][score] += d.amount_won;
			}
			else {
				hard_chart_double[dealer_upcard_index][score] += d.amount_won;
			}
		}
		else if (d.action == BlackJack::Action::Stand) {
			if (d.player_hand.is_soft()) {
				soft_chart_stand[dealer_upcard_index][score] += d.amount_won;
			}
			else {
				hard_chart_stand[dealer_upcard_index][score] += d.amount_won;
			}
		}
	}

	// output 6 CSVs with the data using glaze

	glz::write_file_csv(hard_chart_hit, "hard_chart_hit.csv", std::string{});

	glz::write_file_csv(hard_chart_double, "hard_chart_double.csv", std::string{});

	glz::write_file_csv(hard_chart_stand, "hard_chart_stand.csv", std::string{});

	glz::write_file_csv(soft_chart_hit, "soft_chart_hit.csv", std::string{});

	glz::write_file_csv(soft_chart_double, "soft_chart_double.csv", std::string{});

	glz::write_file_csv(soft_chart_stand, "soft_chart_stand.csv", std::string{});
	
	
	return 0;
}
