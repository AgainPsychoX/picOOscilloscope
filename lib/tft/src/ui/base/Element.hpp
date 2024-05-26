#pragma once

namespace ui {

struct Element
{
	/// Preforms full draw of the element
	virtual void draw() = 0;

	/// Preforms update of the element, which might include partial draw.
	/// Defaults to doing nothing.
	virtual void update() {};
};

}
