#pragma once
#include "Client_Defines.h"
#include "Base.h"


BEGIN(Client)

class CImGuiClass : public CBase
{
public:
	typedef struct tagWindowDesc
	{
		tagWindowDesc() = default;
		tagWindowDesc(const _string& _strName, const _int& _imGuiFlags, const ImVec2& _v2StartPos = ImVec2(0.f, 0.f),
			const ImVec2& _v2Size = ImVec2(0.f, 0.f),const _bool& _bEnable = true)
			: strName(_strName),
			imGuiWindowFlags(_imGuiFlags), v2Pos(_v2StartPos), v2Size(_v2Size), bEnable(_bEnable)
		{}
		_string 							strName = "";
		_int									imGuiWindowFlags = 0;
		ImVec2							v2Pos = ImVec2(0.f, 0.f);
		ImVec2							v2Size = ImVec2(0.f, 0.f);
		_bool								bEnable = false;
	}WINDESC;

	typedef struct tagDockDesc : public WINDESC
	{
		tagDockDesc() = default;
		tagDockDesc(const _string& _strName, const ImGuiWindowFlags& _imGuiFlags, const ImVec2& _v2StartPos = ImVec2(0.f, 0.f),
			const ImVec2& _v2Size = ImVec2(0.f, 0.f), const _bool& _bEnable = true)
			: WINDESC(_strName, _imGuiFlags, _v2StartPos, _v2Size, _bEnable), iID(0)
		{}
		ImGuiID iID;
	}DOCKDESC;
protected:
	explicit CImGuiClass(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CImGuiClass() = default;

public:
	const _bool& Is_Open() const { return m_bOpen; }
	void Set_Open(const _bool& _bOpen) { m_bOpen = _bOpen; }
	// Get Name
	const char* Get_Name() const { return m_pName; }
public:
	virtual HRESULT NativeConstruct();
	// Native Inner Value (Reset ImGuiClass Vairable)
	virtual HRESULT NativeConstruct_InnerValue();
	virtual HRESULT Tick(const _double& _dTimeDelta);
	virtual HRESULT Late_Tick(const _double& _dTimeDelta);
	virtual HRESULT Render();
	// Release Inner Value (Reset ImGuiClass Vairable)
	virtual HRESULT Release_InnerValue();
protected:
	// Define ImGui (Begin ~ End)
	virtual void Show_Window();
protected:
	// Device, Context
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	// Open
	_bool										m_bOpen = false;
	const char*							m_pName = nullptr;
public:
	virtual void Free() override;
};

END