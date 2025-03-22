#pragma once

#pragma once
#include "Tool_Defines.h"
#include "Level.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tool)

class CLevel_Main_Tool final : public CLevel
{
private:
	explicit CLevel_Main_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Main_Tool() = default;
public:
	virtual HRESULT NativeConstruct() override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	// Ready Light Desc
	HRESULT Ready_LightDesc();
	// Ready Layer Camera
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
    //Ready UI
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	
private:
	// Next Level = Logo Level
	const LEVEL m_eNextLevel = LEVEL_TOOL;
	
public:
	static CLevel_Main_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END