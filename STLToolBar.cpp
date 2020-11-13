/*  STLover - Simple STL Viewer
 *  Copyright (C) 2020 Gerasim Troeglazov <3dEyes@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <ControlLook.h>

#include "STLToolBar.h"

STLToolBar::STLToolBar(BRect rect) : BToolBar(rect, B_HORIZONTAL)
{
	GroupLayout()->SetInsets(0.0f, 0.0f, B_USE_HALF_ITEM_INSETS, 1.0f);
	SetFlags(Flags() | B_WILL_DRAW);
	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
}

void
STLToolBar::Draw(BRect updateRect)
{
	BRect rect = Bounds();
	rgb_color base = LowColor();
	be_control_look->DrawBorder(this, rect, updateRect, base, B_PLAIN_BORDER, 0, BControlLook::B_BOTTOM_BORDER);
	BToolBar::Draw(rect & updateRect);
}
