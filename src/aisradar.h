/******************************************************************************
 * $Id:  $
 *
 * Project:  OpenCPN
 * Purpose:  Radar Plugin 
 * Author:   Johan van der Sman
 *
 ***************************************************************************
 *   Copyright (C) 2013 Johan van der Sman                                 *
 *   johan.sman@gmail.com                                                  *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************
 */

#ifndef _RADAR_H_
#define _RADAR_H_

#include "wx/wxprec.h"
#include "wx/dcbuffer.h"
#include "Target.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#define ID_OK                       10001
#define MIN_RADIUS                  150
#define TEXT_MARGIN                 5
#define SPACER_MARGIN               5
#ifdef WIN32
    #define   MyFit(a)    Fit(a)
#else
    #define   MyFit(a)    FitInside(a)
#endif

class aisradar_pi;

class ViewState {
public:
    ViewState (const wxPoint& p, const wxSize& s) : Pos(p), Size(s) {};
    wxPoint  GetPos() { return Pos; }
    wxSize    GetWindowSize() { 
#ifdef WIN32
        return wxDefaultSize;
#else
        return Size; 
#endif
}
    wxSize   GetCanvasSize() {
#ifdef WIN32
        return Size;
#else
        return wxDefaultSize;
#endif
    }
    wxSize   GetSize() { return Size; }
    void     SetPos(const wxPoint& p) { Pos=p; }
    void     SetWindowSize(const wxSize& s) { 
#ifndef WIN32
        Size=s; 
#endif
    }
    void     SetCanvasSize(const wxSize& s) { 
#ifdef WIN32
        Size=s; 
#endif
    }

private:
    wxPoint    Pos;
    wxSize    Size;
};

//----------------------------------------------------------------------------------------------------------
//    RADAR View Specification
//----------------------------------------------------------------------------------------------------------
class RadarFrame: public wxDialog
{
    DECLARE_CLASS( RadarFrame )
    DECLARE_EVENT_TABLE()

public:
    RadarFrame( );
    ~RadarFrame( );
    void Init();
    void Config(int min_radius);
    bool Create(  wxWindow *parent, aisradar_pi *ppi, wxWindowID id = wxID_ANY,
    const wxString& caption = _("Radar Display"), 
    const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxDefaultSize );
    void SetColourScheme(PI_ColorScheme cs);
    void OnLeftMouse( const wxPoint &curpos );
    void paintEvent( wxPaintEvent& event );

private:
    void OnClose(wxCloseEvent& event);
    void OnRange( wxCommandEvent& event );
    void OnNorthUp( wxCommandEvent& event );
    void OnBearingLine( wxCommandEvent& event );
    void OnTimer( wxTimerEvent& event );
    void OnMove( wxMoveEvent& event );
    void OnSize( wxSizeEvent& event );
    void render( wxDC& dc );
    void renderRange( wxDC& dc, wxPoint &center, wxSize &size, int radius );
    void renderBoats(wxDC& dc, wxPoint &center, wxSize &size, int radius);
    void TrimAisField(wxString *fld);
    
    //    Data
    wxWindow               *pParent;
    aisradar_pi            *pPlugIn;
    wxTimer                *m_Timer;
    wxPanel                *m_pCanvas;
    wxCheckBox             *m_pNorthUp;
    wxComboBox             *m_pRange;
    wxCheckBox             *m_pBearingLine;
    wxColor                 m_BgColour;
    double                  m_Ebl;
    int                     m_Range;
    ViewState              *m_pViewState;
};

#endif
