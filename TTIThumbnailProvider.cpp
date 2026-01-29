#include "TTIThumbnailProvider.h"
#include "TeletextRenderer.h"
#include <vector>

extern long g_cDllRef;

TTIThumbnailProvider::TTIThumbnailProvider() : m_cRef(1), m_pStream(NULL)
{
    InterlockedIncrement(&g_cDllRef);
}

TTIThumbnailProvider::~TTIThumbnailProvider()
{
    if (m_pStream)
    {
        m_pStream->Release();
    }
    InterlockedDecrement(&g_cDllRef);
}

#pragma region IUnknown

IFACEMETHODIMP TTIThumbnailProvider::QueryInterface(REFIID riid, void **ppv)
{
    static const QITAB qit[] = 
    {
        QITABENT(TTIThumbnailProvider, IThumbnailProvider),
        QITABENT(TTIThumbnailProvider, IInitializeWithStream),
        { 0 },
    };
    return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) TTIThumbnailProvider::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

IFACEMETHODIMP_(ULONG) TTIThumbnailProvider::Release()
{
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (0 == cRef)
    {
        delete this;
    }
    return cRef;
}

#pragma endregion

#pragma region IInitializeWithStream

IFACEMETHODIMP TTIThumbnailProvider::Initialize(IStream *pStream, DWORD grfMode)
{
    // Release any existing stream
    if (m_pStream)
    {
        m_pStream->Release();
        m_pStream = NULL;
    }
    
    // Store the stream
    return pStream->QueryInterface(&m_pStream);
}

#pragma endregion

#pragma region IThumbnailProvider

IFACEMETHODIMP TTIThumbnailProvider::GetThumbnail(UINT cx, HBITMAP *phbmp, WTS_ALPHATYPE *pdwAlpha)
{
    if (!m_pStream || !phbmp || !pdwAlpha)
        return E_INVALIDARG;
    
    *phbmp = NULL;
    *pdwAlpha = WTSAT_RGB;
    
    HRESULT hr = S_OK;
    
    // Get stream size
    STATSTG stat;
    hr = m_pStream->Stat(&stat, STATFLAG_NONAME);
    if (FAILED(hr))
        return hr;
    
    // Read the entire file
    ULONG cbRead;
    std::vector<uint8_t> buffer(stat.cbSize.LowPart);
    
    LARGE_INTEGER li = {};
    hr = m_pStream->Seek(li, STREAM_SEEK_SET, NULL);
    if (FAILED(hr))
        return hr;
    
    hr = m_pStream->Read(buffer.data(), stat.cbSize.LowPart, &cbRead);
    if (FAILED(hr))
        return hr;
    
    // Parse TTI file
    TeletextPage page;
    if (!page.ParseTTI(buffer))
        return E_FAIL;
    
    // Render to bitmap
    // Use 4:3 aspect ratio typical of teletext displays
    UINT height = (cx * 3) / 4;
    HBITMAP hBitmap = page.RenderToBitmap(cx, height);
    
    if (!hBitmap)
        return E_FAIL;
    
    *phbmp = hBitmap;
    return S_OK;
}

#pragma endregion
