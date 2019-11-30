/*
 * Copyright (c) 2019 Dave French <contact/dot/dave/dot/french3/at/googlemail/dot/com>
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */

#include "SspoLookAndFeel.h"

SspoLookAndFeel::SspoLookAndFeel () : LookAndFeel_V4 ()
{
	setColour (Slider::thumbColourId, Colours::red);
}

void SspoLookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
	const auto radius = jmin (width / 2, height / 2) - 4.0f;
	const auto centreX = x + width * 0.5f;
	const auto centreY = y + height * 0.5f;
	const auto rx = centreX - radius * 0.7f;
	const auto ry = centreY - radius * 0.7f;
	const auto rw = radius * 1.4f;
	const auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

	//draw markings
	const auto markLength = radius * 0.1f;
	constexpr auto markThickness = 1.0f;
	const auto divisionRotation = (rotaryEndAngle - rotaryStartAngle) * 0.1f;
	g.setColour (Colours::whitesmoke);
	for (auto i = 0; i <= 10; ++i)
	{
		Path markPath;
		markPath.addRectangle (-markThickness * 0.5f, -radius, markThickness, markLength);
		markPath.applyTransform (AffineTransform::rotation (rotaryStartAngle + divisionRotation * i).translated (centreX, centreY));
		g.fillPath (markPath);
	}


	//draw knob
	g.setColour (slider.isEnabled () ? Colours::antiquewhite : Colours::grey);
	g.fillEllipse (rx, ry, rw, rw);

	//draw outline
	g.setColour (Colours::black);
	g.drawEllipse (rx, ry, rw, rw, rw * 0.1f);

	if (slider.isEnabled ())
	{
		//create the pointer
		Path linePath;
		Path triPath;
		const auto pointerLength = radius * 0.3f;
		constexpr auto pointerThickness = 2.0f;
		linePath.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
		triPath.addTriangle (0, -radius, pointerLength, -radius + pointerLength, -pointerLength, -radius + pointerLength);
		//rotate pointer
		linePath.applyTransform (AffineTransform::rotation (angle).translated (centreX, centreY));
		triPath.applyTransform (AffineTransform::rotation (angle).translated (centreX, centreY));
		g.setColour (Colours::black);
		g.fillPath (triPath);
		g.setColour (Colours::antiquewhite);
		g.fillPath (linePath);
	}
}

Label* SspoLookAndFeel::createSliderTextBox (Slider& slider)
{
	auto l = LookAndFeel_V4::createSliderTextBox (slider);

	if (l)
	{
		l->setColour (Label::outlineWhenEditingColourId, Colours::transparentBlack);
		l->setColour (Label::outlineColourId, Colours::transparentBlack);
	}
	return l;
}