#pragma once
#include "ImGuiClass.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
END

BEGIN(Engine)

class CObjectView : public CImGuiClass
{
protected:
	typedef struct tagSelectDesc
	{
		CGameObject*					pGameObject = nullptr;
		string									strName = "";
	}SELECTDESC;
private:
	explicit CObjectView(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CObjectView() = default;
public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct_InnerValue() override;
	virtual HRESULT Tick(const _double& _dDeltaSeconds)  override;
	virtual HRESULT Late_Tick(const _double& _dDeltaSeconds);
	virtual HRESULT Render();
	virtual HRESULT Release_InnerValue() override;
protected:
	// Show Windows
	virtual void Show_Window() override;
private:
	// Hierachy Control
	void Hierachy_Control();
	// Perspective 
	void Perspective_Control();
	void Show_Clones_Update(CGameInstance* _pGameInstance);
private:
	list<const _tchar*>															m_ClonesLayerStr;
private:
	DOCKDESC																			m_tMainView;
	DOCKDESC																			m_tPerspective;
	DOCKDESC																			m_tHierarchy;
	_bool																						m_bInitSetting = false;
private:
	map<_string, map<_string, list<SELECTDESC>>>	m_ShowCloneMap;
	_uint m_iShowSize = 0;
	// Select Desc
	SELECTDESC																		 m_tSelectDesc;
public:
	static CObjectView* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END