#pragma once
#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Tool)
class CCutSceneEffect_Tool;

class CCutSceneEffectGroup_Tool final : public CBase
{
private:
	explicit CCutSceneEffectGroup_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CCutSceneEffectGroup_Tool() = default;
public:
	// Native Construct 
	HRESULT NativeConstruct(VOIDDATAS* _pDatas);
	// Render Show Effect
	HRESULT Render_EffectVector();
	// ExternEffect Value
	void Extern_Value(_wstring _strSelectEffectName, const _uint& _iIndex);
	// Save Load
	void Save_CutSceneTex(HANDLE& _hFile, DWORD* _pByte);
	void Load_CutSceneTex(HANDLE& _hFile, DWORD* _pByte);
private:
	// Cut Scene Effect Vector
	vector<CCutSceneEffect_Tool*>		m_CutSceneEffectVector;
	CCutSceneEffect_Tool*						m_pCutSceneEffectData = nullptr;
	// Device Context
	ID3D11Device*										m_pDevice = nullptr;
	ID3D11DeviceContext*							m_pContext = nullptr;
public:
	static CCutSceneEffectGroup_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, VOIDDATAS* _pDatas = nullptr);
	virtual void Free() override;
};

END