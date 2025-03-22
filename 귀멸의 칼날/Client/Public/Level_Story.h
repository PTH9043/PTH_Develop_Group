
#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Story final : public CLevel
{
private:
	CLevel_Story(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Story() = default;

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
	HRESULT Ready_Layer_Animal(const _tchar* pLayerTag);
	HRESULT Ready_Layer_NPC(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Quest(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Map(const _tchar* pLayerTag);
	
public:
	static CLevel_Story* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
private:
	vector<_float3> m_vecMonsterPos;
	vector<_matrix> m_vecInstanceMatrix[30];
};

END
