#pragma once
#include "CComponent.h"

BEGIN(Engine)
class CTarget_Manager;
class CLight_Manager;
class CVIBuffer_Rect;
class CShader;
class CCollider;

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERID{ 
		RENDER_PRIORITY, RENDER_NONALPHABLEND, RENDER_NONLIGHT, RENDER_ALPHABLEND, RENDER_UI, RENDER_END
	};
protected:
	explicit CRenderer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRenderer() = default;
public:
	void Plus_PadeOutValue(const _float& _fPadeOut);
	void Set_PadeOutValue(const _float& _fPadeOut) { this->m_fPadeOutValue = _fPadeOut; }
	const _float& Get_PadeOutValue() const { return m_fPadeOutValue; }

	void Clear();
public:
	// NativeConstruct Prototype
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
public:
	HRESULT Add_RenderGroup(const RENDERID& _eRenderGroup, class CGameObject* pGameObject);
#ifdef _DEBUG
	// Collider
	HRESULT Add_Components(CComponent* _pComponents);
#endif 
public:
	HRESULT Draw();
private:
	// Priority
	HRESULT Render_Priority();
	// NonAlphaBlend
	HRESULT Render_NonAlphaBlend();
	// Render Lights
	HRESULT Render_Lights();

	HRESULT Render_Blend();
	// Non Light
	HRESULT Render_NonLight();
	// AlphaBlend
	HRESULT Render_AlphaBlend();
	// UI 
	HRESULT Render_UI();
#ifdef _DEBUG
	// Debug
	HRESULT Render_Debug();
#endif
private:
	list<class CGameObject*>					m_RenderGroup[RENDER_END];
	typedef list<class CGameObject*>		RENDERGROUPS;
#ifdef _DEBUG
	typedef list<CComponent*>				COMPONENTS;
	COMPONENTS											m_Components;
#endif
private:
	CTarget_Manager*								m_pTarget_Manager = nullptr;
	CLight_Manager*								m_pLight_Manager = nullptr;
	CVIBuffer_Rect*									m_pVIBufferRect = nullptr;
	CShader*												m_pDefferdShader = nullptr;
	CShader*												m_pBlurShader = nullptr;
	CShader*												m_pAlphaBlendShader = nullptr;
	// Matirx
	_float4x4												m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
	// Pade Out Value
	_float														m_fPadeOutValue = 0.f;
public:
	static CRenderer* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END