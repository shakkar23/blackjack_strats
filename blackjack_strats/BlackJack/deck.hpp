#pragma once

#include "card.hpp"

#include <vector>
#include <expected>
#include <utility>
#include <string>
#include <random>
#include <algorithm>

namespace BlackJack {
	class Deck {
	public:
		template <typename T>
		inline void shuffle(T& rng) noexcept {
			std::ranges::shuffle(cards, rng);
		}

		inline void empty() noexcept {
			cards.clear();
		}

		inline void new_shoe(int num_decks = 1) noexcept {
			empty();
			cards.reserve(num_decks * 52); // 52 cards per deck
			for (int i = 0; i < num_decks; ++i)
				for (int suit = static_cast<int>(Suit::Hearts); suit <= static_cast<int>(Suit::Spades); ++suit)
					for (int rank = static_cast<int>(Rank::Ace); rank <= static_cast<int>(Rank::King); ++rank)
						cards.emplace_back(static_cast<Suit>(suit), static_cast<Rank>(rank));
		}

		inline [[nodiscard]] const std::size_t num_cards() const noexcept {
			return cards.size();
		}

		inline std::expected<Card, const char*> draw() noexcept {
			if (cards.empty()) {
				return std::unexpected("No cards left in the deck");
			}
			Card card = cards.back();
			cards.pop_back();
			return card;
		}
	private:
		std::vector<Card> cards;
	};
};