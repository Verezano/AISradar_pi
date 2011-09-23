/******************************************************************************
 * $Id:   $
 *
 * Project:  OpenCPN
 * Purpose:  Class to draw targets on raderview
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
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#	include <wx/wx.h>
#endif
#include <wx/mstream.h>
#include "Target.h"

#ifdef __WXMSW__
  #include <stdlib.h>
  #include <math.h>
  #include <time.h>
  #include <psapi.h>
#endif

static const int VECTOR_SIZE  = 10;
static const int BASE_STATION = 3;
static const wxColor  VectorColor[3] = {
	wxColor(0,128,0),
	wxColor(255,0,0),
	wxColor(128,128,128)
};

static wxImage   *TargetImg[3] = {
	// Green Ship
	new wxImage(wxMemoryInputStream("\211PNG\r\n\032\n\000\000\000\rIHDR\000\000\000\021\000\000\000!\b\006\000\000\000?\027@W\000\000\000\004sBIT\b\b\b\b|\bd\210\000\000\000\011pHYs\000\000\r\327\000\000\r\327\001B(\233x\000\000\000\031tEXtSoftware\000www.inkscape.org\233\356<\032\000\000\001\336IDATH\211\235\326[O\023A\030\207\361\337\273=\027\032\003h\004\004z\240\304DL0J\224x\240 \212Z\215\337\377\322\033.\374\002H[\261^T9\224v\273\360$\223M\336\235}\362\2377\223\2315\034\016M\033\250c\210\207i\363\022\351\354\217='\222.):\266\200\222\343\324y\251\313)9\361\335P\331\217;-'\"\226\235[\262\203s\313\021\261x\227\345\354[\323\223\303\252\036\336\336^R\360A[\r\264\325\224|\274\275$\347XC\200\206\220\370t+ID,\032X\261\362\257\260\212\201zD\324n\223\344\235U\277\344.R\261\354\014\257\263K\n\216\264\315_\253\265\315):\312.\311\371\254q\221cD]N^7\223$\"j\006\032\036\215\275XC\317VDT\262$y\343\201S\371\261j\001\367\235\341\325lI\336\241-s\223bk\253\312{\237E\322\325\270\221cDS^\321\327TIDT\364=\266>Q\301:z\266#\242\220\226d\317\222S\305)\222\022\026\364\260;]\2228\324v\243\373\327\330T\2218\230.)\351jN\3151\242\251\240\354\333DID\024\365=\265\221\252`\003=\317#\342b3^M\262\353\236\236\362\014I\0255\003\354\334\224$\016l*\315P\214h)^\355\313\245\244\244\253\225YRR\271\354K\002\021\221\323\367B=\223bt\033\365\274\214\210\270\232\344\231y}\325\214\222\032*\206xrU\322\261\2510\365\243I4\345\321\271\224\224u\265fl\262qZ\312\252\243\276$\021\021~\333\313\334\217\377\3241\030\035\227\011\266U\3741\361\bNa\001y\205\210h'\350h\316\274\330'\323\004\235D\305\027\255)\207\320,Z\252*\272$~\032\375\203\334m$N\376\002x=\231\2619V\302\254\000\000\000\000IEND\256B`\202", 609)),
    // Red ship
	new wxImage(wxMemoryInputStream("\211PNG\r\n\032\n\000\000\000\rIHDR\000\000\000\021\000\000\000!\b\006\000\000\000?\027@W\000\000\000\004sBIT\b\b\b\b|\bd\210\000\000\000\011pHYs\000\000\r\327\000\000\r\327\001B(\233x\000\000\000\031tEXtSoftware\000www.inkscape.org\233\356<\032\000\000\001\360IDATH\211\235\325Mk\023Q\030\305\361_\322\3144i\032\223\246\005k7\245(\202/`\221J\305\227&m\032C\242\337\320\357\"\270\321/\340\252\013\351&\013\321E\205\322\274\b\216\213IJL\223\311\244\017\034.sg8\363\234\377\314\275W\024E\346\011\273\210p7\351\271\254\344:\232\032gV\242I\211w\367Gc\342\253\222\332,\321\375H\264\301yb\354\004\036\333y\372C\242\002}To\303\344\350\220A\200\027\014\360fi&k\234v(A\233\322:\315\245\231\2249\377J\024\021}!\252r\266\024\023TC\006\303\221\311\220h5\216TZ\206\311\333\347\364\202\321E\200\375\030\356\253\324L\n4\332\254O\316\265(\026h\244fR\341\354\363(\312X\237\2106\371\226\212\011J\001\303\336\224\311\025Q\030s)\244a\362\372)W\371\233\021=\212\271\034.d\022r\334\2468+z\213\265<'\013M\212t\352\344f\231\034\223[\347\375\215\033S<\n\001\303\313)\036c\375\216\271\364\021$1y\371\220\253\231Yp\a{1\334\203\271qr\034\267b\206s\253E!G}\256\311\035:'\204I&\307\004e>\37479\301#\014\350_\314\3411\326\257x\035\365\2602\213\311\301.\203rR\033\330\304=\376\340\331\2158Y\352-V\027x\200&av\202\313\265I9\346\221\312\344\204\325\352$\227\021\217\225\220\336\317\005<\306\352\022\345\271Df\222\311\3766\303\2554m`\a\033\361\241\366x2N\3554\336{R\327i\2744j\327&\025:\315\005?\331t5\310oMp\311\344\271\354\246\3441\326w\242\"\027c&O*\374\335Y\246\r\354!O\220\311d\036dQk,8\223\347U=\036j\331\n\355\346\234MhQ5Y\253\322\221\343\207\370s\335J!\335\177\177N\317|\016\277$\337\000\000\000\000IEND\256B`\202", 627)),
    // Lost ship
	new wxImage(wxMemoryInputStream("\211PNG\r\n\032\n\000\000\000\rIHDR\000\000\000\020\000\000\000 \b\006\000\000\000\033\211\370\314\000\000\000\004sBIT\b\b\b\b|\bd\210\000\000\000\011pHYs\000\000\r\327\000\000\r\327\001B(\233x\000\000\000\031tEXtSoftware\000www.inkscape.org\233\356<\032\000\000\002GIDATH\211\225\224\301r\2330\024E\357\223\220\r\226d&\304\0312\254\273\350\030\260\220\322\244\231\264S\307\331:\375\315\374J\327\375\211\354\275)\216iB\027\005\217c\233\330\276;\211\367\216\356}\b\250\256ktIk\375\2331\266Z,\026\327]5\324\005 \242\200s\276`\214\275UU\245\353\272\256\366\325\261\316\343\201\333$I\376\\^^.\001|\351*\362\272\036p\316g\316\271\001\000<??\317\000\374:\311A\020\004sc\2140\306\b)\345\317\223\034\020Q\237s\236\217\307c\000@Y\226\005\021\361\272\256_\217up\023\307q)\245\204\224\022\027\027\027+\000\356\350\b\214\261\a\347\234\337\256\235s>c\354\341h\300`0\230\033cz\355\332\030\323SJ=\356\253\335\231\001\021\011\317\363L\232\246\353\2754MQ\226\345\025\021\261\272\256\337\0169\270>??_j\255\327\033ZkDQT\001(\016F`\214\335;\347\372\333\373\316\271>c\354\376 \240\311\277\0030\306\364\225R;\367\341\335\014\210\310\363<\357*\313\262\355:dY\206\345ryCDTo|@\333\016\334\331\331\331*\014\303\035@\030\206\b\303\360\025@\336\031\201\210f\326Z\261\323\335\310Z+\210h\326\011\220R\316\2151>:d\214\361\265\326\357\356\303z\006D\304=\317\273\336\227\277U3\207\333\3159l:(\206\303\341\337(\212:\001Q\024A)\005\000\343}\021\246EQ\360\316\356\366\224\3775\323\035\200R\352\261(\212\340\b@0\034\016\327\367\301\003\000\"bB\210\257y\236ww6\312\363\034///w\333\016&R\312z4\032\035\004\214F#\004A\300\211\350\363&`j\214\371\350\a\373NM\355t\rPJ\315\255\265\a\363\267\262\326\006\355}`DD\253\325\352\356\243\367\277\255,\313PU\325\367\326A\352\373>\305q|4 \216c\364z\275\036\021}b\000\246\223\311\344\350\346VM\317\224I)\347\326\332\301\251\000k\355@k\375\310\252\252\372vJ\376V\315\034~0!\204H\222\344d@\222$\340\234\a\236RJ<==\235\014\000\000\245\224\370\a\234\011z\bR=\024\347\000\000\000\000IEND\256B`\202", 714))
};

// Base station
static wxImage   *BaseImg = new wxImage(wxMemoryInputStream("\211PNG\r\n\032\n\000\000\000\rIHDR\000\000\000\023\000\000\000\027\b\006\000\000\000\351\301t\332\000\000\000\004sBIT\b\b\b\b|\bd\210\000\000\000\011pHYs\000\000\r\327\000\000\r\327\001B(\233x\000\000\000\031tEXtSoftware\000www.inkscape.org\233\356<\032\000\000\000\323IDAT8\215\355\322\255JDQ\024\305\361\337\276\n\032\004\2030Y'X,\023\304`1\013>\202\006\301h\266\033|\006Alc\260\011f\215\266\301W\3200E\233O\260-7\b\036\317\275L2\334\005\247\034\366\372\263\366\207\314T{8\300\013\366:k+\220\300\005n1\302\035\316k\260FA\021\261\216{|e\346Yf~f\3461\226#b\032\021k%_)\321\004\317\330\375#\361>\236\260Sm\023\247x\300F\307\034Gx\304\311/\030Vq\203K4]\203n=K\270\3025V\332?\3436\366a\037H\001z\324\372\3070\303\346\"\240\037\300-\314\032\3143\363\275\270\235\236\312\3147\314\213\247\261\250\006\330\000\033`\377\023\026\370\300k\317\332\232\266\277\001\265\364?\353L\240/\231\000\000\000\000IEND\256B`\202", 342));


//---------------------------------------------------------------------------------------
//          Radar target Implementation
//---------------------------------------------------------------------------------------

Target::Target() 
: State(PI_AIS_NO_ALARM), CanvasCenter(wxPoint(0,0)), Radius(0), Range(0.), Mycog(0.), 
	ShowArrow(true), ArrowMinutes(6), Mmsi(0),
	Tclass(0), Dist(0.), Cog(0.), Sog(0.),
	TargetCenter(wxPoint(TargetImg[PI_AIS_NO_ALARM]->GetWidth()/2,TargetImg[PI_AIS_NO_ALARM]->GetHeight()/2))
{


}

Target::~Target( ) {
}


void Target::SetCanvas(wxPoint &center, int radius, wxColor &cl) {
	CanvasCenter=center;
	Radius=radius;
}


void Target::SetNavDetails(double range, double mycog, bool showarrow, int arrowminutes) {
	Range=range;
	Mycog=mycog;
	ShowArrow=showarrow;
	ArrowMinutes=arrowminutes;
}


void Target::SetState(int mmsi, wxString name, double  dist, 
						double brg, double cog, double sog,
						int tclass, plugin_ais_alarm_type state,
						int rot
) {
	Mmsi=mmsi;
	Name=name;
	Dist=dist;
	Brg=brg;
	Cog=cog;
	Sog=sog;
	Tclass=tclass;
	// There are only 3 images so only accept state that match these images
	if (state>=0 && state<=3) {
		State=state;
	}
	Rot=rot;
}


bool Target::Render( wxDC& dc ) {
	bool Result=false;
	// Check if the target is less than sqrt 2 times the range away
	// This way targets in the corners of the square are still displayed
	// Anything furthur away won't display
	if (Dist < Range * 1.4) {
		// Default to center of view
		int x = CanvasCenter.x;
		int y = CanvasCenter.y;
		if (Dist>(double)0.) {
			// Calculate the drawing position using trigonometry
			double delta= Dist/Range*(double)Radius;
			double angle=(Brg-Mycog)*(double)((double)3.141592653589/(double)180.);
			x += sin(angle) * delta;
			y -= cos(angle) * delta;
		}

		// Calculate the targets direction on the screen
		double ScrCog = (Cog-Mycog)*(double)((double)3.141592653589/(double)180.);

		// Draw the speed vector
		if ( ShowArrow && Sog > 0.2 ) {
			int vsize  = ((double)ArrowMinutes/60)* Sog / Range * (double)Radius ;
			int vx    = x + sin(ScrCog) * vsize;
			int vy    = y - cos(ScrCog) * vsize;
			dc.SetPen( wxPen( VectorColor[State], 1, wxSOLID ) );
			dc.DrawLine(x,y,vx,vy);

			// and the rate of turn vector
			if ( Rot != 0 && Rot != -128 ) {
				double rotangle = ScrCog;
				if ( Rot > 0 ) {
					rotangle += (double)3.141592653589/2.;
				} else {
					rotangle -= (double)3.141592653589/2.;
				}
				int nx = vx + sin(rotangle) * VECTOR_SIZE;
				int ny = vy - cos(rotangle) * VECTOR_SIZE;
				dc.SetPen( wxPen( VectorColor[State], 1, wxSOLID ) );
				dc.DrawLine(vx,vy,nx,ny);
			}
		}

		// Rotate the target image to reflect its course
		// ignore base stations
		wxBitmap bm;
		if (Tclass==BASE_STATION) {
			bm= wxBitmap(*BaseImg);
		} else {
			bm= wxBitmap(TargetImg[State]->Rotate(-ScrCog,TargetCenter));
		}

		// put the center of the rotated image on the calculated spot
		int cx = x - bm.GetWidth()/2;
		int cy = y - bm.GetHeight()/2;

		// Draw the target image and id
		dc.DrawBitmap(bm, cx,cy);
		wxFont fnt = dc.GetFont();
		fnt.SetPointSize(8);
		dc.SetFont(fnt);
		if (Name.StartsWith(wxT("Unknown"))) {
			dc.DrawText(wxString::Format(wxT("%07d"),Mmsi), x+15,y-5);
		} else {
			dc.DrawText(Name, x+10,y-5);
		}
	}
	return Result;
}
