#include <glaze/glaze.hpp>

#include "BlackJack/game.hpp"
#include "BlackJack/card.hpp"

#include <array>
#include <bit>
#include <execution>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <map>
#include <fstream>
#include <variant>


struct data_stuff {
	BlackJack::Action action;
	float true_count;
	BlackJack::Card dealer_upcard;
	BlackJack::Hand player_hand;
	int amount_won;
};

struct data_column {
	long long two;
	long long three;
	long long four;
	long long five;
	long long six;
	long long seven;
	long long eight;
	long long nine;
	long long ten;
	long long ace;
};

struct ev_column {
	double two;
	double three;
	double four;
	double five;
	double six;
	double seven;
	double eight;
	double nine;
	double ten;
	double ace;
};

static long long data_column::* rank_to_chart_index(BlackJack::Rank rank) {
	switch (rank) {
	case BlackJack::Rank::Two: return &data_column::two;
	case BlackJack::Rank::Three: return &data_column::three;
	case BlackJack::Rank::Four: return &data_column::four;
	case BlackJack::Rank::Five: return &data_column::five;
	case BlackJack::Rank::Six: return &data_column::six;
	case BlackJack::Rank::Seven: return &data_column::seven;
	case BlackJack::Rank::Eight: return &data_column::eight;
	case BlackJack::Rank::Nine: return &data_column::nine;
	case BlackJack::Rank::Ten: return &data_column::ten;
	case BlackJack::Rank::Jack: return &data_column::ten;
	case BlackJack::Rank::Queen: return &data_column::ten;
	case BlackJack::Rank::King: return &data_column::ten;
	case BlackJack::Rank::Ace: return &data_column::ace;
	default:
		throw std::runtime_error("Invalid rank for chart index");
	}
}

static int action_to_index(BlackJack::Action a) {
	switch (a)
	{
	case BlackJack::Action::Hit:
		return 0;
	case BlackJack::Action::Stand:
		return 1;
	case BlackJack::Action::DoubleDown:
		return 2;
	case BlackJack::Action::Split:
	case BlackJack::Action::Surrender:
	default:
		throw std::runtime_error("Invalid Action!");
	}
}
static BlackJack::Action index_to_action(int index) {
	switch (index) {
	case 0:
		return BlackJack::Action::Hit;
	case 1:
		return BlackJack::Action::Stand;
	case 2:
		return BlackJack::Action::DoubleDown;

	default:
		throw std::runtime_error("bad index");
	}
}

struct chart {
public:
	// score to dealer upcard, value is 
	std::array<data_column, 21> hard_chart_counts{};
	std::array<data_column, 21> hard_chart{};
	// accumulation of score minus extra ace to dealer upcard
	std::array<data_column, 10> soft_chart_counts{};
	std::array<data_column, 10> soft_chart{};
};

static void log(std::map<int, std::array<chart, 3>>& tc_to_chart, data_stuff& results) {
	int score = BlackJack::score_hand(results.player_hand);
	int action_index = action_to_index(results.action);
	bool is_soft = results.player_hand.is_soft();
	long long data_column::* upcard_offset = rank_to_chart_index(results.dealer_upcard.rank);

	auto& chart = tc_to_chart[(size_t)std::round(results.true_count)][action_index];

	if (is_soft) {
		chart.soft_chart_counts[score - 11 - 1].*upcard_offset += 1;
		chart.soft_chart[score - 11 - 1].*upcard_offset += results.amount_won;
	}
	else {
		chart.hard_chart_counts[score - 1].*upcard_offset += 1;
		chart.hard_chart[score - 1].*upcard_offset += results.amount_won;
	}
}

