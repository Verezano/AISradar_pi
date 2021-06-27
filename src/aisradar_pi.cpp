/******************************************************************************
 * $Id:  $
 *
 * Project:  OpenCPN
 * Purpose:  Ais Plugin
 * Author:   Johan van der Sman
 *
 ***************************************************************************
 * Frontend2  Author: Jon Gough  Version: 1.0.132
 ***************************************************************************
 *   Copyright (C) 2015 - 2020 Johan van der Sman                          *
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
#include <wx/wxprec.h>

#ifndef  WX_PRECOMP
  #include <wx/wx.h>
#endif //precompiled headers

#include <wx/fileconf.h>
#include "aisradar_pi.h"

#include "icons.h"				  

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr) {
    return new aisradar_pi(ppimgr);
}


extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p) {
    delete p;
}


//---------------------------------------------------------------------------------------------------------
//
//    Ais PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------
#include "icons.h"


//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

aisradar_pi::aisradar_pi(void *ppimgr)
: opencpn_plugin_116(ppimgr),
    m_pconfig(0),
    m_parent_window(0),
    m_pAisFrame(0),
    AisTargets(0),
    m_display_width(0),
    m_display_height(0),
    m_leftclick_tool_id(0),
    m_ais_frame_x(0),
    m_ais_frame_y(0),
    m_ais_frame_sx(0),
    m_ais_frame_sy(0),
    m_ais_range(0),
    m_lat(0.0),
    m_lon(0.0),
    m_cog(0.0),
    m_sog(0.0),
    m_sats(0),
    m_ais_show_icon(true),
    m_ais_use_ais(true),
    m_ais_north_up(false),
    m_pAisShowIcon(0),
    m_pAisUseAis(0)
{

    initialize_images();

	wxFileName fn;

    auto path = GetPluginDataDir("aisradar_pi");
    fn.SetPath(path);
    fn.AppendDir("data");
    fn.SetFullName("aisradar.png");

    path = fn.GetFullPath();

    wxInitAllImageHandlers();

    wxLogDebug(wxString("Using icon path: ") + path);
    if (!wxImage::CanRead(path)) {
        wxLogDebug("Initiating image handlers.");
        wxInitAllImageHandlers();
    }
    wxImage panelIcon(path);
    if (panelIcon.IsOk())
        m_panelBitmap = wxBitmap(panelIcon);
    else
        wxLogWarning("aisradar panel icon has NOT been loaded");
   // m_bShowShipDriver = false;

/*

    wxString shareLocn = GetPluginDataDir("aisradar_pi") + 
        _T("data") + wxFileName::GetPathSeparator();
    wxImage panelIcon(  shareLocn + _T("aisradar.png"));
	
  OLD METHOD - Don't use anymore
  initialize_images();
	wxString shareLocn = *GetpSharedDataLocation() +
        _T("plugins") + wxFileName::GetPathSeparator() +
        _T("aisradar_pi") + wxFileName::GetPathSeparator() +
        _T("data") + wxFileName::GetPathSeparator();
   wxImage panelIcon(  shareLocn + _T("aisradar.png"));
*/ 

/*  PRIVATE DATA DIRECTORY only when needed for user writable data, but do not use for ICONS!
    initialize_images();
	wxString shareLocn = *GetpPrivateApplicationDataLocation() + 
          _T("plugins") + wxFileName::GetPathSeparator() +
          _T("aisradar") + wxFileName::GetPathSeparator() +
          _T("data") + wxFileName::GetPathSeparator();
    wxImage panelIcon(  shareLocn + _T("aisradar.png"));
*/		

// SHOW THE TOOLBAR  BITMAP	IF SHOW ICON CHECKBOX IS CHECKED, DISPLAYS THE img_ais_pi bitmap
    if(panelIcon.IsOk())
        m_panelBitmap = wxBitmap(panelIcon); 
    else
        wxLogMessage(_T(" AISVIEW panel icon NOT loaded"));
        m_panelBitmap = *_img_ais_pi;
}

aisradar_pi::~aisradar_pi() {
    if ( AisTargets ) {
        WX_CLEAR_ARRAY(*AisTargets);
        delete AisTargets;
        AisTargets=0;
    }
}


