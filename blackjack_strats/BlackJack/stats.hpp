#pragma once

#include "value/hilo.hpp"
#include "card.hpp"


namespace BlackJack {
	struct statistics {
	public:
		void process_card(const Card& card) noexcept {
			hilo_running_count += hilo_value(card);
			cards_counted++;
		}

		void reset() noexcept {
			hilo_running_count = 0;
			cards_counted = 0;
		}

		void increment_rounds() noexcept {
			rounds_played++;
		}

		[[nodiscard]] int rounds_played_count() const noexcept {
			return rounds_played;
		}

		[[nodiscard]] int hilo_count() const noexcept {
			return hilo_running_count;
		}
		[[nodiscard]] int cards_counted_so_far() const noexcept {
			return cards_counted;
		}
	private:
		// hilo count
		int hilo_running_count = 0;
		int cards_counted = 0;
		int rounds_played = 0;
	};
};
