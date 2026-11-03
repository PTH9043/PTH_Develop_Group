#pragma once
#include "CObject.h"

class CTransform;
class CComponent;

typedef  unordered_map<_wstring, weak_ptr< CComponent>> COMPONENTS;

class CActor abstract  : public CObject
{
protected:
	explicit CActor() = default;
	explicit CActor(const CActor& _rhs) :CObject(_rhs) {}
	virtual ~CActor() = default;
public:
	virtual shared_ptr<CActor> Clone(const VOIDDATAS& _tDatas);
	virtual void Free() override;
protected:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct(VOIDDATAS _tDatas);

private:
	shared_ptr<class CComponent> CloneComp(const _wstring& _strPrototypes, const VOIDDATAS& _tDatas = {});
protected:
	virtual HRESULT CreateComponent();
	template<typename T>
	shared_ptr<T> CloneCompData(const _wstring& _strPrototypes, const VOIDDATAS& _tDatas = {}) {
		shared_ptr<CComponent> pComp = CloneComp(_strPrototypes, _tDatas);
		return static_pointer_cast<T>(pComp);
	}
public:
	HRESULT Tick_Awake(const _double& _dTimeDelta);
	virtual HRESULT Tick(const _double& _dTimeDelta);
	virtual HRESULT Late_Tick(const _double& _dTimeDelta);
	virtual HRESULT Render(const HDC& _hDC);

protected:
	// Add Component
	void Add_Component(const _wstring& _str, const shared_ptr<CComponent>& _pCompontens);
public:
	void Set_Active(const _bool& _bActive) { this->m_bActive = _bActive; }
	const _bool& Is_Active() const { return m_bActive; }

	const shared_ptr<CTransform>& Get_Transform() const { return m_pTransform; }
private:
	_bool											m_bActive = false;
	_float											m_fCamToDistance = 0.f;
	shared_ptr<CTransform>		m_pTransform = nullptr;
	// Componets
	COMPONENTS							m_Components{};
};

