#pragma once
#include "CObject.h"

class CActor;

class CComponent abstract : public CObject
{
public:
	typedef struct tagCompDesc
	{
		tagCompDesc() = default;
		tagCompDesc(const shared_ptr<CActor>& _pActor) : pActor(_pActor){}

		weak_ptr<CActor> pActor;
	}COMPDESC;
protected:
	explicit CComponent() = default;
	explicit CComponent(const CComponent& _rhs) :CObject(_rhs) {}
	virtual ~CComponent() = default;
public:
	virtual shared_ptr<CComponent> Clone(const VOIDDATAS& _tDatas) =0;
	virtual void Free() override;
protected:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct(VOIDDATAS _tDatas);
public:
	virtual void Tick(const _double& _dTimeDelta){}
public:
	const weak_ptr<CActor>& Get_Actor() const { return m_pActor; }
private:
	// Actor
	weak_ptr<CActor>		m_pActor;
};

