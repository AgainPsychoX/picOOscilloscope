#include "ui/base/Text.hpp"
#include "ui/tft.hpp"

namespace ui {

void Text::render()
{
	tft.drawString(text, x, y);
}

}
