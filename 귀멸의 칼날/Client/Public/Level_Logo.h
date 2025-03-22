#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Logo final : public CLevel
{
private:
	CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Logo() = default;

public:
	virtual HRESULT NativeConstruct() override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);


public:
	static CLevel_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END