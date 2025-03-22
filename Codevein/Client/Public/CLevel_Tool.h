#pragma once

#pragma once
#include "Client_Defines.h"
#include "CLevel.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CLevel_Tool final : public CLevel
{
private:
	explicit CLevel_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Tool() = default;
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
private:
	// Next Level = Logo Level
	const LEVEL m_eNextLevel = LEVEL_LOGO;
public:
	static CLevel_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END