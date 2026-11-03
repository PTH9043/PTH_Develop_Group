#pragma once
#include "CPawn.h"

class COBBCollider;
class CFloorBuffer;
class CAABBCollider;

class CFloor : public CPawn {
public:
	typedef struct tagFloorDesc {

		tagFloorDesc() = default;

		_float3 vColor{};
		_float3 vBodyScale{};
		_float fWidth{ 0.f }, fDepth{ 0.f };
		_int iSubjects{ 0 };
	}FLOORDESC;
protected:
	explicit CFloor() = default;
	explicit CFloor(const CFloor& _rhs) :CPawn(_rhs) {}
	virtual ~CFloor() = default;

public:
	virtual shared_ptr<CActor> Clone(const VOIDDATAS& _tDatas) override;
	virtual void Free() override;

public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct(VOIDDATAS _tDatas) override;

public:
	virtual HRESULT CreateComponent() override;

public:
	virtual HRESULT Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render(const HDC& _hDC) override;

public:
	const shared_ptr<CAABBCollider>& Get_Collider() const { return m_pCollider; }
	const shared_ptr<CAABBCollider>& Get_RegionCollider() const { return m_pRegionCollider; }
private:
	shared_ptr< CFloorBuffer>	m_pFloorBuffer = nullptr;
	shared_ptr<CAABBCollider>	m_pCollider{ nullptr };
	shared_ptr<CAABBCollider>	m_pRegionCollider{ nullptr };

	_float											m_fWidth{ 0.f };
	_float											m_fDepth = 0.f;
	_int												m_iSubject = 0;
};

