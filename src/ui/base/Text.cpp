#include "ui/base/Text.hpp"
#include "ui/tft.hpp"

namespace ui {

void Text::draw()
{
	tft.drawString(text, x, y);
}

}
