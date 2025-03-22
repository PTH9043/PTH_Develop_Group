#pragma once
#include "Client_Defines.h"
#include "Player_GamePad_Controller.h"

BEGIN(Client)

class CStory_Tanjiro_GamePad_Controller final : public CPlayer_GamePad_Controller
{
private:
	explicit CStory_Tanjiro_GamePad_Controller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CStory_Tanjiro_GamePad_Controller() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(class CGameObject* _pGameObject) override;
	virtual void Tick(const _double & _dTimeDelta) override;
	virtual void Late_Tick(const _double & _dTimeDelta) override;
public:
	// 데이터를 내부에 집어 넣는 함수
	virtual void Insert_Datas(VOIDDATAS _pData);
	// 데이터를 바깥으로 빼는 함수
	virtual  VOIDDATAS Out_Datas();

public:
	static CStory_Tanjiro_GamePad_Controller* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, class CGameObject* _pGameObject);
	virtual void Free() override;
};

END