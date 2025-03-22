#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Rattan final : public CLevel
{
private:
	CLevel_Rattan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Rattan() = default;

public:
	virtual HRESULT NativeConstruct() override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_NPC(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Quest(const _tchar* pLayerTag);
	HRESULT Ready_Map();
	HRESULT Ready_Layer_Animal(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Light();

public:
	static CLevel_Rattan* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
private:
	vector<_matrix> m_vecInstanceMatrix[10];
};

END