int aisradar_pi::Init(void) {
    AddLocaleCatalog( _T("opencpn-aisradar_pi") );
    m_ais_frame_x = m_ais_frame_y = 0;
    m_ais_frame_sx = m_ais_frame_sy = 200;
    m_pAisFrame = 0;
    m_lat= m_lon=0.;
    m_cog= m_sog=0.;
    m_sats=0;
    ::wxDisplaySize(&m_display_width, &m_display_height);
    m_pconfig = GetOCPNConfigObject();
    LoadConfig();
    if (AisTargets) {  // Init may be called more than once, check for cleanup
        WX_CLEAR_ARRAY(*AisTargets);
        delete AisTargets;
    }
    m_parent_window = GetOCPNCanvasWindow();

    if(m_ais_show_icon) {

// FOR SVG ICONS  - CMakeLists.txt line 72  PLUGIN_USE_SVG=ON
		
#ifdef PLUGIN_USE_SVG
      m_leftclick_tool_id = InsertPlugInToolSVG(_T( "AISradar" ),
          _svg_aisradar,  _svg_aisradar_toggled, _svg_aisradar_toggled, 
          wxITEM_CHECK, _("AISradar"), _T( "" ), NULL, AISVIEW_TOOL_POSITION, 0, this);
#else
       m_leftclick_tool_id  = InsertPlugInTool
          (_T(""), _img_ais_pi, _img_ais_pi, wxITEM_CHECK, 
		  _("AisView"), _T(""), NULL, 
		  AISVIEW_TOOL_POSITION, 0, this);
#endif

   }

    AisTargets = GetAISTargetArray();
    return (WANTS_TOOLBAR_CALLBACK | INSTALLS_TOOLBAR_TOOL |
         WANTS_CONFIG | WANTS_PREFERENCES | WANTS_AIS_SENTENCES  |
         WANTS_NMEA_EVENTS | WANTS_PLUGIN_MESSAGING | USES_AUI_MANAGER
    );
}


bool aisradar_pi::DeInit(void) {
    if(m_pAisFrame) {
        m_pAisFrame->Close();
    }
    return true;
}


int aisradar_pi::GetAPIVersionMajor() {
    return OCPN_API_VERSION_MAJOR;
}


int aisradar_pi::GetAPIVersionMinor() {
    return OCPN_API_VERSION_MINOR;
}


int aisradar_pi::GetPlugInVersionMajor() {
    return PLUGIN_VERSION_MAJOR;
}


int aisradar_pi::GetPlugInVersionMinor() {
    return PLUGIN_VERSION_MINOR;
}


wxBitmap *aisradar_pi::GetPlugInBitmap() {
	return &m_panelBitmap;
}


wxString aisradar_pi::GetCommonName() {
   return _T(PLUGIN_COMMON_NAME);
}


wxString aisradar_pi::GetShortDescription() {
    return _(PLUGIN_SHORT_DESCRIPTION);
}


wxString aisradar_pi::GetLongDescription() {
    return _(PLUGIN_LONG_DESCRIPTION);
}


void aisradar_pi::SetDefaults(void) {
}


int aisradar_pi::GetToolbarToolCount(void) {
    return 1;
}


