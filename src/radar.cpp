/******************************************************************************
 * $Id:   $
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
#include <wx/wfstream.h>
#include <wx/debug.h>
#include <wx/graphics.h>
#include <wx/fileconf.h>

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <minmax.h>

#include "radar_pi.h"
#include "icons.h"

static const double    RangeData[9] = {
	0.25, 0.5, 1, 2, 4, 8, 12, 16, 32
};

enum    Ids { cbRangeId = 10001,
				cbNorthUpId,
				tmRefreshId
};

static const int RESTART  = -1;
static const int BASE_STATION = 3;

//---------------------------------------------------------------------------------------
//          Radar Dialog Implementation
//---------------------------------------------------------------------------------------
IMPLEMENT_CLASS ( RadarFrame, wxDialog )


BEGIN_EVENT_TABLE ( RadarFrame, wxDialog )

            EVT_CLOSE    ( RadarFrame::OnClose )
            EVT_MOVE     ( RadarFrame::OnMove )
            EVT_SIZE     ( RadarFrame::OnSize )
			EVT_PAINT    ( RadarFrame::paintEvent)
			EVT_COMBOBOX ( cbRangeId, RadarFrame::OnRange)
			EVT_CHECKBOX ( cbNorthUpId, RadarFrame::OnNorthUp )
			EVT_TIMER    ( tmRefreshId, RadarFrame::OnTimer )

END_EVENT_TABLE()


RadarFrame::RadarFrame() 
: m_Width(0), m_Height(0), m_pCanvas(0), m_pRange(0), m_pNorthUp(0), m_Timer(0) {
      Init();
}

RadarFrame::~RadarFrame( ) {
}

void RadarFrame::Init() {
	GetGlobalColor(_T("DILG1"), &m_BgColour);
	SetBackgroundColour(m_BgColour);
}


bool RadarFrame::Create ( wxWindow *parent, radar_pi *ppi, wxWindowID id,
                              const wxString& caption, 
                              const wxPoint& pos, const wxSize& size )
{
    pParent = parent;
    pPlugIn = ppi;
    long wstyle = wxDEFAULT_FRAME_STYLE;
    wxSize size_min = size;
	if ( !wxDialog::Create ( parent, id, caption, pos, size_min, wstyle ) ) {
          return false;
	}

    // Get config object from parent
	m_pConfig = pPlugIn->GetConfig();

	// Add panel to contents of frame
	wxPanel *panel = new wxPanel(this, -1);
	panel->SetAutoLayout(true);
	wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(vbox);

	// Add Canvas panel to draw upon
	// Use the stored size provided by the pi
	// Create a square box based on the smallest side
    wxBoxSizer *canvas = new wxBoxSizer(wxHORIZONTAL);
	int tmp_size = max(min(size_min.GetWidth(),size_min.GetHeight()),MIN_RADIUS*2);
	size_min.Set(tmp_size, tmp_size);
    m_pCanvas = new wxPanel(panel, wxID_ANY, pos, size_min);
    canvas->Add(m_pCanvas, 1, wxEXPAND);
    vbox->Add(canvas, 1, wxALL | wxEXPAND, 5);
  
	// Add controls
	wxStaticBox    *sb=new wxStaticBox(panel,wxID_ANY, _("Controls"));
    wxStaticBoxSizer *controls = new wxStaticBoxSizer(sb, wxHORIZONTAL);
    wxStaticText *st1 = new wxStaticText(panel,wxID_ANY,_("Range"));
    controls->Add(st1,0,wxRIGHT,5);
	m_pRange = new wxComboBox(panel, cbRangeId, wxT(""));
	m_pRange->Append(wxT("0.25"));
	m_pRange->Append(wxT("0.5") );
	m_pRange->Append(wxT("1")   );
	m_pRange->Append(wxT("2")   );
	m_pRange->Append(wxT("4")   );
	m_pRange->Append(wxT("8")   );
	m_pRange->Append(wxT("12")  );
	m_pRange->Append(wxT("16")  );
	m_pRange->Append(wxT("32")  );
	m_pRange->SetSelection(pPlugIn->GetRadarRange());
    controls->Add(m_pRange);

    wxStaticText *st2 = new wxStaticText(panel,wxID_ANY,_("Mile"));
    controls->Add(st2,0,wxRIGHT|wxLEFT,5);

	m_pNorthUp = new wxCheckBox(panel, cbNorthUpId, _("North Up"));
	m_pNorthUp->SetValue(pPlugIn->GetRadarNorthUp());
    controls->Add(m_pNorthUp, 0, wxLEFT, 10);
    vbox->Add(controls, 0, wxEXPAND | wxALL, 5);

	// Add timer
	m_Timer = new wxTimer(this, tmRefreshId);
	m_Timer->Start(2000);

	vbox->Fit(this);

	return true;
}


void RadarFrame::SetColourScheme(PI_ColorScheme cs) {
	  GetGlobalColor(_T("DILG1"), &m_BgColour);
      SetBackgroundColour(m_BgColour);
      this->Refresh();
}

\
void RadarFrame::OnClose ( wxCloseEvent& event ) {
	// Stop timer if still running
	m_Timer->Stop();

	// Save settings 
	pPlugIn->SetRadarNorthUp(m_pNorthUp->GetValue());
	pPlugIn->SetRadarRange(m_pRange->GetSelection());

	// Cleanup
	RequestRefresh(pParent);
	Destroy();
	pPlugIn->OnRadarFrameClose();
}


void RadarFrame::OnRange ( wxCommandEvent& event ) {
	this->Refresh();
}


void RadarFrame::OnNorthUp ( wxCommandEvent& event ) {
	this->Refresh();
}


void RadarFrame::OnTimer( wxTimerEvent& event ) {
	this->Refresh();
}


void RadarFrame::OnMove ( wxMoveEvent& event ) {
      wxPoint p = event.GetPosition();
      pPlugIn->SetRadarFrameX(p.x);
      pPlugIn->SetRadarFrameY(p.y);
      event.Skip();
}

void RadarFrame::OnSize ( wxSizeEvent& event ) {
	  // Update Config to reflect new size
	  event.Skip();
	  wxClientDC dc(m_pCanvas);
	  dc.Clear();
	  render(dc);
}


void RadarFrame::paintEvent(wxPaintEvent & event) {
    event.Skip();
	wxPaintDC dc(m_pCanvas);
	dc.Clear();
    render(dc);
}


void RadarFrame::render(wxDC& dc)	 {
	m_Timer->Start(RESTART);

	// Store current size of drawing area
	int da_width  = dc.GetSize().GetWidth();
	int da_height = dc.GetSize().GetHeight();
    pPlugIn->SetRadarFrameSizeX(da_width);
    pPlugIn->SetRadarFrameSizeY(da_height);

	// Calculate the size based on paint area size, if smaller then the minimum
	// then the default minimum size applies
	int width  = max(da_width, (MIN_RADIUS)*2 );
	int height = max(da_height,(MIN_RADIUS)*2 );
	wxSize       size(width, height);
	wxPoint      center(width/2, height/2);
	int radius = max((min(width,height)/2),MIN_RADIUS);
	
	m_pCanvas->SetBackgroundColour (m_BgColour);
	renderRange(dc, center, size, radius);    
	renderBoats(dc, center, size, radius);
}


void RadarFrame::TrimAisField(wxString *fld) {
	assert(fld);
	while (fld->Right(1)=='@' || fld->Right(1)==' ') {
		fld->RemoveLast();
	}
}

void RadarFrame::renderBoats(wxDC &dc, wxPoint &center, wxSize &size, int radius) {
	// Determine orientation
	double mycog=0.;
	double offset=pPlugIn->GetCog();
	if (m_pNorthUp->GetValue()) {
		mycog=offset;
		offset=0;
	}

	// Get display settings
	bool   m_ShowMoored=true;
	double m_MooredSpeed=0.0;
	bool   m_ShowCogArrows=true;
	int    m_CogArrowMinutes=0;

	if (m_pConfig) {
		m_pConfig->SetPath ( _T( "/Settings/AIS" ) );
		m_pConfig->Read ( _T( "bShowMooredTargets" ),  &m_ShowMoored, 1 );
		m_pConfig->Read ( _T( "MooredTargetMaxSpeedKnots" ),  &m_MooredSpeed, 0.0 );
		m_pConfig->Read ( _T("bShowCOGArrows"), &m_ShowCogArrows, 1);
		m_pConfig->Read ( _T("CogArrowMinutes"), &m_CogArrowMinutes, 6);
	}

	// Show own boat
    Target Self;
	Self.SetCanvas(center, radius, m_BgColour);
	Self.SetNavDetails(RangeData[m_pRange->GetSelection()], 0., m_ShowCogArrows, m_CogArrowMinutes);
	Self.SetState(0, wxT("Us"), 0., 0., mycog, pPlugIn->GetSog(), 1, PI_AIS_NO_ALARM, 0 );
	Self.Render(dc);

	// Show other boats and base stations
	Target    dt;
	ArrayOfPlugIn_AIS_Targets *AisTargets = pPlugIn->GetAisTargets();
	PlugIn_AIS_Target *t;
	ArrayOfPlugIn_AIS_Targets::iterator it;
	wxString  Name;
	wxString  CallSign;

	// Set generic details for all targets
	dt.SetCanvas(center,radius, m_BgColour);
	dt.SetNavDetails(RangeData[m_pRange->GetSelection()], offset, m_ShowCogArrows, m_CogArrowMinutes);

	for( it = (*AisTargets).begin(); it != (*AisTargets).end(); ++it ) {
		t        = *it;
		// Only display well defined targets
		if (t->Range_NM>0.0 && t->Brg>0.0) {
			if (m_ShowMoored 
				|| t->Class == BASE_STATION
				||(!m_ShowMoored && t->SOG > m_MooredSpeed)
			) {
				wxLogMessage(wxString::Format(_T("target rotation = %d"),t->ROTAIS));
				Name     = wxString::From8BitData(t->ShipName);
				CallSign = wxString::From8BitData(t->CallSign);
				TrimAisField(&Name);
				TrimAisField(&CallSign);
				dt.SetState(t->MMSI, Name, t->Range_NM, t->Brg, t->COG, t->SOG, 
					t->Class, t->alarm_state, t->ROTAIS
				);
				dt.Render(dc);
			}
		}
	}
}


void RadarFrame::renderRange(wxDC& dc, wxPoint &center, wxSize &size, int radius) {
	// Draw the circles
	dc.SetBackground(wxBrush(m_BgColour));
	dc.Clear();
	dc.SetBrush(wxBrush(wxColour(0,0,0),wxTRANSPARENT));
	dc.SetPen( wxPen( wxColor(128,128,128), 1, wxSOLID ) );
    dc.DrawCircle( center, radius );
	dc.SetPen( wxPen( wxColor(128,128,128), 1, wxDOT ) );
    dc.DrawCircle( center, radius*0.75 );
    dc.DrawCircle( center, radius*0.50 );
    dc.DrawCircle( center, radius*0.25 );
	dc.SetPen( wxPen( wxColor(128,128,128), 2, wxSOLID ) );
	dc.DrawCircle( center, 10 );

	// Draw the crosshairs
	dc.SetPen( wxPen( wxColor(128,128,128), 1, wxDOT ) );
	dc.DrawLine( size.GetWidth()/2,0, size.GetWidth()/2, size.GetHeight());
	dc.DrawLine( 0,size.GetHeight()/2, size.GetWidth(), size.GetHeight()/2);

	// Draw the range description
	wxFont fnt = dc.GetFont();
	fnt.SetPointSize(14);
	int fh=fnt.GetPointSize();
	dc.SetFont(fnt);
	int sel=m_pRange->GetSelection();
	float Range=RangeData[m_pRange->GetSelection()];
	dc.DrawText(wxString::Format(wxT("%-7.7s %2.2f"), _("Range"),Range  ), 0, 0); 
	dc.DrawText(wxString::Format(wxT("%-8.8s %2.2f"), _("Ring "), Range/4), 0, fh+TEXT_MARGIN); 

	// Draw the orientation info
	if (m_pNorthUp->GetValue()) {
		dc.DrawText(_("North Up"),  size.GetWidth()-dc.GetCharWidth()*11, 0); 
	} else {
		dc.DrawText(_("Course Up"), size.GetWidth()-dc.GetCharWidth()*11, 0); 
	}
}

