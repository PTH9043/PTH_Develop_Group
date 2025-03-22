#pragma once
#include "KeyBoard_InputController.h"
#include "Client_Defines.h"

BEGIN(Client)
class CPlayer;

class CPlayer_KeyBoard_Controller : public CKeyBoard_InputController
{
public:
	typedef struct tagCondition
	{
		// 키 입력을 받아서 키를 만들어주는 input 목록
		vector<INPUTDESC> input;				
	}CONDITION;

protected:
	explicit CPlayer_KeyBoard_Controller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CPlayer_KeyBoard_Controller() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(class CGameObject* _pGameObject) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;

public:
	// 데이터를 내부에 집어 넣는 함수
	virtual	 void Insert_Datas(VOIDDATAS _pData) override;
	// 데이터를 바깥으로 빼는 함수
	virtual  VOIDDATAS Out_Datas() override;
	
protected:
	// 동시 키입력은 2개까지만 ( 우선은 )
	_bool Make_Trigger(_string _strTrigger, _bool* pFirstInput, _bool* pSecondInput = nullptr);
	// 트리거를 추가한다.
	void Add_Trigger(_string strTrigger);

protected:
	// 카메라 기준으로 만들어진 방향벡터 ( 플레이어한테 적용함. 해당 방향벡터가 0인지 아닌지로 다양한 판단 가능 )
	_float3				m_vPlayerDir;
	// 트리거 리스트 ( InputController에서 입력된 키값을 갖고 만들어짐. )
	vector<_string>		m_TriggerList;
	// 조건 리스트
	vector<CONDITION>	m_ConditionList;
	// 플레이어 객체
	CPlayer*			m_pPlayer = nullptr;
public:
	virtual void Free() override;
};

END