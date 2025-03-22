
#pragma once
#include "Pawn.h"

BEGIN(Engine)
class CAnimator;
END

BEGIN(Client)

class CAnimal : public CPawn
{
public:
	enum AnimalType {ANIMAL_DEER ,ANIMAL_BUTTERFLY, ANIMAL_CAT, ANIMAL_BIRD, ANIMAL_DOG,
		ANIMAL_MOUSE, ANIMAL_CHICKEN, ANIMAL_BUTTERFLY2, ANIMAL_BUTTERFLY3,  ANIMAL_END};
	enum AnimalState {EAT,ALERT,RUN,WALK,STAND, STATE_END};
protected:
	explicit CAnimal(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CAnimal(const CAnimal& _rhs);
	virtual ~CAnimal() = default;
	
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Extern_ImGuiClass() override;


public:
	void Change_Model(_uint Type);
	void Setting_Positon(_uint Type);
	void Setting_Trigger(const _double& _dTimeDelta);
	void Change_State();
	
private:
	_uint m_iType = ANIMAL_END;

	_bool Run = false;

	_double Chickentime = 0.0;
	
	_float m_RunTime = 0.f;
	_float m_Angle = 0.f;
	_float3 m_Length = { 0.f,0.f,0.f };

	_uint PrevState = STATE_END;
	_uint CurrentState = STATE_END;

	_bool SoundPlay = false;
	
protected:
	// Set Up Constant
	virtual HRESULT SetUp_ConstantTable() override;
	// SetUp Components
	virtual HRESULT SetUp_Components() override;

private:
	CModel*			m_pModel = nullptr;
	CModel*			m_pAllAnimModel = nullptr;
	CAnimator*		m_pAnimator = nullptr;

public:
	vector <CModel*> m_ModelList;
	vector <CAnimator*> m_AnimatorList;
	
public:
	static CAnimal* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
