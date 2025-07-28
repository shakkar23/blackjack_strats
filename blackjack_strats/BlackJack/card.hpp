#pragma once
#include <cstdint>

namespace BlackJack {
	enum class Suit : std::uint8_t {
		Hearts,
		Clubs,
		Diamonds,
		Spades
	};

	enum class Rank : std::uint8_t {
		Ace,
		Two,
		Three,
		Four,
		Five,
		Six,
		Seven,
		Eight,
		Nine,
		Ten,
		Jack,
		Queen,
		King
	};

	struct Card {
		Suit suit;
		Rank rank;
		Card(Suit s, Rank r) : suit(s), rank(r) {}
	};
};