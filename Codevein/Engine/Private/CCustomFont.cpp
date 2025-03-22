#include "CCustomFont.h"
#include "CFile_Manager.h"

CCustomFont::CCustomFont(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice), m_pContext(_pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CCustomFont::Initialize(const _tchar* _pFontFilePath)
{
    m_pSpriteFont = new SpriteFont(m_pDevice, _pFontFilePath);
    m_pSpriteBatch = new SpriteBatch(m_pContext);
    return S_OK;
}

HRESULT CCustomFont::Initialize(FILEDATA* _pFileData)
{
    if (nullptr == _pFileData)
        return E_FAIL;

    m_pSpriteFont = new SpriteFont(m_pDevice, _pFileData->strFilePath.c_str());
    m_pSpriteBatch = new SpriteBatch(m_pContext);
    return S_OK;
}

HRESULT CCustomFont::Render(const _tchar* _pString, const _float2& _vPos, _fvector _vColor)
{
    if (nullptr == m_pSpriteFont ||
        nullptr == m_pSpriteBatch)
        return E_FAIL;

    m_pContext->GSSetShader(nullptr, nullptr, 0);
    m_pSpriteBatch->Begin();
    m_pSpriteFont->DrawString(m_pSpriteBatch, _pString, _vPos, _vColor);
    m_pSpriteBatch->End();
    return S_OK;
}

CCustomFont* CCustomFont::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pFontFilePath)
{
    CCustomFont* pCustomFont = new CCustomFont(_pDevice, _pContext);
    if (FAILED(pCustomFont->Initialize(_pFontFilePath)))
    {
        Safe_Release(pCustomFont);
        MSG_BOX("CCustomFont::Creat To Failed");
    }
    return pCustomFont;
}
CCustomFont* CCustomFont::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, FILEDATA* _pFileData)
{
    CCustomFont* pCustomFont = new CCustomFont(_pDevice, _pContext);
    if (FAILED(pCustomFont->Initialize(_pFileData)))
    {
        Safe_Release(pCustomFont);
        MSG_BOX("CCustomFont::Create To Failed");
    }
    return pCustomFont;
}

void CCustomFont::Free()
{
    Safe_Delete(m_pSpriteFont);
    Safe_Delete(m_pSpriteBatch);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
