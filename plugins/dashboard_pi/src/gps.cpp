/******************************************************************************
 * $Id: gps.cpp, v1.0 2010/08/26 SethDart Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Dashboard Plugin
 * Author:   Jean-Eudes Onfray
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register   *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
 ***************************************************************************
 */

#include "gps.h"
#include "wx28compat.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

// Required deg2rad
#include "dial.h"

DashboardInstrument_GPS::DashboardInstrument_GPS( wxWindow *parent, wxWindowID id, wxString title) :
    DashboardInstrument(parent, id, title, OCPN_DBP_STC_GPS, true)
{
      m_SatCount = 0;
      for (int idx = 0; idx < 12; idx++)
      {
            m_SatInfo[idx].SatNumber = 0;
            m_SatInfo[idx].ElevationDegrees = 0;
            m_SatInfo[idx].AzimuthDegreesTrue = 0;
            m_SatInfo[idx].SignalToNoiseRatio = 0;
      }

      wxClientDC dc(this);
      int w;
      dc.GetTextExtent(m_title, &w, &m_TitleHeight, 0, 0, g_pFontTitle);
      SetMinSize( wxSize(MinWidth, m_TitleHeight+40) );
}

void DashboardInstrument_GPS::SetSatInfo(int cnt, int seq, SAT_INFO sats[4])
{
      m_SatCount = cnt;
      // Some GPS receivers may emit more than 12 sats info
      if (seq < 1 || seq > 3)
            return;

      int lidx = (seq-1)*4;
      for (int idx = 0; idx < 4; idx++)
      {
            m_SatInfo[lidx+idx].SatNumber = sats[idx].SatNumber;
            m_SatInfo[lidx+idx].ElevationDegrees = sats[idx].ElevationDegrees;
            m_SatInfo[lidx+idx].AzimuthDegreesTrue = sats[idx].AzimuthDegreesTrue;
            m_SatInfo[lidx+idx].SignalToNoiseRatio = sats[idx].SignalToNoiseRatio;
      }
}

void DashboardInstrument_GPS::Draw(wxGCDC* dc)
{
      DrawFrame(dc);
      DrawBackground(dc);
      DrawForeground(dc);
}

void DashboardInstrument_GPS::DrawFrame(wxGCDC* dc)
{
      wxSize size = GetClientSize();
      int cx = size.x/2, cy = (size.y-m_TitleHeight-52)/2 + m_TitleHeight;
      int radius = wxMin(cx, cy-m_TitleHeight);
      wxColour cb;

      GetGlobalColor(_T("DASHB"), &cb);
      dc->SetTextBackground(cb);
      dc->SetBackgroundMode(wxSOLID);

      wxColour cl;
      GetGlobalColor(_T("DASHL"), &cl);
      dc->SetTextForeground(cl);
      dc->SetBrush(*wxTRANSPARENT_BRUSH);

      wxPen pen;
      pen.SetStyle(wxPENSTYLE_SOLID);
      wxColour cf;
      GetGlobalColor(_T("DASHF"), &cf);
      pen.SetColour(cf);
      pen.SetWidth(1);
      dc->SetPen(pen);
      
      dc->DrawCircle(cx, cy, radius);

      dc->SetFont(*g_pFontSmall);

      wxScreenDC sdc;
      int height, width;
      sdc.GetTextExtent(_T("W"), &width, &height, NULL, NULL, g_pFontSmall);

      wxBitmap tbm( width, height, -1 );
      wxMemoryDC tdc( tbm );
      tdc.SetBackground( cb );
      tdc.SetTextForeground( cl );
      tdc.SetTextBackground(cb);
      tdc.SetBackgroundMode(wxSOLID);
      tdc.SetFont(*g_pFontSmall );

        tdc.Clear();
        tdc.DrawText(_("N"), 0,0);
        dc->Blit(cx-3, cy-radius-6, width, height, &tdc, 0, 0);

        tdc.Clear();
        tdc.DrawText(_("E"), 0,0);
        dc->Blit(cx+radius-4, cy-5, width, height, &tdc, 0, 0);

        tdc.Clear();
        tdc.DrawText(_("S"), 0,0);
        dc->Blit(cx-3, cy+radius-6, width, height, &tdc, 0, 0);

        tdc.Clear();
        tdc.DrawText(_("W"), 0,0);
        dc->Blit(cx-radius-4, cy-5, width, height, &tdc, 0, 0);

      tdc.SelectObject( wxNullBitmap );

      dc->SetBackgroundMode(wxTRANSPARENT);

      dc->DrawLine(3, size.y-52, size.x-3, size.y-52);
      dc->DrawLine(3, size.y-12, size.x-3, size.y-12);

      pen.SetStyle(wxPENSTYLE_DOT);
      dc->SetPen(pen);
      dc->DrawCircle(cx, cy, radius * sin(deg2rad(45)));
      dc->DrawCircle(cx, cy, radius * sin(deg2rad(20)));

      //        wxSHORT_DASH is not supported on GTK, and it destroys the pen.
#ifndef __WXGTK__
      pen.SetStyle(wxSHORT_DASH);
      dc->SetPen(pen);
#endif
      for(int i=0; i<3; i++)
          dc->DrawLine(3, size.y-22-10*i, size.x-3, size.y-20-10*i);
}

