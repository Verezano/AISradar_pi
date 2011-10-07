#include "radar.h"
 
class Canvas : public wxPanel {
public:
	Canvas(wxWindow *parent, RadarFrame *view, wxWindowID id, const wxPoint& pos, const wxSize& size); 
    void mouseMoved(wxMouseEvent& event);
    void mouseDown(wxMouseEvent& event);
    void mouseReleased(wxMouseEvent& event);
    DECLARE_EVENT_TABLE()

private:
	RadarFrame	*pv;
	bool         MouseDown;
};
 




 
