#pragma once

#include "card.hpp"

namespace BlackJack {
	// Function to score a single card
	inline int score_card(const Card& card) {
		switch (card.rank)
		{
		case Rank::Ace: return 11;
		case Rank::Two: return 2;
		case Rank::Three: return 3;
		case Rank::Four: return 4;
		case Rank::Five: return 5;
		case Rank::Six: return 6;
		case Rank::Seven: return 7;
		case Rank::Eight: return 8;
		case Rank::Nine: return 9;
		case Rank::Ten: return 10;
		case Rank::Jack: return 10;
		case Rank::Queen: return 10;
		case Rank::King: return 10;
		}
		throw std::invalid_argument("Invalid card rank");
	}
};