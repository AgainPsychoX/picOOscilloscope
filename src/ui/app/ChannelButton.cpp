#include "ui/app/ChannelButton.hpp"
#include "ui/tft.hpp"
#include "sampling.hpp"

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

void ChannelButton::onPressDown(uint16_t hx, uint16_t hy)
{
	(void) hx; (void) hy; // unused
	render(TFT_DARKGREY);
}
void ChannelButton::render()
{
	render(TFT_BLACK);
}

void ChannelButton::render(color_t backgroundColor)
{
	tft.drawRect(x, y, w, h, TFT_LIGHTGREY);
	tft.fillRect(x + 1, y + 1, w - 2, h - 2, backgroundColor);

	tft.setTextDatum(TC_DATUM);
	tft.setTextColor(TFT_WHITE, backgroundColor);
	tft.drawString("Wybor kanalu", x + w / 2, y + 2);
	// TODO: Polish ó and ł, maybe by overlapping: o' and -l, or custom font

	tft.setTextDatum(BC_DATUM);
	color_t c;

	c = sampling::channelSelection.first() ? channel1Color :  TFT_DARKGREY;
	tft.setTextColor(c, backgroundColor);
	tft.drawString("CH1", x + w / 4, y + h - 1);

	c = sampling::channelSelection.together() ? TFT_WHITE :  TFT_DARKGREY;
	tft.setTextColor(c, backgroundColor);
	tft.drawString("+", x + w / 2, y + h - 1);

	c = sampling::channelSelection.second() ? channel2Color :  TFT_DARKGREY;
	tft.setTextColor(c, backgroundColor);
	tft.drawString("CH2", x + w - w / 4, y + h - 1);
}

}
