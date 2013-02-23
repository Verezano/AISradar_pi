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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************
 */
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/fileconf.h>
#include "radar_pi.h"

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr) {
    return new radar_pi(ppimgr);
}


extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p) {
    delete p;
}


//---------------------------------------------------------------------------------------------------------
//
//    Radar PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------
#include "my_icons.h"


//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

radar_pi::radar_pi(void *ppimgr) : opencpn_plugin_17(ppimgr), m_pRadarFrame(0)
{
      initialize_my_images();
}


radar_pi::~radar_pi() {
	if ( AisTargets ) {
		WX_CLEAR_ARRAY(*AisTargets); 	
		delete AisTargets;
	}
}


int radar_pi::Init(void) {
	  AddLocaleCatalog( _T("opencpn-radar_pi") );
      m_radar_frame_x = m_radar_frame_y = 0;
      m_radar_frame_sx = m_radar_frame_sy = 200;
      m_pRadarFrame = 0;
	  m_lat= m_lon=0.;
	  m_cog= m_sog=0.;
	  m_sats=0;
      ::wxDisplaySize(&m_display_width, &m_display_height);
      m_pconfig = GetOCPNConfigObject();
      LoadConfig();
	  AisTargets = GetAISTargetArray();
      m_parent_window = GetOCPNCanvasWindow();
      if(m_radar_show_icon) {
            m_leftclick_tool_id  = InsertPlugInTool(_T(""), _img_radar, 
				_img_radar, wxITEM_NORMAL, _("AIS Radar view"), _T(""), 0,
                   RADAR_TOOL_POSITION, 0, this
			);
	  }
      return (WANTS_TOOLBAR_CALLBACK | INSTALLS_TOOLBAR_TOOL |
           WANTS_CONFIG | WANTS_PREFERENCES | WANTS_AIS_SENTENCES  |
		   WANTS_NMEA_EVENTS | WANTS_PLUGIN_MESSAGING | USES_AUI_MANAGER
      );
}


bool radar_pi::DeInit(void) {
      if(m_pRadarFrame) {
            m_pRadarFrame->Close();
	  }
      return true;
}


int radar_pi::GetAPIVersionMajor() {
      return MY_API_VERSION_MAJOR;
}


int radar_pi::GetAPIVersionMinor() {
      return MY_API_VERSION_MINOR;
}


int radar_pi::GetPlugInVersionMajor() {
      return PLUGIN_VERSION_MAJOR;
}


int radar_pi::GetPlugInVersionMinor() {
      return PLUGIN_VERSION_MINOR;
}


wxBitmap *radar_pi::GetPlugInBitmap() {
      return _img_radar_pi;
}


wxString radar_pi::GetCommonName() {
      return _("AIS Radar view");
}


wxString radar_pi::GetShortDescription() {
      return _("AIS Radar view PlugIn");
}


wxString radar_pi::GetLongDescription() {
      return _("Radar PlugIn for OpenCPN\nShows AIS targets in a radar style view \n\n");
}


void radar_pi::SetDefaults(void) {
	  if(!m_radar_show_icon) {
            m_radar_show_icon = true;
            m_leftclick_tool_id  = InsertPlugInTool(_T(""), _img_radar, _img_radar, wxITEM_NORMAL,
                  _("AIS Radar"), _T(""), 0,
                   RADAR_TOOL_POSITION, 0, this
			);
      }
}


int radar_pi::GetToolbarToolCount(void) {
      return 1;
}


