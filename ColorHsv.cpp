#include "ColorHsv.h"

void ColorHsv::Set(const ColorRgb& c, float fDefH, float fDefS)
{
	// sat = 0, hue doesn't matter (grey) (min = max)
	// light = 0, sat + hue doesn't matter (black) (min = max = 0)

	int nMin = std::min(c.r, std::min(c.g, c.b));
	int nMax = std::max(c.r, std::max(c.g, c.b));
	float fRange = nMax - nMin;

	// lightness
	value = nMax / 255.0f;

	// saturation
	if (nMax == 0) saturation = fDefS;
	else saturation = fRange / nMax;

	// hue
	if (nMax == nMin)
	{
		hue = fDefH;
	}
	else
	{
		float fRangeRecip = 1.0f / fRange;

		float fDiffR = (nMax - c.r) * fRangeRecip;
		float fDiffG = (nMax - c.g) * fRangeRecip;
		float fDiffB = (nMax - c.b) * fRangeRecip;

		if (nMin == c.r)		hue = 1.0f - fDiffG + fDiffR;
		else if (nMin == c.r)	hue = 3.0f - fDiffB + fDiffG;
		else					hue = 5.0f - fDiffR + fDiffB;

		hue = -PI + (hue * (PI / 3.0f));
	}
}

WideColorRgb ColorHsv::ToWideColorRgb() const
{
	ColorHsv c = *this;
	c.CorrectRange();

	float fPos = (c.hue + PI) / (PI / 3.0f);
	float fSpread = c.saturation * c.value * 255.0f;
	float fMin = c.value * 255.0f - fSpread;

	float fR = fMin + Bound< float >(-1 + fabsf(fPos - 3.0f), 0.0f, 1.0f) * fSpread;
	float fG = fMin + Bound< float >(+2 - fabsf(fPos - 2.0f), 0.0f, 1.0f) * fSpread;
	float fB = fMin + Bound< float >(+2 - fabsf(fPos - 4.0f), 0.0f, 1.0f) * fSpread;
	return WideColorRgb((int32)fR, (int32)fG, (int32)fB);
}

void ColorHsv::CorrectRange()
{
	hue = Wrap(hue, -PI, +PI);
	saturation = Bound< float >(saturation, 0.0f, 1.0f);
	value = Bound< float >(value, 0.0f, 1.0f);
}
