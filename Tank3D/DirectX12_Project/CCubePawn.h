#pragma once
#include "CPawn.h"

class CCubeBuffer;
class COBBCollider;

class CCubePawn abstract : public CPawn {
public:
	typedef struct tagCubePawnDesc {

		tagCubePawnDesc() = default;

		_float3 vColor{};
		_float3 vBodyScale{};
	}CUBEPAWNDESC;
protected:
	explicit CCubePawn() = default;
	explicit CCubePawn(const CCubePawn& _rhs) :CPawn(_rhs) {}
	virtual ~CCubePawn() = default;

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
	const shared_ptr<CCubeBuffer>& Get_CubeBuffer() const { return m_pCubeBuffer; }
private:
	// CubeBuffer
	shared_ptr<CCubeBuffer> m_pCubeBuffer = nullptr;
};

