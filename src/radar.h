
/******************************************************************************
 * $Id:  $
 *
 * Project:  OpenCPN
 * Purpose:  Radar Plugin 
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
 */

#ifndef _RADAR_H_
#define _RADAR_H_


#include "wx/wxprec.h"
#include "Target.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#define ID_OK                       10001
#define MIN_RADIUS                  150
#define TEXT_MARGIN                 5
#define SPACER_MARGIN               5

class radar_pi;


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
		bool Create(  wxWindow *parent, radar_pi *ppi, wxWindowID id = wxID_ANY,
			const wxString& caption = _("Radar Display"), 
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize );
		void SetColourScheme(PI_ColorScheme cs);

private:
		    void OnClose(wxCloseEvent& event);
            void OnRange( wxCommandEvent& event );
            void OnNorthUp( wxCommandEvent& event );
            void OnTimer( wxTimerEvent& event );
            void OnMove( wxMoveEvent& event );
            void OnSize( wxSizeEvent& event );
			void paintEvent( wxPaintEvent& event );
			void render( wxDC& dc );
			void renderRange( wxDC& dc, wxPoint &center, wxSize &size, int radius );
			void renderBoats(wxDC &dc, wxPoint &center, wxSize &size, int radius);
			void TrimAisField(wxString *fld);

            //    Data
            wxWindow          *pParent;
            radar_pi          *pPlugIn;
			wxTimer           *m_Timer;
			wxFileConfig      *m_pConfig;
			wxPanel           *m_pCanvas;
			wxCheckBox        *m_pNorthUp;
			wxComboBox        *m_pRange;
			wxColor            m_BgColour;
			int                m_Width;
			int                m_Height;
			int                m_Range;
};


#endif

