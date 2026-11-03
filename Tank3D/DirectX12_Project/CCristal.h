#pragma once
#include "CCharacter.h"

class CCristalBuffer;
class CCubeBuffer;
class CGameScene;

class CCristal : public CCharacter {
protected:
	explicit CCristal() : CCharacter(CHAR_CRISTAL) {};
	explicit CCristal(const CCristal& _rhs) :CCharacter(_rhs) {}
	virtual ~CCristal() = default;

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
	// Damaged
	virtual void Damaged(const shared_ptr<CCharacter>& _pChar) override;
protected:
	void DecreaseHealth() { --m_iHealth; }
	void IncreaseHealth() { if (m_iHealth < 100) ++m_iHealth; }
public:
	const _int Get_Health() const { return m_iHealth; }
public:
	// Set 
	void SetGameScene(const shared_ptr<CGameScene>& _pGameScene) { this->m_pGameScene = _pGameScene; }
private:
	shared_ptr<CCubeBuffer> m_pCristalBuffer = nullptr;
	// 
	weak_ptr<CGameScene>	m_pGameScene{};
	// Health
	_int											m_iHealth = 30;
};

