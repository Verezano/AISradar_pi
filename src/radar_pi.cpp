/******************************************************************************
 * $Id: radar_pi.cpp,v 1.8 2010/06/21 01:54:37 bdbcat Exp $
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


#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers


#include <wx/fileconf.h>
#include "radar_pi.h"


// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr)
{
    return new radar_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p)
{
    delete p;
}


//---------------------------------------------------------------------------------------------------------
//
//    Radar PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------
#include "my_icons.h"
#include "icons.h"


//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

radar_pi::radar_pi(void *ppimgr) : opencpn_plugin(ppimgr), m_pRadarFrame(0)
{
	// Initialise my images
      initialize_my_images();
}

int radar_pi::Init(void) {
	  AddLocaleCatalog( _T("opencpn-radar_pi") );
      m_radar_frame_x = 0;
      m_radar_frame_y = 0;
      m_radar_frame_sx = 200;
      m_radar_frame_sy = 200;
      m_pRadarFrame = 0;
	  m_cog=0.;
	  m_sog=0.;
	  m_sats=0;
      ::wxDisplaySize(&m_display_width, &m_display_height);
      m_pconfig = GetOCPNConfigObject();
      LoadConfig();
      m_parent_window = GetOCPNCanvasWindow();
      if(m_radar_show_icon) {
            m_leftclick_tool_id  = InsertPlugInTool(_T(""), _img_radar, _img_radar, wxITEM_NORMAL,
                  _("Radar"), _T(""), 0,
                   RADAR_TOOL_POSITION, 0, this);

	  }
	  if (m_radar_use_ais) {
		  AisTargets = GetAISTargetArray();
	  }
      return (WANTS_TOOLBAR_CALLBACK   |
           INSTALLS_TOOLBAR_TOOL       |
           WANTS_CONFIG                |
           WANTS_PREFERENCES           |
		   WANTS_AIS_SENTENCES         |
		   WANTS_NMEA_EVENTS           |
		   USES_AUI_MANAGER

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
      return _("Radar");
}


wxString radar_pi::GetShortDescription() {
      return _("Radar PlugIn for OpenCPN");
}


wxString radar_pi::GetLongDescription() {
      return _("Radar PlugIn for OpenCPN\nShows AIS targets in a radar style view \n\n");
}


void radar_pi::SetDefaults(void) {
      if(!m_radar_show_icon)
      {
            m_radar_show_icon = true;
            m_leftclick_tool_id  = InsertPlugInTool(_T(""), _img_radar, _img_radar, wxITEM_NORMAL,
                  _("Radar"), _T(""), 0,
                   RADAR_TOOL_POSITION, 0, this);
      }
}


int radar_pi::GetToolbarToolCount(void) {
      return 1;
}


void radar_pi::ShowPreferencesDialog( wxWindow* parent ) {
	//
	//     Panel structure
	//
	//     dialog
	//     - RadarBox
	//     - PanelSizer
	//       - RadarBoxSizer
	//         - m_pRadarShowIcon
	//         - m_pRadarUseAis
	//       - DialogButtonSizer
	//
	//

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
			m_pRadarFrame->Create ( m_parent_window, this, -1, _("Radar Display Control"),
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
		AisTargets = GetAISTargetArray();
	}
	if ( m_pRadarFrame ) {
		m_pRadarFrame->Refresh();
	}
}


void radar_pi::SetPositionFix(PlugIn_Position_Fix &pfix) {
	m_cog  = pfix.Cog;
	m_sog  = pfix.Sog;
	m_sats = pfix.nSats;
	if ( m_pRadarFrame ) {
		m_pRadarFrame->Refresh();
	}
}


void radar_pi::OnRadarFrameClose() {
	  m_pRadarFrame = 0;
      SaveConfig();
}


void radar_pi::SetColorScheme(PI_ColorScheme cs) {
	// Colours changed, pass the event on to the radarframe
	if ( m_pRadarFrame ) {
		m_pRadarFrame->SetColourScheme(cs);
	}
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
      if(pConf)
      {
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
