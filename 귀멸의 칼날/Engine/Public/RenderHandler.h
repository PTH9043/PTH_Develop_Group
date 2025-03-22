#pragma once
#include "Base.h"

BEGIN(Engine)

class CTexture;
	  
class CRenderHandler final : public CBase
{
protected:
	explicit CRenderHandler(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderHandler() = default;
public:
	// NativeConstruct
	HRESULT NativeConstruct();

	HRESULT Begin_MRT(const _tchar* _pMrtName, class CShader* _pShader, RH_VALUE_VEC _valueVec, RH_TEX_VEC _TexVec);
	HRESULT Begin_MRT(const _tchar* _pMrtName, const _tchar* _pShaderTag, RH_VALUE_VEC _valueVec, RH_TEX_VEC _TexVec);

	HRESULT Render_Lights(const _tchar* _pMrtName, const _tchar* _pShaderTag, RH_VALUE_VEC _valueVec, RH_TEX_VEC _TexVec);
	HRESULT End_MRT(const _tchar* _pMRTName);

	HRESULT Rendering(class CShader* _pShader, const _uint& _iPassIndex, 
		class CVIBuffer_Rect* _pRect, RH_VALUE_VEC _valueVec, RH_TEX_VEC _TexVec);
	HRESULT Rendering(const _tchar* _pShaderTag, const _uint& _iPassIndex, RH_VALUE_VEC _valueVec, RH_TEX_VEC _TexVec);

	HRESULT Rendering_Texture(const _tchar* _pMrtName, class CShader* _pShader, const _uint& _iPassIndex,
		class CVIBuffer_Rect* _pRect, RH_VALUE_VEC _valueVec, RH_TEX_VEC _TexVec);
	HRESULT Rendering_Texture(const _tchar* _pMrtName, const _tchar* _pShaderTag, const _uint& _iPassIndex, RH_VALUE_VEC _valueVec, RH_TEX_VEC _TexVec);

	HRESULT Rendering_GameObject(const _tchar* _pMRTName, list<class CGameObject*>* _ObjectList);
	HRESULT Rendering_GameObject(list<class CGameObject*>* _ObjectList);

	HRESULT Bind_Texture(const _tchar* _pShaderTag, const char* _pConstantName, _uint iIndex);


private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	CVIBuffer_Rect*				m_pVIBufferRect = nullptr;

private:
	CTexture*					m_pDeferredTexture = nullptr;

private:
	class CTarget_Manager*		m_pTargetManager = nullptr;

public:
	static CRenderHandler* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

 END