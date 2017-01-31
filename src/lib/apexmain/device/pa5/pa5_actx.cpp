/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/
 
#include "pa5_actx.h"
#include "exceptions.h"

#include <afxwin.h>
#include <afxdtctl.h>
#include <qwidget.h>

#pragma warning ( disable : 4800 )  //BOOL to bool (FIXME?)

class PA5actx : public CWnd
{
protected:
  DECLARE_DYNCREATE( PA5actx )
public:
  CLSID const& GetClsid();
  virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
            const RECT& rect, CWnd* pParentWnd, UINT nID,
            CCreateContext* pContext = NULL);

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
        UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE,
        BSTR bstrLicKey = NULL);

public:
  BOOL Connect(long Interface, long DevNum);
  BOOL SetAtten(float AttVal);
  float GetAtten();
  BOOL Reset();
  BOOL SetUser(long ParCode, float Val);
  BOOL Display(LPCTSTR Text, long Position);
  long ConnectPA5(LPCTSTR IntName, long DevNum);
  CString GetError();
};

IMPLEMENT_DYNCREATE(PA5actx, CWnd)

CLSID const& PA5actx::GetClsid()
{
  static CLSID const clsid
    = { 0xEC05FCDE, 0x300D, 0x4CE3, { 0x97, 0x74, 0xA4, 0xC3, 0x77, 0x50, 0x7B, 0xD7 } };
  return clsid;
}
BOOL PA5actx::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
          const RECT& rect, CWnd* pParentWnd, UINT nID,
          CCreateContext* pContext )
{
  return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID);
}

  BOOL PA5actx::Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
      UINT nID, CFile* pPersist, BOOL bStorage ,
      BSTR bstrLicKey)
{
  return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
  pPersist, bStorage, bstrLicKey);
}

BOOL PA5actx::Connect(long Interface, long DevNum)
{
  BOOL result;
  static BYTE parms[] = VTS_I4 VTS_I4;
  InvokeHelper(0x1, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Interface, DevNum);
  return result;
}

BOOL PA5actx::SetAtten(float AttVal)
{
  BOOL result;
  static BYTE parms[] = VTS_R4;
  InvokeHelper(0x2, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, AttVal);
  return result;
}

float PA5actx::GetAtten()
{
  float result;
  InvokeHelper(0x3, DISPATCH_METHOD, VT_R4, (void*)&result, NULL);
  return result;
}

BOOL PA5actx::Reset()
{
  BOOL result;
  InvokeHelper(0x4, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
  return result;
}

BOOL PA5actx::SetUser(long ParCode, float Val)
{
  BOOL result;
  static BYTE parms[] = VTS_I4 VTS_R4;
  InvokeHelper(0x5, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, ParCode, Val);
  return result;
}

CString PA5actx::GetError()
{
  CString result;
  InvokeHelper(0x6, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
  return result;
}

BOOL PA5actx::Display(LPCTSTR Text, long Position)
{
  BOOL result;
  static BYTE parms[] = VTS_BSTR VTS_I4;
  InvokeHelper(0x7, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Text, Position);
  return result;
}

long PA5actx::ConnectPA5(LPCTSTR IntName, long DevNum)
{
  long result;
  static BYTE parms[] = VTS_BSTR VTS_I4;
  InvokeHelper(0x8, DISPATCH_METHOD, VT_I4, (void*)&result, parms, IntName, DevNum);
  return result;
}

  //the CWnd container FIXME is this needed??
class PA5Wnd : public CWnd
{
public:
  PA5Wnd(){}
  virtual ~PA5Wnd(){}

  bool mf_bCreate( CRect* a_pRect, HWND hParent )
  {
    AfxEnableControlContainer();

    if( !CWnd::CreateEx( 0, _T("STATIC"), "fl", WS_CHILD | WS_VISIBLE, 0, 0, a_pRect->right, a_pRect->bottom, hParent, (HMENU)1234 ) )
      return false;

    CWnd::GetClientRect( a_pRect );

    if( !m_PA5.CreateControl( m_PA5.GetClsid(), "", WS_VISIBLE, *a_pRect, this, 5000, 0, FALSE, 0 ) )
      return false;

    return true;
  }

  PA5actx* GetPA5Object()
  { return &m_PA5; }

protected:
  PA5actx m_PA5;
};

Pa5ActX::Pa5ActX() :
  m_pContainer( new QWidget() ),
  m_pObj( new PA5Wnd() )
{
  CRect rect( 0, 0, 5, 5 );
  if( !m_pObj->mf_bCreate( &rect, m_pContainer->winId() ) )
    throw( ApexStringException( "Pa5ActX::Pa5ActX(): couldn't initialize ActiveX for Pa5" ) );
}

Pa5ActX::~Pa5ActX()
{
  delete m_pContainer;
  delete m_pObj;
}

bool Pa5ActX::Connect( long Interface, long DevNum )
{
  return m_pObj->GetPA5Object()->Connect( Interface, DevNum );
}

bool Pa5ActX::SetAtten( float AttVal )
{
  return m_pObj->GetPA5Object()->SetAtten( AttVal );
}

float Pa5ActX::GetAtten()
{
  return m_pObj->GetPA5Object()->GetAtten();
}

bool Pa5ActX::Reset()
{
  return m_pObj->GetPA5Object()->Reset();
}

bool Pa5ActX::SetUser( long ParCode, float Val )
{
  return m_pObj->GetPA5Object()->SetUser( ParCode, Val );
}

bool Pa5ActX::Display( char* Text, long Position )
{
  return m_pObj->GetPA5Object()->Display( Text, Position );
}

long Pa5ActX::ConnectPA5( char* IntName, long DevNum )
{
  return m_pObj->GetPA5Object()->ConnectPA5( IntName, DevNum );
}