void DashboardInstrument_GPS::DrawBackground(wxGCDC* dc)
{
    wxSize size = GetClientSize();
      // Draw SatID

      wxScreenDC sdc;
      int height, width;
      sdc.GetTextExtent(_T("W"), &width, &height, NULL, NULL, g_pFontSmall);

      wxColour cl;
      wxBitmap tbm( dc->GetSize().x, height, -1 );
      wxMemoryDC tdc( tbm );
      wxColour c2;
      GetGlobalColor( _T("DASHB"), &c2 );
      tdc.SetBackground( c2 );
      tdc.Clear();

      tdc.SetFont(*g_pFontSmall );
      GetGlobalColor( _T("DASHF"), &cl );
      tdc.SetTextForeground( cl );

      for (int idx = 0; idx < 12; idx++)
      {
            if (m_SatInfo[idx].SatNumber)
                  tdc.DrawText(wxString::Format(_T("%02d"), m_SatInfo[idx].SatNumber), idx*16+5, 0);
      }

      tdc.SelectObject( wxNullBitmap );
      dc->DrawBitmap(tbm, 0, size.y-10, false);
}

void DashboardInstrument_GPS::DrawForeground( wxGCDC* dc )
{
    wxSize size = GetClientSize();
    int cx = size.x/2, cy = (size.y-m_TitleHeight-52)/2 + m_TitleHeight;
    int radius = wxMin(cx, cy-m_TitleHeight);

    wxColour cl;
    GetGlobalColor( _T("DASHL"), &cl );
    wxBrush brush( cl );
    dc->SetBrush( brush );
    dc->SetPen( *wxTRANSPARENT_PEN);
    dc->SetTextBackground( cl );

    wxColor cf;
    GetGlobalColor( _T("DASHF"), &cf );
    dc->SetTextForeground( cf );
    dc->SetBackgroundMode( wxSOLID );

    wxColour cb;
    GetGlobalColor( _T("DASHB"), &cb );

    for( int idx = 0; idx < 12; idx++ ) {
        if( m_SatInfo[idx].SignalToNoiseRatio ) {
            int h = m_SatInfo[idx].SignalToNoiseRatio * 0.4;
            dc->DrawRectangle( idx * 16 + 5, size.y - h - 12, 13, h );
        }
    }

    wxString label;
    for( int idx = 0; idx < 12; idx++ ) {
        if( m_SatInfo[idx].SignalToNoiseRatio ) {
            label.Printf( _T("%02d"), m_SatInfo[idx].SatNumber );
            int width, height;
            wxScreenDC sdc;
            sdc.GetTextExtent( label, &width, &height, 0, 0, g_pFontSmall );

            wxBitmap tbm( width, height, -1 );
            wxMemoryDC tdc( tbm );
            tdc.SetBackground( cb );
            tdc.Clear();

            tdc.SetFont(*g_pFontSmall );
            tdc.SetTextForeground( cf );
            tdc.SetBackgroundMode( wxSOLID );
            tdc.SetTextBackground( cl );

            tdc.DrawText( label, 0, 0 );
            tdc.SelectObject( wxNullBitmap );

            int posx = cx + radius * cos( deg2rad( m_SatInfo[idx].AzimuthDegreesTrue - ANGLE_OFFSET ) )
                            * sin( deg2rad( ANGLE_OFFSET - m_SatInfo[idx].ElevationDegrees ) ) - width / 2;
            int posy = cy + radius * sin( deg2rad( m_SatInfo[idx].AzimuthDegreesTrue - ANGLE_OFFSET ) )
                            * sin( deg2rad( ANGLE_OFFSET - m_SatInfo[idx].ElevationDegrees ) ) - height / 2;
             dc->DrawBitmap( tbm, posx, posy, false );
        }
    }

}

