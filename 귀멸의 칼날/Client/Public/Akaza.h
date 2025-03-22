#pragma once
#include "Player.h"

BEGIN(Client)

class CAkaza final : public CPlayer
{
protected:
	explicit CAkaza(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CAkaza(const CAkaza& _rhs);
	virtual ~CAkaza() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	// SetUp AI Controller
	virtual HRESULT SetUp_Controller(const CONTROLLER_TYPE& _eType);
	// Set Up Constant
	virtual HRESULT SetUp_ConstantTable() override;
	// SetUp Components
	virtual HRESULT SetUp_Components() override;

protected:
	// Datas
	virtual void		Fill_Datas() override;
	// Chnage State
	virtual void			Behavior_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Animation Controller State 
	void					Animation_Control_State(const _double& _dTimeDelta, VOIDDATAS _Datas);

protected:
	// Skill 1
	virtual void	Skill_1(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Skill 1 Air
	virtual void	Skill_1_Air(const _double& _dTimeDelta, VOIDDATAS _Datas);
	// Skill 2 
	virtual void	Skill_2(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Skill 3
	virtual void	Skill_3(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Move Normal
	virtual void	Move_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Attack
	virtual void	Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Attack Upper
	virtual void	Attack_Upper_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Attack Down
	virtual void	Attack_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Attack Awake
	virtual void	Attack_Awake_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Ultimae
	virtual void	Ultimate_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Charge Attack
	virtual void	Charge_Attack_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Step Back
	virtual void	Step_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Dash Ground
	virtual void	Dash_Ground_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Dash Air
	virtual void	Dash_Air_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Jump
	virtual void	Jump_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// 낙법
	virtual void	Passive_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// 구르기
	virtual void	Roll_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// 일반적인 Hit의 경우에 사용
	virtual void	Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// 공중에 떠서 Hit 됐을 경우에 사용
	virtual void	Air_Hit_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Upper맞아서 AIR_DOWN 상태가 됐거나 하는 경우에 사용
	virtual void	Air_Down_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Strike ( 공중에서 STRIKE 공격에 맞은 경우에 사용 )
	virtual void	Strike_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;
	// Bound 됐을때의 상태처리
	virtual void	Bound_State(const _double& _dTimeDelta, VOIDDATAS _Datas) override;

protected:
	virtual HRESULT Initialize_Sound() override;

public:
	// 성공한 공격의 종류를 받아서 해당 공격이 적중시 이뤄져야하는 행동등을 설정합니다. ( 해당 함수를 한번만 호출함. )
	virtual void	Check_Attack_Type(_string _strHitType) override;
protected:
	// Upper 공격에 성공했을 때 호출된다.
	virtual void	Success_Attack_Upper() override;
	// Down 공격에 성공했을 때 호출된다.
	virtual void	Success_Attack_Down() override;
	// 스킬 1 공격에 성공했을 때 호출된다.
	virtual void	Success_SKILL_1() override;
	// 스킬 2 공격에 성공했을 때 호출된다.
	virtual void	Success_SKILL_2() override;
	// 스킬 3 공격에 성공했을 때 호출된다.
	virtual void	Success_SKILL_3() override;
	// 잡기 공격에 성공했을 때 호출된다.
	virtual void	Success_Throw() override;
	// 개방 추가 공격에 성공했을 때 호출된다. ( 컷씬 실행 )
	virtual void	Success_Attack_Awake() override;
	// 오의 시동기를 맞췄을 때 호출된다. ( 컷씬 실행 )
	virtual void	Success_Ultimate() override;

public:
	static CAkaza* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END