/******************************************************************
***
***
***				FREE WINDOWLESS FLASH CONTROL
***
***					   by Makarov Igor
***
***		for questions and remarks mailto: mak_july@list.ru
***
***
*******************************************************************/
// FlashWnd.cpp: implementation of the COleContainerWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "flashwnd.h"

//DirectDraw GUIDS

/*DEFINE_GUID2(CLSID_DirectDraw,0xD7B70EE0,0x4340,0x11CF,0xB0,0x63,0x00,0x20,0xAF,0xC2,0xCD,0x35);
DEFINE_GUID2(CLSID_DirectDraw7,0x3c305196,0x50db,0x11d3,0x9c,0xfe,0x00,0xc0,0x4f,0xd9,0x30,0xc5);

DEFINE_GUID2(IID_IDirectDraw,0x6C14DB80,0xA733,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60);
DEFINE_GUID2(IID_IDirectDraw3,0x618f8ad4,0x8b7a,0x11d0,0x8f,0xcc,0x0,0xc0,0x4f,0xd9,0x18,0x9d);
DEFINE_GUID2(IID_IDirectDraw4,0x9c59509a,0x39bd,0x11d1,0x8c,0x4a,0x00,0xc0,0x4f,0xd9,0x30,0xc5);
DEFINE_GUID2(IID_IDirectDraw7,0x15e65ec0,0x3b9c,0x11d2,0xb9,0x2f,0x00,0x60,0x97,0x97,0xea,0x5b);*/


CFlashWnd::CFlashWnd()
{
	m_lVersion = 0;
	//m_lpDD4 = NULL;
}
CFlashWnd::~CFlashWnd()
{
	/*if (m_lpDD4)
		m_lpDD4->Release();*/
}

//DShockwaveFlashEvents
HRESULT STDMETHODCALLTYPE CFlashWnd::OnReadyStateChange(long )
{
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CFlashWnd::OnProgress(long )
{
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CFlashWnd::FSCommand(_bstr_t , _bstr_t )
{
	return S_OK;
}

void CFlashWnd::OnErrorClassNotReg()
{
	//some notification code here
}
BOOL CFlashWnd::OnBeforeShowingContent()
{
	m_lVersion = m_lpControl->FlashVersion();
	if ((m_lVersion & 0x00FF0000) == 0x00080000)
		m_bFixTransparency = TRUE;
	else
		m_bFixTransparency = FALSE;
	HRESULT hr;
	hr = m_lpControl->QueryInterface(IID_IConnectionPointContainer, (void**)&m_lpConCont);
	if (FAILED(hr))
		return FALSE;
	hr = m_lpConCont->FindConnectionPoint(ShockwaveFlashObjects::DIID__IShockwaveFlashEvents, &m_lpConPoint);
	if (FAILED(hr))
		return FALSE;
	hr = m_lpConPoint->Advise((ShockwaveFlashObjects::_IShockwaveFlashEvents *)this, &m_dwConPointID);
	if (FAILED(hr))
		return FALSE;

	if (m_bTransparent)
		m_lpControl->PutWMode(L"transparent");
	m_lpControl->PutScale(L"showAll");
	//m_lpControl->PutBackgroundColor(0x00000000);
	m_lpControl->PutEmbedMovie(FALSE);
	return TRUE;
}
BOOL CFlashWnd::OnAfterShowingContent()
{
	//HRESULT hr;
	m_lpControl->PutEmbedMovie(TRUE);
  //hr = m_lpControl->LoadMovie(0, L"c:\\test.swf");
//	hr = m_lpControl->LoadMovie(0, L"c:\\mailru.swf");
	//if (FAILED(hr))
	//	return FALSE;
	//hr = m_lpControl->Play();
	//if (FAILED(hr))
	//	return FALSE;
	return TRUE;
}

//IUnknown

HRESULT STDMETHODCALLTYPE CFlashWnd::QueryInterface(REFIID riid, void ** ppvObject)
{
	HRESULT hr = COleContainerWnd<ShockwaveFlashObjects::IShockwaveFlash>::QueryInterface(riid, ppvObject);
	if (hr != E_NOINTERFACE)
		return hr;
	if (IsEqualGUID(riid, ShockwaveFlashObjects::DIID__IShockwaveFlashEvents))
		*ppvObject = (void*)dynamic_cast<ShockwaveFlashObjects::_IShockwaveFlashEvents *>(this);
	/*else if (IsEqualGUID(riid, ShockwaveFlashObjects::IID_IServiceProvider))
		*ppvObject = (void*)dynamic_cast<ShockwaveFlashObjects::IServiceProvider *>(this);*/
	else
	{
		*ppvObject = 0;
		return E_NOINTERFACE;
	}
	if (!(*ppvObject))
		return E_NOINTERFACE; //if dynamic_cast returned 0
	m_iRef++;
	return S_OK;
}

ULONG STDMETHODCALLTYPE CFlashWnd::AddRef()
{
	m_iRef++;
	return m_iRef;
}

ULONG STDMETHODCALLTYPE CFlashWnd::Release()
{
	m_iRef--;
	return m_iRef;
}

//IDispatch
HRESULT STDMETHODCALLTYPE CFlashWnd::GetTypeInfoCount(UINT __RPC_FAR *)
{
	NOTIMPLEMENTED;
}
HRESULT STDMETHODCALLTYPE CFlashWnd::GetTypeInfo( 
    /* [in] */ UINT ,
    /* [in] */ LCID ,
    /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *)
{
	NOTIMPLEMENTED;
}
HRESULT STDMETHODCALLTYPE CFlashWnd::GetIDsOfNames( 
    /* [in] */ REFIID ,
    /* [size_is][in] */ LPOLESTR __RPC_FAR *,
    /* [in] */ UINT ,
    /* [in] */ LCID ,
    /* [size_is][out] */ DISPID __RPC_FAR *)
{
	NOTIMPLEMENTED;
}
HRESULT STDMETHODCALLTYPE CFlashWnd::Invoke( 
    /* [in] */ DISPID ,
    /* [in] */ REFIID ,
    /* [in] */ LCID ,
    /* [in] */ WORD ,
    /* [out][in] */ DISPPARAMS __RPC_FAR *,
    /* [out] */ VARIANT __RPC_FAR *,
    /* [out] */ EXCEPINFO __RPC_FAR *,
    /* [out] */ UINT __RPC_FAR *)
{
/*	switch (dispIdMember)
	{
	default:
		return S_OK;
	}*/
	return S_OK;
}

HRESULT __stdcall CFlashWnd::raw_RemoteQueryService (
       GUID * ,
       GUID * ,
       IUnknown * *  )
{
	/*HRESULT hr;
	if (IsEqualGUID(*guidService, IID_IDirectDraw3))
	{
		if (!m_lpDD4)
		{
			m_lpDD4 = new IDirectDraw4Ptr;
			hr = m_lpDD4->CreateInstance(CLSID_DirectDraw, NULL, CLSCTX_INPROC_SERVER); 
			if (FAILED(hr))
			{
				delete m_lpDD4;
				m_lpDD4 = NULL;
				return E_NOINTERFACE;
			}
		}
		if (m_lpDD4 && m_lpDD4->GetInterfacePtr())
		{
			*ppvObject = m_lpDD4->GetInterfacePtr();
			m_lpDD4->AddRef();
			return S_OK;
		}
	}*/
	return E_NOINTERFACE;
}