template<size_t N>
static std::map<int, std::array<chart, 3>> combine_charts(std::array<std::map<int, std::array<chart, 3>>, N>& TC_charts) {
	std::map<int, std::array<chart, 3>> true_count_to_chart;
	for (auto& TC_chart : TC_charts) {
		for (auto& [k, v] : TC_chart) {
			auto& tc_charts = TC_chart[k];
			auto& main_charts = true_count_to_chart[k];
			for (const auto& [tc_chart, main_chart] : std::views::zip(tc_charts, main_charts)) {
				for (const auto& [thread_hc, main_hc] : std::views::zip(tc_chart.hard_chart, main_chart.hard_chart)) {
					auto casted_thread = std::bit_cast<std::array<long long, 10>>(thread_hc);
					auto casted_main = std::bit_cast<std::array<long long, 10>>(main_hc);

					for (const auto& [thread_val, main_val] : std::views::zip(casted_thread, casted_main)) {
						main_val += thread_val;
					}

					auto new_main = std::bit_cast<data_column>(casted_main);
					main_hc = new_main;
				}
				for (const auto& [thread_hc, main_hc] : std::views::zip(tc_chart.hard_chart_counts, main_chart.hard_chart_counts)) {
					auto casted_thread = std::bit_cast<std::array<long long, 10>>(thread_hc);
					auto casted_main = std::bit_cast<std::array<long long, 10>>(main_hc);

					for (const auto& [thread_val, main_val] : std::views::zip(casted_thread, casted_main)) {
						main_val += thread_val;
					}

					auto new_main = std::bit_cast<data_column>(casted_main);
					main_hc = new_main;
				}
				for (const auto& [thread_hc, main_hc] : std::views::zip(tc_chart.soft_chart, main_chart.soft_chart)) {
					auto casted_thread = std::bit_cast<std::array<long long, 10>>(thread_hc);
					auto casted_main = std::bit_cast<std::array<long long, 10>>(main_hc);

					for (const auto& [thread_val, main_val] : std::views::zip(casted_thread, casted_main)) {
						main_val += thread_val;
					}

					auto new_main = std::bit_cast<data_column>(casted_main);
					main_hc = new_main;
				}
			}
		}
	}
	return true_count_to_chart;
}


