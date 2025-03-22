#pragma once
#include "Controller.h"

BEGIN(Engine)


class ENGINE_DLL CInput_Controller : public CController
{
public:
	// 들어온 인풋의 종류
	enum INPUTTYPE { DOWN, UP, PRESSING, INPUTTYPE_END };
protected:
	explicit CInput_Controller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CInput_Controller() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(class CGameObject* _pGameObject) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
public:
		// 데이터를 내부에 집어 넣는 함수
		virtual void Insert_Datas(VOIDDATAS _pData) override;
		// 데이터를 바깥으로 빼는 함수
		virtual  VOIDDATAS Out_Datas() override;
protected:
	virtual void Input_Defines();
protected:
	// Input Con Type 
	EInputCon_Type m_eInputConType = INPUTCON_TYPE::TYPE_END;
public:
	virtual void Free() override;
};

END
