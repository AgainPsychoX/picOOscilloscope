#pragma once
#include <cstdint>

namespace sampling {

/// Class-encapsulated enum for channel selection.
class ChannelSelection
{
public:
	enum Value : uint8_t
	{
		None            = 0b00000,
		OnlyFirst       = 0b00001,
		OnlySecond      = 0b00010,
		BothSeparate    = 0b00011,
		BothTogether    = 0b10011
	};

	constexpr bool first() const { return value & OnlyFirst; }
	constexpr bool second() const { return value & OnlySecond; }
	constexpr bool single() const { return first() ^ second(); }
	constexpr bool both() const { return value & BothSeparate; }
	constexpr bool together() const { return value & 0b10000; }

	ChannelSelection() = default;
	constexpr ChannelSelection(Value value) : value(value) {}
	constexpr operator Value() const { return value; }
	explicit operator bool() const = delete;
private:
	Value value;
};

/// Channel selection for the sampling.
/// Some changes take effect after (re)initialisation the sampling module.
extern ChannelSelection channelSelection;

}
