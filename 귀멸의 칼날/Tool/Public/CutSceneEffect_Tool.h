#pragma once
#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CTransform;
class CGameInstance;
END

BEGIN(Tool)

class CCutSceneEffect_Tool final : public CBase
{
private:
	explicit CCutSceneEffect_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CCutSceneEffect_Tool() = default;
public:
	const _wstring& Get_Name() const { return m_strEffectName; }
public:
	// Native Construct 
	HRESULT NativeConstruct(VOIDDATAS* _pDatas);
	// Render Show Effect
	HRESULT Render_Effect();
	// ExternEffect Value
	void Extern_EffectValue();
	// Draw Effect 
	void Draw_EffectValue(CGameInstance* _pGameInstance);
	// Save Load
	void Save_CutSceneTex(HANDLE& _hFile, DWORD* _pByte);
	void Load_CutSceneTex(HANDLE& _hFile, DWORD* _pByte);
private:
	ID3D11Device*							m_pDevice = nullptr;
	ID3D11DeviceContext*				m_pContext = nullptr;
	// Transform  Com
	CTransform*								m_pTransformCom = nullptr;
	// Effect Name
	_wstring										m_strEffectName = L"";
public:
	static CCutSceneEffect_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, VOIDDATAS* _pDatas = nullptr);
	virtual void Free() override;
};

END