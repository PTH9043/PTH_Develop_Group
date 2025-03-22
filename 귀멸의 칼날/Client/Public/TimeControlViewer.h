#pragma once
#include "ImGuiClass.h"

BEGIN(Client)

class CTimerControlViewer final : public CImGuiClass
{
private:
	explicit CTimerControlViewer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CTimerControlViewer() = default;
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
protected:
	WINDESC								m_tWinDesc;
	list<const _tchar*>			m_ClonesLayerStr;
public:
	static CTimerControlViewer* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END