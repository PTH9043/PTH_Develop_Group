#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CAI_State abstract : public CBase
{
public:
	typedef struct tagAIDesc
	{
		_string strAnimationName = "";
		_uint	  iAnimationIndex = 0;
		_uint	  iStateID = 0;
	}AIDESC;
protected:
	explicit CAI_State(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CAI_State() = default;
public:
	const _char* Get_CurAnimation() { return m_strAnmationName.c_str(); }
	const _uint& Get_CurAnimation_Index() const { return m_iAnimationIndex; }
	const _uint& Get_State() const { return m_iStateID; }
public:
	virtual HRESULT NativeConstruct_Prototype(void* _pArg);
	// FSM = Controller
	// Owner = 주인 오브젝트
	// Target = 대상 오브젝트
	// ExtraData = 구조체 주소로 값을 던져서 State 내부에서 따로 함수를 만들지 않아도 사용 가능하게 만드는 용도
	// _ppOut 마찬가지고 따로 State 내부에서 함수를 만들지 않아도 값을 얻어올 수 있게 만듦
	virtual void Tick(const _double& _dTimeDelta, class CAI_FSMController* _pFSM, 
		class CGameObject* _pOwner, class CGameObject* _pTarget,
		void* _pExtraData = nullptr, void** _ppOut = nullptr);

	virtual void Late_Tick(const _double& _dTimeDelta, class CAI_FSMController* _pFSM, class CGameObject* _pOwner, 
		class CGameObject* _pTarget, void* _pExtraData = nullptr, void** _ppOut = nullptr);

public:
	// Change State
	virtual void Change_State(class CAI_FSMController* _pFSM, const _uint& _iID);
// private:
// 	virtual void Change_State(class CAI_FSMController* _pFSM);
protected:
	ID3D11Device*					 m_pDevice = nullptr;
	ID3D11DeviceContext*			 m_pContext = nullptr;
	_string							 m_strAnmationName = "";
	_uint							 m_iAnimationIndex = 0;
	_uint							 m_iStateID = 0;
public:
	virtual void Free() override;
};

END