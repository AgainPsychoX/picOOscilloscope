#include "ui/base/Text.hpp"
#include "tft.hpp"

namespace ui {

void Text::draw()
{
	tft.drawString(text, x, y);
}

}
