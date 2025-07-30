#pragma once

#include "card.hpp"
#include "value/card_scoring.hpp"

#include <vector>
#include <expected>

namespace BlackJack {
	class Hand {
	public:
		inline void addCard(const Card& card) noexcept {
			cards.push_back(card);
		}

		inline [[nodiscard]] std::vector<Card> getCards() noexcept {
			return std::vector<Card>(cards);
		}

		inline void clear() noexcept {
			cards.clear();
		}

		inline [[nodiscard]] std::size_t size() const noexcept {
			return cards.size();
		}

		inline [[nodiscard]] const std::vector<Card>& getCards() const noexcept {
			return cards;
		}

		inline bool can_split() const noexcept {
			return cards.size() == 2 && BlackJack::score_card(cards[0]) == BlackJack::score_card(cards[1]);
		}

		inline [[nodiscard]] std::expected<Hand, const char*> split() noexcept {
			if (cards.size() != 2) {
				return std::unexpected("Cannot split a hand with less than 2 cards");
			}
			Hand splitHand;
			Card splitCard = cards.back();
			splitHand.addCard(splitCard);
			cards.pop_back();
			return splitHand;
		}

		inline [[nodiscard]] const Card& operator[](const std::size_t index) const noexcept {
			return cards[index];
		}

		inline [[nodiscard]] bool is_soft() const noexcept {
			int score = 0;
			int aces = 0;
			
			for (const auto& card : cards) {
				int card_score = score_card(card);
				score += card_score;
				if (card.rank == Rank::Ace) {
					aces++;
				}
			}

			while (score > 21 && aces > 0) {
				score -= 10; // Convert Ace from 11 to 1
				aces--;
			}

			return aces > 0;
		}

	private:
		std::vector<Card> cards;
		friend struct glz::meta<BlackJack::Hand>;
	};
};