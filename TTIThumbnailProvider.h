#pragma once

#include <windows.h>
#include <thumbcache.h>
#include <shlwapi.h>
#include <new>

// {F8A7B9C2-1234-5678-9ABC-DEF012345678}
// Generate your own GUID using guidgen.exe or online tool
static const GUID CLSID_TTIThumbnailProvider = 
{ 0xF8A7B9C2, 0x1234, 0x5678, { 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78 } };

class TTIThumbnailProvider : 
    public IInitializeWithStream,
    public IThumbnailProvider
{
public:
    TTIThumbnailProvider();

    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // IInitializeWithStream
    IFACEMETHODIMP Initialize(IStream *pStream, DWORD grfMode);

    // IThumbnailProvider
    IFACEMETHODIMP GetThumbnail(UINT cx, HBITMAP *phbmp, WTS_ALPHATYPE *pdwAlpha);

private:
    ~TTIThumbnailProvider();

    long m_cRef;
    IStream *m_pStream;
};
