#ifndef __DFXGUI_SLIDER_H
#define __DFXGUI_SLIDER_H


#include "dfxgui.h"


typedef enum {
	kDGSliderStyle_vertical,
	kDGSliderStyle_horizontal
} DfxGuiSliderStyle;



//-----------------------------------------------------------------------------
class DGSlider : public DGControl
{
public:
	DGSlider(DfxGuiEditor*, AudioUnitParameterID, DGRect*, DfxGuiSliderStyle, DGGraphic*, DGGraphic*);
	virtual ~DGSlider();

	virtual void draw(CGContextRef context, UInt32 portHeight);
	virtual void mouseDown(Point inPos, bool, bool);
	virtual void mouseTrack(Point inPos, bool, bool);
	virtual void mouseUp(Point inPos, bool, bool);

private:
	UInt32			orientation;
	DGGraphic *		ForeGround;
	DGGraphic *		BackGround;
	float			fineTuneFactor;	// slow-down factor for shift control
	long			mouseOffset;	// for mouse tracking with click in the middle of the slider handle
	SInt32			sldr_X;
	SInt32			sldr_Y;
};



#endif