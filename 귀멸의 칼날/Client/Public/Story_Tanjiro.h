#pragma once
#include "Player.h"

BEGIN(Client)

class CStory_Tanjiro final : public CPlayer
{
public:
	enum  INPUTDATA : _uint
	{
		INPUT_ANIMTYPE = 0, INPUT_TRANSITIONLIST,
		INPUT_CURANIMATION, INPUT_PREVANIMATION,

		INPUT_END
	}INPUTDATA;

	enum OUTDATA : _uint
	{
		OUT_STATE = 0, OUT_DIRECTION, OUT_TARGET_TRANSFORM,
		OUT_END
	};

protected:
	explicit CStory_Tanjiro(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CStory_Tanjiro(const CStory_Tanjiro& _rhs);
	virtual ~CStory_Tanjiro() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;

public:
	// Collision Jump		
	void					Collision_Jump(_vector vDir);
	// Collision Slide
	void					Collision_Slide(_vector vDir);

protected:
	// Set Up Constant
	virtual HRESULT SetUp_ConstantTable() override;
	// SetUp Components
	virtual HRESULT SetUp_Components() override;
	// SetUp AI Controller
	virtual HRESULT SetUp_Controller(const CONTROLLER_TYPE& _eType) override;
protected:
	// Datas
	virtual void		Fill_Datas() override;
	// Chnage State
	virtual void			Behavior_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Animation Controller State 
	virtual void			Animation_Control_State(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Jump
	virtual void			Jump_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;

private:
	// 현재 프레임에 들어온 트리거 리스트
	vector<_string>		m_TriggerList;
	_float				m_fSpeed = 50.f;
	_float3				m_vSlideDir = _float3(0.f, 0.f, 0.f);
	_bool				m_bIsJumpEnd = false;

public:
	static CStory_Tanjiro* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END