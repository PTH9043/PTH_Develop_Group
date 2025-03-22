#include "Font_Manager.h"

IMPLEMENT_SINGLETON(CFont_Manager)

CFont_Manager::CFont_Manager()
{
}

HRESULT CFont_Manager::Add_Font(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
	const _tchar* _pFontTag, const _tchar* _pFontFIlePath, CCustomFont** _ppOut)
{
	if (nullptr != Find_Font(_pFontTag))
		return E_FAIL;

	CCustomFont* pFont = CCustomFont::Create(_pDevice, _pContext, _pFontFIlePath);
	if (nullptr == pFont)
		return E_FAIL;

	m_Fonts.emplace(_pFontTag, pFont);

	if (nullptr != _ppOut)
		*_ppOut = pFont;

	return S_OK;
}

HRESULT CFont_Manager::Add_Font_FileData(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
	const _tchar* _pFontTag, FILEDATA* _pFileData, CCustomFont** _ppOut)
{
	if (nullptr != Find_Font(_pFontTag))
		return E_FAIL;

	CCustomFont* pFont = CCustomFont::Create(_pDevice, _pContext, _pFileData);
	if (nullptr == pFont)
		return E_FAIL;

	m_Fonts.emplace(_pFontTag, pFont);

	if (nullptr != _ppOut)
		*_ppOut = pFont;

	return S_OK;
}

HRESULT CFont_Manager::Render_Font(const _tchar* _pFontTag, const _tchar* _pString, const _float2& _vPos, _fvector& _vColor)
{
	CCustomFont* pFont = Find_Font(_pFontTag);
	if (nullptr == pFont)
		return E_FAIL;

	return pFont->Render(_pString, _vPos, _vColor);
}

HRESULT CFont_Manager::Align_RenderFont(const _tchar* _pFontTag, const _tchar* pString, const _float2& vPosition, _fvector vColor, _bool alignCenter)
{
	CCustomFont* pFont = Find_Font(_pFontTag);
	if (nullptr == pFont)
		return E_FAIL;
	
	return pFont->Align_Render(pString, vPosition, vColor, alignCenter);

}

CCustomFont* CFont_Manager::Find_Font(const _tchar* _pFontTag)
{
	auto	iter = find_if(m_Fonts.begin(), m_Fonts.end(), CTag_Finder(_pFontTag));

	if (iter == m_Fonts.end())
		return nullptr;

	return iter->second;
}

void CFont_Manager::Free()
{
	for (auto& Pair : m_Fonts)
		Safe_Release(Pair.second);

	m_Fonts.clear();
}
