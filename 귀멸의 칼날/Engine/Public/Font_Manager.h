#pragma once
#include "Base.h"
#include "CustomFont.h"

BEGIN(Engine)

class CFont_Manager final : public CBase
{
	DECLARE_SINGLETON(CFont_Manager);
private:
	explicit CFont_Manager();
	virtual ~CFont_Manager() = default;

public:
	HRESULT Add_Font(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		const _tchar* _pFontTag, const _tchar* _pFontFIlePath, CCustomFont** _ppOut = nullptr);
	HRESULT Add_Font_FileData(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		const _tchar* _pFontTag, FILEDATA* _pFileData, CCustomFont** _ppOut = nullptr);

	// Render Font
	HRESULT Render_Font(const _tchar* _pFontTag, const _tchar* _pString, const _float2& _vPos, _fvector& _vColor);
	HRESULT Align_RenderFont(const _tchar* _pFontTag, const _tchar* pString, const _float2& vPosition, _fvector vColor, _bool alignCenter);

	CCustomFont* Find_Font(const _tchar* _pFontTag);

private:
	typedef map<const _tchar*, CCustomFont*> FONTS;
	FONTS m_Fonts;

public:
	virtual void Free() override;
};

END