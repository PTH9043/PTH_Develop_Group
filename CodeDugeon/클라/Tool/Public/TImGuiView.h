#pragma once

#include "UObject.h"

/*
@ Date: 2024-02-04, Writer: 이성현, 박태현
@ Explain
- Imgui 뷰들의 부모 클래스
*/

BEGIN(Engine)
class UGameInstance;
class UGpuCommand;
END

BEGIN(Tool)

class TImGuiView : public UObject {
public:
	typedef struct tagMainDesc {
		tagMainDesc() = default;
		tagMainDesc(const ImGuiWindowFlags& _imGuiFlags,
			const ImGuiDockNodeFlags& _imgDockNodeFlags,
			const ImVec2& _v2StartPos = ImVec2(0.f, 0.f),
			const ImVec2& _v2Size = ImVec2(0.f, 0.f))
			: imgWindowFlags(_imGuiFlags), imgDockNodeFlags(_imgDockNodeFlags), vPos(_v2StartPos),
			vSize(_v2Size)
		{
		}
		ImGuiWindowFlags	imgWindowFlags{ 0 };
		ImGuiDockNodeFlags	imgDockNodeFlags{ 0 };
		ImVec2				vPos{ 0, 0 };
		ImVec2				vSize{ 0, 0 };
		_uint				iDockSpaceID{ 0 };
	}MAINDESC;

	typedef struct tagDockDesc : public MAINDESC {
		tagDockDesc() = default;
		tagDockDesc(const _string& _strName, const ImGuiWindowFlags& _imGuiFlags, const ImGuiDockNodeFlags& _imgDockNodeFlags = ImGuiDockNodeFlags_None,
			const ImVec2& _v2StartPos = ImVec2(0.f, 0.f),
			const ImVec2& _v2Size = ImVec2(0.f, 0.f))
			: strName(_strName), MAINDESC(_imGuiFlags, _imgDockNodeFlags, _v2StartPos, _v2Size)
		{}
		_string				strName{ "" };
	}DOCKDESC;

public:
	TImGuiView(CSHPTRREF<UDevice> _spDevice, const _string& _strName);
	NO_COPY(TImGuiView)
	DESTRUCTOR(TImGuiView)
public:
	CSHPTRREF<UGameInstance> GetGameInstance() { return m_spGameInstance; }
	const _string& GetName() const { return m_strName; }
	const _bool IsActive() const { return m_isActive; }
	const _bool IsOpen() const { return m_isOpen; }

	void SetOpen(const _bool _isOpen) { this->m_isOpen = _isOpen; }

public:
	virtual void Free() override PURE;
	HRESULT NativeConstruct() override PURE;

	void Tick(const _double& _dTimeDelta) { (this->*m_pTickFunc)(_dTimeDelta); }
	void LateTick(const _double& _dTimeDelta) { (this->*m_pLateTickFunc)(_dTimeDelta); }
	void Render() { (this->*m_pRenderFunc)(); }

	void OpenImGui();
	void CloseImGui();

	_bool* GetOpenPointer() { return &m_isOpen; }

protected:
	virtual HRESULT LoadResource() PURE;
	virtual HRESULT ReleaseResource() PURE;
	// Tick, LateTick, Render
	virtual void TickActive(const _double& _dTimeDelta) PURE;
	virtual void LateTickActive(const _double& _dTimeDetla) PURE;
	virtual void RenderActive() PURE;
	// 해당하는 함수가 있어야지 삭제함  @Writer: 박태현
	void ActiveResetSceneData();
private:
	void TickNonActive(const _double& _dTimeDelta) {}
	void LateTickNonActive(const _double& _dTimeDelta) {}
	void RenderNonActive() { }
private:
	// Tick, LateTick, Render
	void		(TImGuiView::* m_pTickFunc)(const _double&);
	void		(TImGuiView::* m_pLateTickFunc)(const _double&);
	void		(TImGuiView::* m_pRenderFunc)();

	SHPTR<UGameInstance>	m_spGameInstance;

	_string									m_strName;
	_bool										m_isOpen;
	_bool										m_isActive;

};

END
