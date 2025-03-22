#pragma once
#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Tool)
class CImGuiClass_Tool;

class CImGuiClass_Manager_Tool final : public CBase
{
	DECLARE_SINGLETON(CImGuiClass_Manager_Tool)
public:

protected:
	explicit CImGuiClass_Manager_Tool();
	virtual ~CImGuiClass_Manager_Tool() = default;
public:
	CImGuiClass_Tool* Get_ImGuiClass(const _uint& _iIndex);
public:
	// NativeConstruct
	HRESULT NativeConstruct(HWND _hWnd, ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext
	, const _uint& _iImGuiClassCount = 1);
	// NativeConstruct InnerValue
	HRESULT NativeConstruct_InnerValue();
	// Insert ImGui Class;
	void Insert_ImGuiClass(const _uint& _iIndex, CImGuiClass_Tool* _pImGuiClass);
public:
	void Tick(const _double& _dTimeDelta);
	HRESULT Late_Tick(const _double& _dTimeDelta);
	HRESULT Render();
private:
	// Is Find Same 
	_bool Is_FindSame(CImGuiClass_Tool* _pImGuiClass);
private:
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
private:
	// Imort ImGui Class List;
	vector<CImGuiClass_Tool*>		m_ImGuiClassList;
	// ImGui Class Count
	_uint										m_iImGuiClassCount = 0;
	_bool m_bState = false;
public:
	virtual void Free() override;
};

END