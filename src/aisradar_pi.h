/******************************************************************************
 * $Id:  $
 *
 * Project:  OpenCPN
 * Purpose:  Radar Plugin
 * Author:   Johan van der Sman
 *
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

#ifndef _AISRADARPI_H_
#define _AISRADARPI_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "version.h"

#include "ocpn_plugin.h"
#include "aisview.h"

//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------

#define AISVIEW_TOOL_POSITION    -1          // Request default positioning of toolbar tool

class aisradar_pi : public opencpn_plugin_118
{
public:
    aisradar_pi(void *ppimgr);
    ~aisradar_pi();

// The required PlugIn Methods
    int Init(void) override;
    bool DeInit(void) override;

    int GetAPIVersionMajor() override;
    int GetAPIVersionMinor() override;
    int GetPlugInVersionMajor() override;
    int GetPlugInVersionMinor() override;
    int GetPlugInVersionPatch() override;
    int GetPlugInVersionPost() override;
    wxBitmap *GetPlugInBitmap() override;
    wxString GetCommonName() override;
    wxString GetShortDescription() override;
    wxString GetLongDescription() override;
    wxBitmap m_panelBitmap;
    const char *GetPlugInVersionPre() override { return ""; }
    const char *GetPlugInVersionBuild() override { return ""; }
        // FIXME: Add real build info such as git hash and/or build number

    // from shipdriver for panel icon
    void SetDefaults(void) override;
    int  GetToolbarToolCount(void) override;
    void ShowPreferencesDialog( wxWindow* parent ) override;
    void OnToolbarToolCallback(int id) override;
    void SetAISSentence(wxString &sentence) override;
    void SetPositionFix(PlugIn_Position_Fix &pfix) override;
    void SetPluginMessage(wxString &message_id, wxString &message_body) override;
    void SetColorScheme(PI_ColorScheme cs) override;

// Other public methods
    void             SetAisFrameX      (int x)  { m_ais_frame_x  = x;   }
    void             SetAisFrameY      (int x)  { m_ais_frame_y  = x;   }
    void             SetAisFrameSizeX  (int x)  { m_ais_frame_sx = x;   }
    void             SetAisFrameSizeY  (int x)  { m_ais_frame_sy = x;   }
    void             SetAisNorthUp     (bool x) { m_ais_north_up = x;   }
    void             SetAisRange       (int x)  { m_ais_range    = x;   }
    bool             GetAisNorthUp     (void)   { return m_ais_north_up;}
    int              GetAisRange       (void)   { return m_ais_range;   }
    double           GetCog            (void)   { return m_cog;         }
    double           GetSog            (void)   { return m_sog;         }
    int              GetSats           (void)   { return m_sats;        }
    wxFileConfig    *GetConfig         (void)   { return m_pconfig;     }
    ArrayOfPlugIn_AIS_Targets  *GetAisTargets();
    void             OnAisFrameClose();
    bool             ShowMoored        (void);
    double           GetMooredSpeed    (void);
    bool             ShowCogArrows     (void);
    int              GetCogArrowMinutes(void);

private:
    bool LoadConfig(void);
    bool SaveConfig(void);

private:
    wxFileConfig     *m_pconfig;
    wxWindow         *m_parent_window;
    AisFrame         *m_pAisFrame;
    ArrayOfPlugIn_AIS_Targets *AisTargets;
    int               m_display_width, m_display_height;
    int               m_leftclick_tool_id;
    int               m_ais_frame_x, m_ais_frame_y;
    int               m_ais_frame_sx, m_ais_frame_sy;
    int               m_ais_range;
    double            m_lat;
    double            m_lon;
    double            m_cog;
    double            m_sog;
    int               m_sats;
    bool              m_ais_show_icon;
    bool              m_ais_use_ais;
    bool              m_ais_north_up;
    wxCheckBox       *m_pAisShowIcon;
    wxCheckBox       *m_pAisUseAis;
//	wxBitmap          m_panelBitmap;
};

#endif
