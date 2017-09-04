/******************************************************************************
 * $Id:  $
 *
 * Project:  OpenCPN
 * Purpose:  Class to draw targets on raderview
 * Author:   Johan van der Sman
 *
 ***************************************************************************
 *   Copyright (C) 2015 Johan van der Sman                                 *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************
 */

#ifndef _TARGET_H_
#define _TARGET_H_


#include "wx/wxprec.h"
#include "ocpn_plugin.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

//----------------------------------------------------------------------------------------------------------
//    Target Specification
//----------------------------------------------------------------------------------------------------------
//      Describe NavStatus variable
typedef enum ais_nav_status
{
    UNDERWAY_USING_ENGINE = 0,
    AT_ANCHOR,
    NOT_UNDER_COMMAND,
    RESTRICTED_MANOEUVRABILITY,
    CONSTRAINED_BY_DRAFT,
    MOORED,
    AGROUND,
    FISHING,
    UNDERWAY_SAILING,
    HSC,
    WIG,
    RESERVED_11,
    RESERVED_12,
    RESERVED_13,
    RESERVED_14,
    UNDEFINED,
    ATON_VIRTUAL,
    ATON_VIRTUAL_ONPOSITION,
    ATON_VIRTUAL_OFFPOSITION,
    ATON_REAL,
    ATON_REAL_ONPOSITION,
    ATON_REAL_OFFPOSITION

}_ais_nav_status;


//      Describe Transponder Class
typedef enum ais_transponder_class
{
    AIS_CLASS_A = 0,
    AIS_CLASS_B,
    AIS_ATON,    // Aid to Navigation   pjotrc 2010/02/01
    AIS_BASE,     // Base station
    AIS_GPSG_BUDDY, // GpsGate Buddy object
    AIS_DSC,	// DSC target
    AIS_SART,   // SART
    AIS_ARPA,    // ARPA radar target
    AIS_APRS    // APRS position report
}_ais_transponder_class;

//    Describe AIS Alert state
typedef enum ais_alert_type
{
    AIS_NO_ALERT = 0,
    AIS_ALERT_SET,

}_ais_alarm_type;

class Target
{
public:
    Target( );
    ~Target();
    void SetCanvas(wxPoint &center, int radius, wxColor &cl);     // details of drawing area
    void SetNavDetails(double range, double mycog,                // details of navigation view
    bool ShowCogArrow, int CogArrowMinutes);
    void SetState( int mmsi, wxString name, double  dist,          // Target details
        double brg, double cog, double sog, double hdg,
                ais_transponder_class tclass, ais_nav_status tstatus, plugin_ais_alarm_type state,
                        int rot
                    );
    bool Render( wxDC& dc);                                       // render the target

private:
    plugin_ais_alarm_type     State;
    wxPoint                   CanvasCenter;
    int                       Radius;
    double                    Range;
    double                    Mycog;
    bool                      ShowArrow;
    int                       ArrowMinutes;
    int                       Mmsi;
    wxString                  Name;
    ais_transponder_class     Tclass;
    ais_nav_status            Tstatus;
    double                    Dist;
    double                    Brg;
    double                    Cog;
    double                    Hdg;
    double                    Sog;
    int                       Rot;
    int                       TargetWidth;
    int                       TargetHeight;
    wxPoint                   TargetCenter;
};

#endif
