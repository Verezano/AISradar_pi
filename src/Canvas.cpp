/******************************************************************************
 * $Id:   $
 *
 * Project:  OpenCPN
 * Purpose:  Clickable canvas to draw upon
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


//
// wxWidgets sends the mouse movement and mouse clicks to the windows objects
// In this case the panel object that acts as drawing canvas
// This helper class receives the events and informs the radar view object that owns
// the canvas
//

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
	event.Skip();
}


void Canvas::mouseDown(wxMouseEvent& event) {
	MouseDown=true;
	if (pv) {
		pv->OnLeftMouse(event.GetPosition());
	}
	event.Skip();
}


void Canvas::mouseReleased(wxMouseEvent& event) {
	MouseDown=false;
	event.Skip();
}
