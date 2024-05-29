#include "ui/app/ChannelButton.hpp"
#include "tft.hpp"
#include "sampling/ChannelSelection.hpp"

namespace ui {

void ChannelButton::action()
{
	using namespace sampling;
	switch (channelSelection) {
		default:
		case ChannelSelection::None:
			channelSelection = ChannelSelection::OnlyFirst;
			break;
		case ChannelSelection::OnlyFirst:
			channelSelection = ChannelSelection::OnlySecond;
			break;
		case ChannelSelection::OnlySecond:
			channelSelection = ChannelSelection::BothSeparate;
			break;
		case ChannelSelection::BothSeparate:
			channelSelection = ChannelSelection::BothTogether;
			break;
		case ChannelSelection::BothTogether:
			channelSelection = ChannelSelection::OnlyFirst;
			break;
	}

	updateVoltageShifterInput();
}

void ChannelButton::onPressDown(uint16_t sx, uint16_t sy)
{
	(void) sx; (void) sy; // unused
	draw(defaultPressedBackgroundColor);
}
void ChannelButton::draw()
{
	draw(defaultBackgroundColor);
}

void ChannelButton::draw(color_t backgroundColor)
{
	tft.drawRect(x, y, w, h, defaultBorderColor);
	tft.fillRect(x + 1, y + 1, w - 2, h - 2, backgroundColor);

	tft.setTextDatum(TC_DATUM);
	tft.setTextColor(defaultTextColor, backgroundColor);
	tft.drawString("Wybor kanalu", x + w / 2, y + 2);
	// TODO: Polish ó and ł, maybe by overlapping: o' and -l, or custom font

	tft.setTextDatum(BC_DATUM);
	color_t c;

	c = sampling::channelSelection.first() ? channel1Color :  defaultPressedBackgroundColor;
	tft.setTextColor(c, backgroundColor);
	tft.drawString("CH1", x + w / 4, y + h - 1);

	c = sampling::channelSelection.together() ? defaultTextColor :  defaultPressedBackgroundColor;
	tft.setTextColor(c, backgroundColor);
	tft.drawString("+", x + w / 2, y + h - 1);

	c = sampling::channelSelection.second() ? channel2Color :  defaultPressedBackgroundColor;
	tft.setTextColor(c, backgroundColor);
	tft.drawString("CH2", x + w - w / 4, y + h - 1);
}

}