void radar_pi::ShowPreferencesDialog( wxWindow* parent ) {
    wxDialog *dialog = new wxDialog( parent, wxID_ANY, _("Radar Preferences"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE );
    int border_size = 4;

	wxColor     cl;
	GetGlobalColor(_T("DILG1"), &cl);
	dialog->SetBackgroundColour(cl);

	wxBoxSizer* PanelSizer = new wxBoxSizer(wxVERTICAL);
    dialog->SetSizer(PanelSizer);

    //  Radar toolbox icon checkbox
    wxStaticBox* RadarBox = new wxStaticBox(dialog, wxID_ANY, _("Radar"));
    wxStaticBoxSizer* RadarBoxSizer = new wxStaticBoxSizer(RadarBox, wxVERTICAL);
    PanelSizer->Add(RadarBoxSizer, 0, wxGROW|wxALL, border_size);

    m_pRadarShowIcon = new wxCheckBox( dialog, -1, _("Show RADAR icon:"), wxDefaultPosition, wxSize(-1, -1), 0 );
    RadarBoxSizer->Add(m_pRadarShowIcon, 1, wxALIGN_LEFT|wxALL, border_size);
    m_pRadarShowIcon->SetValue(m_radar_show_icon);

	m_pRadarUseAis = new wxCheckBox( dialog, -1, _("Use AIS as radar source:"), wxDefaultPosition, wxSize(-1, -1), 0 );
    RadarBoxSizer->Add(m_pRadarUseAis, 2, wxALIGN_LEFT|wxALL, border_size);
    m_pRadarUseAis->SetValue(m_radar_use_ais);

	wxStdDialogButtonSizer* DialogButtonSizer = dialog->CreateStdDialogButtonSizer(wxOK|wxCANCEL);
    PanelSizer->Add(DialogButtonSizer, 0, wxALIGN_RIGHT|wxALL, 5);
    dialog->Fit();

    if(dialog->ShowModal() == wxID_OK)       {
            //    Show Icon changed value?
            if(m_radar_show_icon != m_pRadarShowIcon->GetValue()) {
                  m_radar_show_icon= m_pRadarShowIcon->GetValue();
				  if(m_radar_show_icon) {
                        m_leftclick_tool_id  = InsertPlugInTool(_T(""), _img_radar, _img_radar, wxITEM_NORMAL,
                              _("Radar"), _T(""), 0, RADAR_TOOL_POSITION,
                              0, this);
				  } else {
                        RemovePlugInTool(m_leftclick_tool_id);
				  }
            }
            m_radar_use_ais    = m_pRadarUseAis->GetValue();
			SaveConfig();
      }
}


void radar_pi::OnToolbarToolCallback(int id) {
     ::wxBell();
      if(!m_pRadarFrame) {
            m_pRadarFrame = new RadarFrame();
			m_pRadarFrame->Create ( m_parent_window, this, -1, _("AIS Radar view"),
                               wxPoint( m_radar_frame_x, m_radar_frame_y), wxSize( m_radar_frame_sx, m_radar_frame_sy));
  	        m_pRadarFrame->Show();
	  } else {
		  m_pRadarFrame->Close(true);
	  }
}


void radar_pi::SetAISSentence(wxString &sentence) {
	// Ignore the AIS message itself. It is merely used as an event
	// to retrieve the AIS targets from the mainprogram
	// The targets are already updated to reflect the current message
	// So we re-use that information
	if (m_radar_use_ais) {
		GetAisTargets();
	}
	if ( m_pRadarFrame ) {
		m_pRadarFrame->Refresh();
	}
}


void radar_pi::SetPositionFix(PlugIn_Position_Fix &pfix) {
	// Check if our position changed
	// If so, update view otherwise not, 
	if ( m_lat != pfix.Lat || m_lon != pfix.Lon || m_cog != pfix.Cog || m_sog != pfix.Sog ) {
		m_lat  = pfix.Lat;
		m_lon  = pfix.Lon;
		m_cog  = pfix.Cog;
		m_sog  = pfix.Sog;
		m_sats = pfix.nSats;
		if ( m_pRadarFrame ) {
			m_pRadarFrame->Refresh();
		}
	}
}


void radar_pi::SetPluginMessage(wxString &message_id, wxString &message_body) {
// Parse message with radar targets
// Format:
//	{
//		"RadarTargets" :
//		{
//			"Source"      : "BR24_pi",
//          "Orientation" : "North",
//			"TTL"         : 30,
//			"Targets" : 
//			[
//				{ "Brg" : 180, Range : 1.2 },
//				{ "Brg" : 359, Range : 8.3 },
//				{ "Brg" : 34, Range : 0.9 },
//				{ "Brg" : 13, Range : 0.4 },
//				{ "Brg" : 57, Range : 0.7 }
//			]
//		}
//	}
//  TODO: implement parse routine	
}


void radar_pi::OnRadarFrameClose() {
	  m_pRadarFrame = 0;
      SaveConfig();
}


bool radar_pi::ShowMoored(void) {
	bool Result=true;
	m_pconfig->SetPath ( _T( "/Settings/AIS" ) );
	m_pconfig->Read ( _T( "bShowMooredTargets" ),  &Result, 1 );
	return Result;
}


double radar_pi::GetMooredSpeed(void) {
	double Result=0.;
	m_pconfig->SetPath ( _T( "/Settings/AIS" ) );
	m_pconfig->Read ( _T( "MooredTargetMaxSpeedKnots" ),  &Result, 0.0 );
	return Result;
}


bool radar_pi::ShowCogArrows(void) {
	bool Result=true;
	m_pconfig->SetPath ( _T( "/Settings/AIS" ) );
	m_pconfig->Read ( _T("bShowCOGArrows"), &Result, 1);
	return Result;
}


int radar_pi::GetCogArrowMinutes(void) {
	int Result=6;
	m_pconfig->SetPath ( _T( "/Settings/AIS" ) );
	m_pconfig->Read ( _T("CogArrowMinutes"), &Result, 6);
	return Result;
}


void radar_pi::SetColorScheme(PI_ColorScheme cs) {
	// Colours changed, pass the event on to the radarframe
	if ( m_pRadarFrame ) {
		m_pRadarFrame->SetColourScheme(cs);
	}
}


ArrayOfPlugIn_AIS_Targets  *radar_pi::GetAisTargets() {
	if ( AisTargets ) {
		WX_CLEAR_ARRAY(*AisTargets); 	
		delete AisTargets;
	}
	AisTargets = GetAISTargetArray();
	return AisTargets;
}


bool radar_pi::LoadConfig(void) {
      wxFileConfig *pConf = (wxFileConfig *)m_pconfig;
      if(pConf) {
            pConf->SetPath ( _T( "/Settings" ) );
            pConf->Read ( _T( "ShowRADARIcon" ),  &m_radar_show_icon, 1 );
            pConf->Read ( _T( "UseAisRadar" ),  &m_radar_use_ais, 1 );
            pConf->Read ( _T( "NorthUp" ),  &m_radar_north_up, 0 );
            m_radar_frame_sx  = pConf->Read ( _T ( "RADARDialogSizeX" ),   300L );
            m_radar_frame_sy  = pConf->Read ( _T ( "RADARDialogSizeY" ),   300L );
            m_radar_frame_x   = pConf->Read ( _T ( "RADARDialogPosX" ),     50L );
            m_radar_frame_y   = pConf->Read ( _T ( "RADARDialogPosY" ),    170L );
            m_radar_range     = pConf->Read ( _T ( "RADARRange" ),           4L );
            return true;
      } else {
            return false;
	  }
}


bool radar_pi::SaveConfig(void) {
      wxFileConfig *pConf = (wxFileConfig *)m_pconfig;
      if(pConf) {
            pConf->SetPath ( _T ( "/Settings" ) );
            pConf->Write   ( _T ( "ShowRADARIcon" ),      m_radar_show_icon  );
            pConf->Write   ( _T ( "UseAisRadar" ),        m_radar_use_ais    );
            pConf->Write   ( _T ( "NorthUp" ),            m_radar_north_up   );
            pConf->Write   ( _T ( "RADARDialogSizeX" ),   m_radar_frame_sx   );
            pConf->Write   ( _T ( "RADARDialogSizeY" ),   m_radar_frame_sy   );
            pConf->Write   ( _T ( "RADARDialogPosX" ),    m_radar_frame_x    );
            pConf->Write   ( _T ( "RADARDialogPosY" ),    m_radar_frame_y    );
            pConf->Write   ( _T ( "RADARRange" ),         m_radar_range      );
            return true;
      } else {
            return false;
	  }
}
