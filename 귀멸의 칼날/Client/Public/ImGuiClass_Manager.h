#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CRefDebugActor;
END

BEGIN(Client)
class CImGuiClass;

class CImGuiClass_Manager final : public CBase
{
	DECLARE_SINGLETON(CImGuiClass_Manager)
public:

protected:
	explicit CImGuiClass_Manager();
	virtual ~CImGuiClass_Manager() = default;
public:
	CImGuiClass* Get_ImGuiClass(const _uint& _iIndex);

	const _bool& Is_ImGuiActive() const { return m_bImGui_Active; }
public:
#ifdef _DEBUG
	// Stop Frame
	_bool* Is_StopFrame() { return &m_bStopFrame; }
#endif
public:
	// NativeConstruct
	HRESULT NativeConstruct(HWND _hWnd, ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext
	, const _uint& _iImGuiClassCount = 1);
	// Insert ImGui Class;
	void Insert_ImGuiClass(const _uint& _iIndex, CImGuiClass* _pImGuiClass);
public:
	void Tick(const _double& _dTimeDelta);
	HRESULT Late_Tick(const _double& _dTimeDelta);
	HRESULT Render();
private:
	// Is Find Same 
	_bool Is_FindSame(CImGuiClass* _pImGuiClass);
private:
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
private:
	CRefDebugActor* m_pRefDebugActor = nullptr;
private:
	// Imort ImGui Class List;
	vector<CImGuiClass*>		m_ImGuiClassList;
	// ImGui Class Count
	_uint										m_iImGuiClassCount = 0;
	// ImGui Active 
	_bool										m_bImGui_Active = false;
#ifdef _DEBUG
	_bool										m_bStopFrame = false;
#endif
public:
	virtual void Free() override;
};

END