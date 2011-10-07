#include "Canvas.h"

BEGIN_EVENT_TABLE(Canvas, wxPanel)
 EVT_MOTION(Canvas::mouseMoved)
 EVT_LEFT_DOWN(Canvas::mouseDown)
 EVT_LEFT_UP(Canvas::mouseReleased) 
END_EVENT_TABLE()
 
Canvas::Canvas(wxWindow *parent, RadarFrame *view, 
	wxWindowID id, const wxPoint& pos, const wxSize& size) 
: wxPanel(parent, id, pos, size), pv(view), MouseDown(false)
{
}


void Canvas::mouseMoved(wxMouseEvent& event) {
	if (MouseDown) {
		if (pv) {
			pv->OnLeftMouse(event.GetPosition());
		}
	}
}


void Canvas::mouseDown(wxMouseEvent& event) {
	MouseDown=true;
	if (pv) {
		pv->OnLeftMouse(event.GetPosition());
	}
}


void Canvas::mouseReleased(wxMouseEvent& event) {
	MouseDown=false;
}




 