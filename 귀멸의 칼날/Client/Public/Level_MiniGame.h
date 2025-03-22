#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_MiniGame final : public CLevel
{
private:
	CLevel_MiniGame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_MiniGame() = default;

public:
	virtual HRESULT NativeConstruct() override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_NPC(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Light();
	HRESULT Ready_Map();
public:
	static CLevel_MiniGame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
private:
	vector<_matrix> m_vecInstanceMatrix[10];
};

END