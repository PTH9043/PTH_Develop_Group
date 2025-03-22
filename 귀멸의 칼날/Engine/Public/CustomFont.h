#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCustomFont final : public CBase
{
private:
	explicit CCustomFont(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	virtual ~CCustomFont() = default;
public:
	HRESULT Initialize(const _tchar* _pFontFilePath);
	HRESULT Initialize(FILEDATA* _pFileData);
	HRESULT Render(const _tchar* _pString, const _float2& _vPos, _fvector _vColor);

public:
	HRESULT Align_Render(const _tchar* pString, const _float2& vPosition, _fvector vColor,_bool alignCenter);
	
private:
	SpriteFont* m_pSpriteFont = nullptr;
	SpriteBatch* m_pSpriteBatch = nullptr;

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

public:
	static CCustomFont* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pFontFilePath);
	static CCustomFont* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, FILEDATA* _pFileData);
	virtual void Free() override;
};

END