void datagen() {
	constexpr size_t num_threads = 8;
	std::array<std::map<int, std::array<chart,3>>, num_threads> TC_charts;

	// 15'000'000 -> 10853.6s
	constexpr long long num_games = 15'000'000ll;
	constexpr long long games_per_thread = num_games / TC_charts.size();

	std::for_each(std::execution::par_unseq, TC_charts.begin(), TC_charts.end(), [](auto& TC_chart) {
		std::map<int, std::array<chart, 3>> true_count_to_chart;

		BlackJack::game gamington(std::hash<std::thread::id>{}(std::this_thread::get_id()));
		
		for (long long epoch = 0; epoch < games_per_thread; epoch++) {
			gamington.add_player(BlackJack::stand_player{});
			gamington.add_player(BlackJack::hit_player{});
			gamington.add_player(BlackJack::doubledown_player{});
			gamington.add_player(BlackJack::stand_player{});
			gamington.add_player(BlackJack::hit_player{});
			gamington.add_player(BlackJack::doubledown_player{});
		
			gamington.randomize_players();

			for (int i = 0; i < 100; ++i) {
				gamington.play_round();
			}

			for (auto&players : gamington.get_players()) {
				std::visit([&](auto&& player) {
					using PlayerType = std::decay_t<decltype(player)>;
					if constexpr (std::is_same_v<PlayerType, BlackJack::stand_player<>>) {
						data_stuff results = { BlackJack::Action::Surrender, 0,{BlackJack::Suit::Spades, BlackJack::Rank::Ace},{}};
						for (const auto& d : player.data) {
							std::visit([&](auto&& p) {
								using U = std::decay_t<decltype(p)>;
								if constexpr (std::is_same_v<U, PlayerType::datum>) {
									results = { BlackJack::Action::Stand, p.true_count, p.dealer_upcard, p.player_hand };
								}
								else if constexpr (std::is_same_v<U, PlayerType::result>) {
									results.amount_won = p.amount_won;
									log(true_count_to_chart, results);
								}
							}, d);
						}
					} else if constexpr (std::is_same_v<PlayerType, BlackJack::hit_player<>>) {
						data_stuff results = { BlackJack::Action::Surrender, 0,{BlackJack::Suit::Spades, BlackJack::Rank::Ace},{} };
						for (const auto& d : player.data) {
							std::visit([&](auto&& p) {
								using U = std::decay_t<decltype(p)>;
								if constexpr (std::is_same_v<U, PlayerType::datum>) {
									results = { BlackJack::Action::Hit, p.true_count, p.dealer_upcard, p.player_hand };
								}
								else if constexpr (std::is_same_v<U, PlayerType::result>) {
									results.amount_won = p.amount_won;
									log(true_count_to_chart, results);
								}
							}, d);
						}
					} else if constexpr (std::is_same_v<PlayerType, BlackJack::doubledown_player<>>) {
						data_stuff results = { BlackJack::Action::Surrender, 0,{BlackJack::Suit::Spades, BlackJack::Rank::Ace},{} };
						for (const auto& d : player.data) {
							std::visit([&](auto&& p) {
								using U = std::decay_t<decltype(p)>;
								if constexpr (std::is_same_v<U, PlayerType::datum>) {
									results = { BlackJack::Action::DoubleDown, p.true_count, p.dealer_upcard, p.player_hand };
								}
								else if constexpr (std::is_same_v<U, PlayerType::result>) {
									results.amount_won = p.amount_won;
									log(true_count_to_chart, results);
								}
							}, d);
						}
					}
				}, players);
			}

			gamington.reset();
		}
		TC_chart = std::move(true_count_to_chart);
		});

	std::map<int, std::array<chart, 3>> true_count_to_chart = combine_charts(TC_charts);

	
	// output charts to csv
	std::string buffer{};
	for (auto& [key, value] : true_count_to_chart) {
		for (int i = 0; i < 3; i++) {
			BlackJack::Action action = index_to_action(i);
			std::string action_str;
			auto ec = glz::write_json(action, action_str);
			action_str = action_str.substr(1, action_str.size() - 2);
			std::array<ev_column, 10> ev{};
			std::array<ev_column, 21> hard_ev{};

			for (int j = 0; j < 10; j++) {
				std::array<long long, 10> chart_vals = std::bit_cast<std::array<long long, 10>>(value[i].soft_chart[j]);
				std::array<long long, 10> chart_counts = std::bit_cast<std::array<long long, 10>>(value[i].soft_chart_counts[j]);
				std::array<double, 10> ev_vals{};

				constexpr long long precision = 1000;

				for (int k = 0; k < 10; k++) {
					if (chart_counts[i] > 0) {
						ev_vals[k] = double(chart_vals[k] * precision / chart_counts[i]);
						ev_vals[k] /= 10.0 * precision;
					}
				}

				ev[j] = std::bit_cast<ev_column>(ev_vals);
			}


			for (int j = 0; j < 21; j++) {
				std::array<long long, 10> chart_vals = std::bit_cast<std::array<long long, 10>>(value[i].hard_chart[j]);
				std::array<long long, 10> chart_counts = std::bit_cast<std::array<long long, 10>>(value[i].hard_chart_counts[j]);
				std::array<double, 10> ev_vals{};

				constexpr long long precision = 1000;

				for (int k = 0; k < 10; k++) {
					if (chart_counts[i] > 0) {
						ev_vals[k] = double(chart_vals[k] * precision / chart_counts[i]);
						ev_vals[k] /= 10.0 * precision;
					}
				}

				hard_ev[j] = std::bit_cast<ev_column>(ev_vals);
			}

			std::string file_name = "hard_chart_" + action_str + "_" + std::to_string(key) + ".csv";
			auto error = glz::write_file_csv(hard_ev, file_name, buffer);
			buffer.clear();
			if (error) {
				std::cout << glz::format_error(error.ec) << std::endl;
				std::cout << file_name << std::endl;
			}

			file_name = "hard_chart_counts_" + action_str + "_" + std::to_string(key) + ".csv";
			error = glz::write_file_csv(value[i].hard_chart_counts, file_name, buffer);
			buffer.clear();
			if (error) {
				std::cout << glz::format_error(error.ec) << std::endl;
				std::cout << file_name << std::endl;
			}
			
			file_name = "soft_chart_" + action_str + "_" + std::to_string(key) + ".csv";
			error = glz::write_file_csv(ev, file_name, buffer);
			buffer.clear();
		
			if (error) {
				std::cout << glz::format_error(error.ec) << std::endl;
				std::cout << file_name << std::endl;
			}

			file_name = "soft_chart_counts_" + action_str + "_" + std::to_string(key) + ".csv";
			error = glz::write_file_csv(value[i].soft_chart_counts, file_name, buffer);
			buffer.clear();
			if (error) {
				std::cout << glz::format_error(error.ec) << std::endl;
				std::cout << file_name << std::endl;
			}
		}
	}
}

#include <chrono>

int main() {
	auto before = std::chrono::high_resolution_clock::now();
	datagen();
	auto after = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double, std::ratio<1,1>> time_spent = after - before;

	std::cout << "this program finished in " << time_spent << std::endl;
	return 0;
}
