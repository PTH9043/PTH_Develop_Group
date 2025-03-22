#include "CustomFont.h"
#include "FilePath_Manager.h"

CCustomFont::CCustomFont(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice), m_pContext(_pContext)
{
    Safe_AddRef_DV(m_pDevice);
    Safe_AddRef_DV(m_pContext);
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

HRESULT CCustomFont::Align_Render(const _tchar* pString, const _float2& vPosition, _fvector vColor, _bool alignCenter)
{
    if (nullptr == m_pSpriteFont ||
        nullptr == m_pSpriteBatch)
        return E_FAIL;

    m_pContext->GSSetShader(nullptr, nullptr, 0);

    _vector color = XMVectorSetW(Colors::Black, XMVectorGetW(vColor));
	
    m_pSpriteBatch->Begin();
	
    if (true == alignCenter)   //±âÁ¸ 1.f
    {
        _float2 origin;
        XMStoreFloat2(&origin, m_pSpriteFont->MeasureString(pString) / 2.f);

        /*m_pSpriteFont->DrawString(m_pSpriteBatch, pString, _float2(vPosition.x + 1.f, vPosition.y + 1.f), color, 0.f, origin);
        m_pSpriteFont->DrawString(m_pSpriteBatch, pString, _float2(vPosition.x - 1.f, vPosition.y + 1.f), color, 0.f, origin);
        m_pSpriteFont->DrawString(m_pSpriteBatch, pString, _float2(vPosition.x - 1.f, vPosition.y - 1.f), color, 0.f, origin);
        m_pSpriteFont->DrawString(m_pSpriteBatch, pString, _float2(vPosition.x + 1.f, vPosition.y - 1.f), color, 0.f, origin);

        m_pSpriteFont->DrawString(m_pSpriteBatch, pString, vPosition, vColor, 0.f, origin);*/
        _float2 vPos = _float2(vPosition.x - origin.x , vPosition.y - origin.y);
		
        m_pSpriteFont->DrawString(m_pSpriteBatch, pString, _float2(vPos.x + 1.f, vPos.y + 1.f), color);
        m_pSpriteFont->DrawString(m_pSpriteBatch, pString, _float2(vPos.x - 1.f, vPos.y + 1.f), color);
        m_pSpriteFont->DrawString(m_pSpriteBatch, pString, _float2(vPos.x - 1.f, vPos.y - 1.f), color);
        m_pSpriteFont->DrawString(m_pSpriteBatch, pString, _float2(vPos.x + 1.f, vPos.y - 1.f), color);

        m_pSpriteFont->DrawString(m_pSpriteBatch, pString, vPos);
    }
	
    else
    {
      
        m_pSpriteFont->DrawString(m_pSpriteBatch, pString, _float2(vPosition.x + 1, vPosition.y + 1), color);
        m_pSpriteFont->DrawString(m_pSpriteBatch, pString, _float2(vPosition.x - 1, vPosition.y + 1), color);
        m_pSpriteFont->DrawString(m_pSpriteBatch, pString, _float2(vPosition.x - 1, vPosition.y - 1), color);
        m_pSpriteFont->DrawString(m_pSpriteBatch, pString, _float2(vPosition.x + 1, vPosition.y - 1), color);

        m_pSpriteFont->DrawString(m_pSpriteBatch, pString, vPosition);
    }

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
    Safe_Release_DV(m_pDevice);
    Safe_Release_DV(m_pContext);
}
