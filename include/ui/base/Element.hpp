#pragma once

namespace ui {

struct Element
{
	/// Preforms full draw of the element
	virtual void draw() = 0;

	/// Preforms partial draw of the element, updating small part at the time,
	/// without necessarily redrawing unaffected areas. Defaults to `draw()`.
	virtual void partialDraw()
	{
		draw();
	}
};

}
