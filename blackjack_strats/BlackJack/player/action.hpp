#pragma once

namespace BlackJack {
	enum class Action : uint8_t {
		Hit,
		Stand,
		Surrender,
		DoubleDown,
		Split
	};
};