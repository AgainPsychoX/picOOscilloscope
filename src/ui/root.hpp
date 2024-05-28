#pragma once
#include <ui/common.hpp>
#include <ui/base/Group.hpp>
#include <vector>

namespace ui {

////////////////////////////////////////////////////////////////////////////////
// Top level menu

using RootGroup = Group<std::vector>;

extern RootGroup root;

}
