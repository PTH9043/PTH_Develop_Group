#pragma once
#include "CIMGUI_MD.h"

BEGIN(Engine)
class CGameObject;
class CModel;
END

BEGIN(Model)
class CModel_FBX;

class CMainView_MD final : public CIMGUI_MD
{
	typedef struct tagDockDesc : public WINDESC
	{
		tagDockDesc() = default;
		tagDockDesc(const char* _pName, const _uint& _imGuiFlags, const _float2& _v2StartPos = _float2(0.f, 0.f),
			const _float2& _v2Size = _float2(0.f, 0.f), const _bool& _bEnable = true)
			: WINDESC(_pName, _imGuiFlags, _v2StartPos, _bEnable), v2Size(_v2Size), iID(0)
		{}
		_float2 v2Size;
		_uint iID;
	}DOCKDESC;
protected:
	explicit CMainView_MD(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CMainView_MD() = default;
public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT Tick(const _double& _dDeltaSeconds)  override;
	virtual HRESULT Render()  override;
public:
	// Set Debug Model
	void Set_DebugObject_Model(CModel_FBX* _pModel);
	// Set 
	void Set_DebugObject_Model(CModel* _pModel);
	// Set Debug Model
	void Set_AnimDebugObject_Model(CModel_FBX* _pModel);
	// Set 
	void Set_AnimDebugObject_Model(CModel* _pModel);
protected:
	virtual void Show_Window() override;
private:
	// Ready ImGuiClass
	HRESULT Ready_ImGuiClass();
private:
	DOCKDESC							m_tMainView;
private:
	_bool										m_bInitSetting = false;
private:
	// Debug Object
	CGameObject*						m_pDebugObject = nullptr;
	// Anim Debug Object
	CGameObject*						m_pAnimDebugObject = nullptr;

	// Anim Model
	CModel_FBX*						m_pModelFBX = nullptr;
	// Anim Model
	CModel*								m_pModel = nullptr;

	_uint m_iAnimationIndexFBX = 0;
	_uint m_iAnimationIndex = 0;
public:
	static CMainView_MD* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END