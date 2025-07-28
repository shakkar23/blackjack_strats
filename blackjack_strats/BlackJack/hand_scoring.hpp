#pragma once

#include "card_scoring.hpp"
#include "hand.hpp"

#include <optional>

namespace BlackJack
{
	int score_hand(const Hand& hand) noexcept
	{
		int score = 0;
		int aces = 0;
		for (const auto& card : hand.getCards())
		{
			int card_score = score_card(card);
			score += card_score;
			if (card.rank == Rank::Ace) {
				aces++;
			}
		}
		while (score > 21 && aces > 0)
		{
			score -= 10; // Convert Ace from 11 to 1
			aces--;
		}
		return score;
	}
};