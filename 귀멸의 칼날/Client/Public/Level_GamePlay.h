#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT NativeConstruct() override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Light();
	// 맵 불러오기용 함수 / 파일 불러와서 맵 오브젝트들 생성 
	HRESULT Ready_Map(); 
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Lines(const _tchar* pLayerTag);
	
public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
private:
	// 몬스터 인스턴스 오브젝트 위치값을 위한 벡터 
	vector<_float3> m_vecMonsterPos;
	_int iIndex = 0;
	vector<_matrix> m_vecInstanceMatrix[20];
};

END
