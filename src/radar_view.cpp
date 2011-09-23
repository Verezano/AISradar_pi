/******************************************************************************
 *
 *
 * Project:  OpenCPN
 * Purpose:  RadarView Object
 * Author:   Johan van der Sman
 *
 ***************************************************************************
 *   Copyright (C) 2011 Johan van der Sman                                 *
 *   hannes@andcrew.nl                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************
 *
 */

#include "wx/wx.h"
#include "radar_view.h"


//---------------------------------------------------------------------------------------
//          Radar view Implementation
//---------------------------------------------------------------------------------------
IMPLEMENT_CLASS ( RadarView, wxPanel )

BEGIN_EVENT_TABLE ( RadarView, wxPanel )

	EVT_PAINT(RadarView::paintEvent)
 
END_EVENT_TABLE()
 
 
// some useful events
/*
 void RadarView::mouseMoved(wxMouseEvent& event) {}
 void RadarView::mouseDown(wxMouseEvent& event) {}
 void RadarView::mouseWheelMoved(wxMouseEvent& event) {}
 void RadarView::mouseReleased(wxMouseEvent& event) {}
 void RadarView::rightClick(wxMouseEvent& event) {}
 void RadarView::mouseLeftWindow(wxMouseEvent& event) {}
 void RadarView::keyPressed(wxKeyEvent& event) {}
 void RadarView::keyReleased(wxKeyEvent& event) {}
 */
 
RadarView::RadarView(wxFrame* parent) : 
  wxScrolledWindow(parent, wxID_ANY), m_pParent(parent)
{
	wxLogMessage(_T("RadarView::RadarView"));
    SetBackgroundColour (wxColour (255,255,255));
    ClearBackground();
}
 
/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */
 
void RadarView::paintEvent(wxPaintEvent & evt) {
	wxLogMessage(_T("RadarView::paintEvent"));
	wxPaintDC dc(this);
    render(dc);
}
 
/*
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 *
 * In most cases, this will not be needed at all; simply handling
 * paint events and calling Refresh() when a refresh is needed
 * will do the job.
 */
void RadarView::paintNow() {
	wxLogMessage(_T("RadarView::paintNow"));
    wxClientDC dc(this);
    render(dc);
}
 
/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void RadarView::render(wxDC&  dc) {
    wxLogMessage(_T("RadarView::render"));

    // draw some text
    dc.DrawText(wxT("Testing"), 40, 60); 
    
    // draw a circle
    dc.SetBrush(*wxGREEN_BRUSH); // green filling
    dc.SetPen( wxPen( wxColor(255,0,0), 5 ) ); // 5-pixels-thick red outline
    dc.DrawCircle( wxPoint(200,100), 25 /* radius */ );
    
    // draw a rectangle
    dc.SetBrush(*wxBLUE_BRUSH); // blue filling
    dc.SetPen( wxPen( wxColor(255,175,175), 10 ) ); // 10-pixels-thick pink outline
    dc.DrawRectangle( 300, 100, 400, 200 );
    
    // draw a line
    dc.SetPen( wxPen( wxColor(0,0,0), 3 ) ); // black line, 3 pixels thick
    dc.DrawLine( 300, 100, 700, 300 ); // draw line across the rectangle
    
    // Look at the wxDC docs to learn how to draw other stuff
}