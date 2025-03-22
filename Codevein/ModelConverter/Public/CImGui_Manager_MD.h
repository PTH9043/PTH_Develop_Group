#pragma once
#include "CManager.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Model)
class CMainView_MD;
class CCamTool_MD;
class CModel_FBX;

class CImGui_Manager_MD final : public CManager
{
	DECLARE_SINGLETON(CImGui_Manager_MD)
protected:
	explicit CImGui_Manager_MD();
	virtual ~CImGui_Manager_MD() = default;
public:
	HRESULT NativeConstruct(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	void Tick(const _double& _dDeltaSeconds);
	HRESULT Render();
private:
	// MainVie MD
	CMainView_MD*		m_pMainView = nullptr;
	// CamTool MD
	CCamTool_MD*		m_pCamTool = nullptr;
public:
	virtual void Free() override;
};

END