void aisradar_pi::ShowPreferencesDialog( wxWindow* parent ) {
    wxDialog *dialog = new wxDialog( parent, wxID_ANY, _("AIS view Preferences"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE );
    int border_size = 4;

    wxColor     cl;
    GetGlobalColor(_T("DILG1"), &cl);
    dialog->SetBackgroundColour(cl);

    wxBoxSizer* PanelSizer = new wxBoxSizer(wxVERTICAL);
    dialog->SetSizer(PanelSizer);

    //  Ais toolbox icon checkbox
    wxStaticBox* AisBox = new wxStaticBox(dialog, wxID_ANY, _("Ais"));
    wxStaticBoxSizer* AisBoxSizer = new wxStaticBoxSizer(AisBox, wxVERTICAL);
    PanelSizer->Add(AisBoxSizer, 0, wxGROW|wxALL, border_size);

    m_pAisShowIcon = new wxCheckBox( dialog, -1, _("Show Ais icon:"), wxDefaultPosition, wxSize(-1, -1), 0 );
    AisBoxSizer->Add(m_pAisShowIcon, 1, wxALIGN_LEFT|wxALL, border_size);
    m_pAisShowIcon->SetValue(m_ais_show_icon);

    m_pAisUseAis = new wxCheckBox( dialog, -1, _("Use AIS as radar source:"), wxDefaultPosition, wxSize(-1, -1), 0 );
    AisBoxSizer->Add(m_pAisUseAis, 2, wxALIGN_LEFT|wxALL, border_size);
    m_pAisUseAis->SetValue(m_ais_use_ais);

    wxStdDialogButtonSizer* DialogButtonSizer = dialog->CreateStdDialogButtonSizer(wxOK|wxCANCEL);
    PanelSizer->Add(DialogButtonSizer, 0, wxALIGN_RIGHT|wxALL, 5);
    dialog->Fit();
    if(dialog->ShowModal() == wxID_OK)       {
         //    Show Icon changed value?
         if(m_ais_show_icon != m_pAisShowIcon->GetValue()) {
              m_ais_show_icon= m_pAisShowIcon->GetValue();
              if(m_ais_show_icon) {
                 #ifdef PLUGIN_USE_SVG
      m_leftclick_tool_id = InsertPlugInToolSVG(_T( "AISradar" ),
          _svg_aisradar,  _svg_aisradar_toggled, _svg_aisradar_toggled, 
          wxITEM_CHECK, _("AISradar"), _T( "" ), NULL, AISVIEW_TOOL_POSITION, 0, this);
#else
       m_leftclick_tool_id  = InsertPlugInTool
          (_T(""), _img_ais_pi, _img_ais_pi, wxITEM_CHECK, 
		  _("AisView"), _T(""), NULL, 
		  AISVIEW_TOOL_POSITION, 0, this);
#endif
              } else {
                   RemovePlugInTool(m_leftclick_tool_id);
              }
         }
         m_ais_use_ais    = m_pAisUseAis->GetValue();
         SaveConfig();
    }
}


void aisradar_pi::OnToolbarToolCallback(int id) {
   ::wxBell();
    if(!m_pAisFrame) {
        m_pAisFrame = new AisFrame();
        m_pAisFrame->Create ( m_parent_window,
            this,
            -1,
            wxString::Format(_T("AIS Radar View %d.%d"),
                PLUGIN_VERSION_MAJOR,
                PLUGIN_VERSION_MINOR
            ),
            wxPoint( m_ais_frame_x, m_ais_frame_y),
            wxSize( m_ais_frame_sx, m_ais_frame_sy)
        );
        m_pAisFrame->Show();
    } else {
        m_pAisFrame->Close(true);
    }
}


void aisradar_pi::SetAISSentence(wxString &sentence) {
    // Ignore the AIS message itself. It is merely used as an event
    // to retrieve the AIS targets from the mainprogram
    // The targets are already updated to reflect the current message
    // So we re-use that information
    if (m_ais_use_ais) {
        GetAisTargets();
    }
    if ( m_pAisFrame ) {
        m_pAisFrame->Refresh();
    }
}


void aisradar_pi::SetPositionFix(PlugIn_Position_Fix &pfix) {
    // Check if our position changed
    // If so, update view otherwise not,
    if ( m_lat != pfix.Lat || m_lon != pfix.Lon || m_cog != pfix.Cog || m_sog != pfix.Sog ) {
        m_lat  = pfix.Lat;
        m_lon  = pfix.Lon;
        if (pfix.Cog>=0.0) {
            m_cog  = pfix.Cog;
        }
        if (pfix.Sog>=0.0) {
            m_sog  = pfix.Sog;
        }
        m_sats = pfix.nSats;
        if ( m_pAisFrame ) {
            m_pAisFrame->Refresh();
        }
    }
}


void aisradar_pi::SetPluginMessage(wxString &message_id, wxString &message_body) {
// Parse message with radar targets
// Format:
//    {
//        "RadarTargets" :
//        {
//            "Source"      : "BR24_pi",
//          "Orientation" : "North",
//            "TTL"         : 30,
//            "Targets" :
//            [
//                { "Brg" : 180, Range : 1.2 },
//                { "Brg" : 359, Range : 8.3 },
//                { "Brg" : 34, Range : 0.9 },
//                { "Brg" : 13, Range : 0.4 },
//                { "Brg" : 57, Range : 0.7 }
//            ]
//        }
//    }
//  TODO: implement parse routine
}


void aisradar_pi::OnAisFrameClose() {
    m_pAisFrame = 0;
    SaveConfig();
}


bool aisradar_pi::ShowMoored(void) {
    bool Result=true;
    m_pconfig->SetPath ( _T( "/Settings/AIS" ) );
    m_pconfig->Read ( _T( "bShowMooredTargets" ),  &Result, 1 );
    return Result;
}


double aisradar_pi::GetMooredSpeed(void) {
    double Result=0.;
    m_pconfig->SetPath ( _T( "/Settings/AIS" ) );
    m_pconfig->Read ( _T( "MooredTargetMaxSpeedKnots" ),  &Result, 0.0 );
    return Result;
}


bool aisradar_pi::ShowCogArrows(void) {
    bool Result=true;
    m_pconfig->SetPath ( _T( "/Settings/AIS" ) );
    m_pconfig->Read ( _T("bShowCOGArrows"), &Result, 1);
    return Result;
}


int aisradar_pi::GetCogArrowMinutes(void) {
    int Result=6;
    m_pconfig->SetPath ( _T( "/Settings/AIS" ) );
    m_pconfig->Read ( _T("CogArrowMinutes"), &Result, 6);
    return Result;
}


void aisradar_pi::SetColorScheme(PI_ColorScheme cs) {
    // Colours changed, pass the event on to the radarframe
    if ( m_pAisFrame ) {
        m_pAisFrame->SetColourScheme(cs);
        DimeWindow(m_pAisFrame);
    }
}


ArrayOfPlugIn_AIS_Targets  *aisradar_pi::GetAisTargets() {
    if ( AisTargets ) {
        WX_CLEAR_ARRAY(*AisTargets);
        delete AisTargets;
    }
    AisTargets = GetAISTargetArray();
    return AisTargets;
}


bool aisradar_pi::LoadConfig(void) {
    wxFileConfig *pConf = (wxFileConfig *)m_pconfig;
    if(pConf) {
        pConf->SetPath ( _T( "/Plugins/AISRadar" ) );
        pConf->Read ( _T( "ShowRADARIcon" ),  &m_ais_show_icon, 1 );
        pConf->Read ( _T( "UseAisRadar" ),  &m_ais_use_ais, 1 );
        pConf->Read ( _T( "NorthUp" ),  &m_ais_north_up, 0 );
        m_ais_frame_sx  = pConf->Read ( _T ( "RADARDialogSizeX" ),   300L );
        m_ais_frame_sy  = pConf->Read ( _T ( "RADARDialogSizeY" ),   300L );
        m_ais_frame_x   = pConf->Read ( _T ( "RADARDialogPosX" ),     50L );
        m_ais_frame_y   = pConf->Read ( _T ( "RADARDialogPosY" ),    170L );
        m_ais_range     = pConf->Read ( _T ( "RADARRange" ),           4L );
        return true;
    } else {
        return false;
    }
}


bool aisradar_pi::SaveConfig(void) {
    wxFileConfig *pConf = (wxFileConfig *)m_pconfig;
    if(pConf) {
        pConf->SetPath ( _T ( "/Plugins/AISRadar" ) );
        pConf->Write   ( _T ( "ShowRADARIcon" ),      m_ais_show_icon  );
        pConf->Write   ( _T ( "UseAisRadar" ),        m_ais_use_ais    );
        pConf->Write   ( _T ( "NorthUp" ),            m_ais_north_up   );
        pConf->Write   ( _T ( "RADARDialogSizeX" ),   m_ais_frame_sx   );
        pConf->Write   ( _T ( "RADARDialogSizeY" ),   m_ais_frame_sy   );
        pConf->Write   ( _T ( "RADARDialogPosX" ),    m_ais_frame_x    );
        pConf->Write   ( _T ( "RADARDialogPosY" ),    m_ais_frame_y    );
        pConf->Write   ( _T ( "RADARRange" ),         m_ais_range      );
        return true;
    } else {
        return false;
    }
}
