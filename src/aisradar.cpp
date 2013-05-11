/******************************************************************************
 * $Id:   $
 *
 * Project:  OpenCPN
 * Purpose:  RadarView Object
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
 *
 */
#include "wx/wx.h"
#include <wx/debug.h>
#include <wx/fileconf.h>
#include <math.h>
#define min(a,b)  ( (a>b)? b : a)
#define max(a,b)  ( (a>b)? a : b)
#include "aisradar_pi.h"
#include "Canvas.h"

static const double    RangeData[9] = {
    0.25, 0.5, 1, 2, 4, 8, 12, 16, 32
};

enum    Ids { cbRangeId = 10001,
                cbNorthUpId,
                cbBearingLineId,
                tmRefreshId,
                plCanvasId,
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
    EVT_CHECKBOX ( cbBearingLineId, RadarFrame::OnBearingLine )
    EVT_TIMER    ( tmRefreshId, RadarFrame::OnTimer )

END_EVENT_TABLE()

RadarFrame::RadarFrame() 
: pParent(0), 
    pPlugIn(0), 
    m_Timer(0), 
    m_pCanvas(0), 
    m_pNorthUp(0), 
    m_pRange(0),
    m_pBearingLine(0), 
    m_BgColour(),
    m_Ebl(0.0),  
    m_Range(0), 
    m_pViewState(0)
{
    Init();
}

RadarFrame::~RadarFrame( ) {
}


void RadarFrame::Init() {
    GetGlobalColor(_T("DILG1"), &m_BgColour);
    SetBackgroundColour(m_BgColour);
}


bool RadarFrame::Create ( wxWindow *parent, aisradar_pi *ppi, wxWindowID id,
                              const wxString& caption, 
                              const wxPoint& pos, const wxSize& size )
{
    pParent = parent;
    pPlugIn = ppi;
    long wstyle = wxDEFAULT_FRAME_STYLE;
    m_pViewState = new ViewState(pos, size);
    if ( !wxDialog::Create ( parent, id, caption, pos, m_pViewState->GetWindowSize(), wstyle ) ) {
        return false;
    }

    // Add panel to contents of frame
    wxPanel *panel = new wxPanel(this, plCanvasId );
    panel->SetAutoLayout(true);
    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(vbox);

    // Add Canvas panel to draw upon
    // Use the stored size provided by the pi
    // Create a square box based on the smallest side
    wxBoxSizer *canvas = new wxBoxSizer(wxHORIZONTAL);
    m_pCanvas = new Canvas(panel, this, wxID_ANY, pos, m_pViewState->GetCanvasSize());
    m_pCanvas->SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    wxBoxSizer *cbox = new wxBoxSizer(wxVERTICAL);
    cbox->FitInside(m_pCanvas);
    canvas->Add(m_pCanvas, 1, wxEXPAND);
    vbox->Add(canvas, 1, wxALL | wxEXPAND, 5);
  
    // Add controls
    wxStaticBox    *sb=new wxStaticBox(panel,wxID_ANY, _("Options"));
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

    wxStaticText *st2 = new wxStaticText(panel,wxID_ANY,_("Nautical Miles"));
    controls->Add(st2,0,wxRIGHT|wxLEFT,5);

    m_pNorthUp = new wxCheckBox(panel, cbNorthUpId, _("North Up"));
    m_pNorthUp->SetValue(pPlugIn->GetRadarNorthUp());
    controls->Add(m_pNorthUp, 0, wxLEFT, 10);

    m_pBearingLine = new wxCheckBox(panel, cbBearingLineId, _("EBL"));
    m_pBearingLine->SetValue(false);
    controls->Add(m_pBearingLine, 0, wxLEFT, 10);
    vbox->Add(controls, 0, wxEXPAND | wxALL, 5);

    // Add timer
    m_Timer = new wxTimer(this, tmRefreshId);
    m_Timer->Start(2000);

    vbox->MyFit(this);

    return true;
}


void RadarFrame::SetColourScheme(PI_ColorScheme cs) {
      GetGlobalColor(_T("DILG1"), &m_BgColour);
      SetBackgroundColour(m_BgColour);
      this->Refresh();
}


void RadarFrame::OnClose ( wxCloseEvent& event ) {
    // Stop timer if still running
    m_Timer->Stop();

    // Save window size
    pPlugIn->SetRadarFrameX(m_pViewState->GetPos().x);
    pPlugIn->SetRadarFrameY(m_pViewState->GetPos().y);
    pPlugIn->SetRadarFrameSizeX(m_pViewState->GetSize().GetWidth());
    pPlugIn->SetRadarFrameSizeY(m_pViewState->GetSize().GetHeight());

    // Cleanup
    RequestRefresh(pParent);
    Destroy();
    pPlugIn->OnRadarFrameClose();
}


void RadarFrame::OnRange ( wxCommandEvent& event ) {
    pPlugIn->SetRadarRange(m_pRange->GetSelection());
    this->Refresh();
}



void RadarFrame::OnNorthUp ( wxCommandEvent& event ) {
    pPlugIn->SetRadarNorthUp(m_pNorthUp->GetValue());
    if (m_pNorthUp->GetValue()) {
        m_Ebl += pPlugIn->GetCog();
    } else {
        m_Ebl -= pPlugIn->GetCog();
    }
    this->Refresh();
}


void RadarFrame::OnTimer( wxTimerEvent& event ) {
    this->Refresh();
}


void RadarFrame::OnBearingLine( wxCommandEvent& event ) {
    this->Refresh();
}


void RadarFrame::OnLeftMouse(const wxPoint &curpos) {
    if (m_pBearingLine->GetValue()) {
        int width      = max(m_pCanvas->GetSize().GetWidth(), (MIN_RADIUS)*2 );
        int height     = max(m_pCanvas->GetSize().GetHeight(),(MIN_RADIUS)*2 );
        wxPoint center(width/2, height/2);
        int dx = curpos.x - center.x;
        int dy = center.y - curpos.y;    // top of screen y=0
        double tmpradius = sqrt((double)(dx*dx)+(double)(dy*dy));
        double angle= dy/tmpradius;
        m_Ebl = asin(angle)*(double)((double)180./(double)3.141592653589);
        if ( dx >= 0 ) {
            m_Ebl = 90 - m_Ebl;
        } else {
            m_Ebl = 360 - (90 - m_Ebl);
        }
        this->Refresh();
    }
}


void RadarFrame::OnMove ( wxMoveEvent& event ) {
    wxPoint p = event.GetPosition();

    // Save window position
    m_pViewState->SetPos(wxPoint(p.x, p.y));
    event.Skip();
}


void RadarFrame::OnSize ( wxSizeEvent& event ) {
    event.Skip();
    wxClientDC dc(m_pCanvas);
    m_pViewState->SetCanvasSize(dc.GetSize());
    m_pViewState->SetWindowSize(GetSize());
    render(dc);
}


void RadarFrame::paintEvent(wxPaintEvent & event) {
    wxAutoBufferedPaintDC   dc(m_pCanvas);
    render(dc);
    event.Skip();
}


void RadarFrame::render(wxDC& dc)     {
    m_Timer->Start(RESTART);

    // Calculate the size based on paint area size, if smaller then the minimum
    // then the default minimum size applies
    int width  = max(m_pCanvas->GetSize().GetWidth(), (MIN_RADIUS)*2 );
    int height = max(m_pCanvas->GetSize().GetHeight(),(MIN_RADIUS)*2 );
    wxSize       size(width, height);
    wxPoint      center(width/2, height/2);
    int radius = max((min(width,height)/2),MIN_RADIUS);
    
    //    m_pCanvas->SetBackgroundColour (m_BgColour);
    renderRange(dc, center, size, radius);    
    if ( pPlugIn->GetAisTargets() ) {
        renderBoats(dc, center, size, radius);
    }
}


void RadarFrame::TrimAisField(wxString *fld) {
    assert(fld);
    while (fld->Right(1)=='@' || fld->Right(1)==' ') {
        fld->RemoveLast();
    }
}


void RadarFrame::renderBoats(wxDC& dc, wxPoint &center, wxSize &size, int radius) {
    // Determine orientation
    double offset=pPlugIn->GetCog();
    if (m_pNorthUp->GetValue()) {
        offset=0;
    }

    // Get display settings
    bool   m_ShowMoored=pPlugIn->ShowMoored();
    double m_MooredSpeed=pPlugIn->GetMooredSpeed();
    bool   m_ShowCogArrows=pPlugIn->ShowCogArrows();
    int    m_CogArrowMinutes=pPlugIn->GetCogArrowMinutes();

    // Show other boats and base stations
    Target    dt;
    ArrayOfPlugIn_AIS_Targets *AisTargets = pPlugIn->GetAisTargets();
    PlugIn_AIS_Target *t;
    ArrayOfPlugIn_AIS_Targets::iterator it;
    wxString  Name;

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
                Name     = wxString::From8BitData(t->ShipName);
                TrimAisField(&Name);
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
	dc.DrawCircle( center, radius);
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
    float Range=RangeData[m_pRange->GetSelection()];
    dc.DrawText(wxString::Format(wxT("%s %2.2f"), _("Range"),Range  ), 0, 0); 
 //   dc.DrawText(wxString::Format(wxT("%s %2.2f"), _("Ring "), Range/4), 0, fh+TEXT_MARGIN); 

    // Draw the orientation info
	wxString dir;
	if (m_pNorthUp->GetValue()) {
		dir=_("North Up");
        // Draw north, east, south and west indicators
        dc.SetTextForeground(wxColour(128,128,128));
        dc.DrawText(_("N"), size.GetWidth()/2 + 5, 0);
        dc.DrawText(_("S"), size.GetWidth()/2 + 5, size.GetHeight()-dc.GetCharHeight());
        dc.DrawText(_("W"), 5, size.GetHeight()/2 - dc.GetCharHeight());
        dc.DrawText(_("E"), size.GetWidth() - 7 - dc.GetCharWidth(), size.GetHeight()/2 - dc.GetCharHeight());
    } else {
        dir=_("Course Up"); 
        // Display our own course at to top
        double offset=pPlugIn->GetCog();
        dc.SetTextForeground(wxColour(128,128,128));
        int cpos=0;
        dc.DrawText(wxString::Format(_T("%3.0f\u00B0"),offset), size.GetWidth()/2 - dc.GetCharWidth()*2, cpos);
    }
    dc.SetTextForeground(wxColour(0,0,0));
    dc.DrawText(dir,  size.GetWidth()-dc.GetCharWidth()*dir.Len()-fh-TEXT_MARGIN, 0); 
    if (m_pBearingLine->GetValue()) {
        // Display and estimated bearing line
        int x = center.x;
        int y = center.y;
        double angle = m_Ebl *(double)((double)3.141592653589/(double)180.);
        x += sin(angle) * (radius + 20);
        y -= cos(angle) * (radius + 20);
        dc.DrawLine(center.x, center.y, x, y);
        int tx = center.x + sin(angle) * (radius - 20) - dc.GetCharWidth() * 1.5;
        int ty = center.y - cos(angle) * (radius - 20);
        double offset=0.;
        if ( !m_pNorthUp->GetValue() ) {
            offset = pPlugIn->GetCog();
        }
        dc.SetTextForeground(wxColour(128,128,128));
        dc.DrawText(wxString::Format(_T("%3.1d\u00B0"),(int)(m_Ebl+offset)%360),tx,ty);
    }
}
