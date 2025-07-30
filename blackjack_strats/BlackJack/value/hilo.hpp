#pragma once

#include "../card.hpp"

#include <utility>

//hilo
namespace BlackJack {
	inline int hilo_value(const Card& card) noexcept {
		switch (card.rank) {
			case Rank::Two:   return 1;
			case Rank::Three: return 1;
			case Rank::Four:  return 1;
			case Rank::Five:  return 1;
			case Rank::Six:   return 1;
			case Rank::Seven: return 0;
			case Rank::Eight: return 0;
			case Rank::Nine:  return 0;
			case Rank::Ten:   return -1;
			case Rank::Jack:  return -1;
			case Rank::Queen: return -1;
			case Rank::King:  return -1;
			case Rank::Ace:   return -1;
		}
		std::unreachable();
	}
};