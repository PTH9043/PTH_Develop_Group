#pragma once
#include "ImGuiClass.h"


BEGIN(Client)
class CPlayer;
class CAI_CharController;

class CAIConViewer final : public CImGuiClass
{
protected:
	explicit CAIConViewer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CAIConViewer() = default;

public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct_InnerValue() override;
	virtual HRESULT Tick(const _double& _dDeltaSeconds)  override;
	virtual HRESULT Late_Tick(const _double& _dDeltaSeconds);
	virtual HRESULT Release_InnerValue() override;

protected:
	// Show Window
	virtual void Show_Window() override;
	// Find Current AI
	void Find_Current_AI_Battle();
protected:
	CPlayer* m_pAIPlayer												= nullptr;
	CAI_CharController* m_pAICharController			= nullptr;
public:
	static CAIConViewer* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END