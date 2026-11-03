#pragma once
#include "CActor.h"

class CCubeBuffer;
class CRenderer;

class CCube : public CActor{
public:
	typedef struct tagCubeDesc {
		_float3 vColor;
	}CUBEDESC;
protected:
	explicit CCube() = default;
	explicit CCube(const CCube& _rhs) :CActor(_rhs) {}
	virtual ~CCube() = default;
public:
	virtual shared_ptr<CActor> Clone(const VOIDDATAS& _tDatas)override;
	virtual void Free() override;

public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct(VOIDDATAS _tDatas)override;

protected:
	virtual HRESULT CreateComponent() override;
public:
	virtual HRESULT Tick(const _double& _dTimeDelta)override;
	virtual HRESULT Late_Tick(const _double& _dTimeDelta)override;
	virtual HRESULT Render(const HDC& _hDC) override;
protected:
	const shared_ptr<CCubeBuffer>& Get_CubeBuffer() const { return m_pCubeBuffer; }
	const shared_ptr<CRenderer>& Get_Renderer() const { return m_pRenderer; }
private:

	shared_ptr<CCubeBuffer>		m_pCubeBuffer{ nullptr };
	shared_ptr<CRenderer>			m_pRenderer{ nullptr };
};

