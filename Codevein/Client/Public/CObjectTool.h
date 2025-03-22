#pragma once
#include "CIMGUI.h"

BEGIN(Engine)
class CGameObject;
class CModel;
class CRenderer;
END

BEGIN(Client)

class CMainView;
class CObjectTool final : public CIMGUI
{
protected:
	explicit CObjectTool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CObjectTool() = default;
public:
	CGameObject* Get_DebugObject() { return m_pDebugObject; }
#ifdef _DEBUG
	const _bool& Is_WorldObjectDrawCall() const { return m_bWorldObjectDrawCall; }
	const _bool& Is_CharObjectDrawCall() const { return m_bCharObjectDrawCall; }
#endif
public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct_Tool() override;
	virtual HRESULT Tick(const _double& _dDeltaSeconds)  override;
	virtual HRESULT Render()  override;
	virtual HRESULT Release_Tool() override;
public:
	// Set Debug Model
	void Set_DebugObject_Model(CModel* _pModel);
protected:
	virtual void Show_Window() override;
	// Spawn Player Pos 
	void Spawn_Player_Pos();
protected:
	// Reset DebugObject
	void Reset_DebugObject_State();

private:
	// DebugObject
	CGameObject* m_pDebugObject = nullptr;
	// Spawn
	CGameObject* m_pSpawnObject = nullptr;
private:		
private:
#ifdef _DEBUG
	_bool					m_bWorldObjectDrawCall = false;
	_bool					m_bCharObjectDrawCall = false;
	CRenderer* m_pRenderer = nullptr;
#endif
public:
	static CObjectTool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END