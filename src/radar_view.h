/******************************************************************************
 * $Id: radar_view.h  $
 *
 * Project:  OpenCPN
 * Purpose:  Radar Plugin
 * Author:   Johan van der Sman
 *
 ***************************************************************************
 *   Copyright (C) 2011 by Johan van der Sman                              *
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
 */

#ifndef _RADARVIEW_H_
#define _RADARVIEW_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers


class RadarView : public wxScrolledWindow
{
      DECLARE_CLASS( Radar_view )
                   DECLARE_EVENT_TABLE()
public:
    RadarView(wxFrame* parent);
    
    void paintEvent(wxPaintEvent & evt);
    void paintNow();
    
    void render(wxDC& dc);
    
    // some useful events
    /*
     void mouseMoved(wxMouseEvent& event);
     void mouseDown(wxMouseEvent& event);
     void mouseWheelMoved(wxMouseEvent& event);
     void mouseReleased(wxMouseEvent& event);
     void rightClick(wxMouseEvent& event);
     void mouseLeftWindow(wxMouseEvent& event);
     void keyPressed(wxKeyEvent& event);
     void keyReleased(wxKeyEvent& event);
     */
    
private:
	wxFrame       *m_pParent;
};


#endif