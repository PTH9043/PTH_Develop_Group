#pragma once
#include "CSword.h"

class CBossSword : public CSword
{
public:
	explicit CBossSword();
	explicit CBossSword(CObj* pOwner, const TCHAR* szImgPath, const wstring& strName, const ESWORDID& eSwordID);
	virtual ~CBossSword();
protected:
	float m_fSpinDistance;
	float m_fSpeed;
	VECTOR2 m_v2SaveSpawnPos;
	TIMER		m_tTimer;
public:
	// CEquip을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC _hdc, const VECTOR2 & v2ScrollPos) override;
	virtual void Collision_Actor(CObj *& pObj) override;
	virtual void Release() override;

	void Reset_Timer(){ m_tTimer.dwTime = GetTickCount(); 	m_tTimer.iPassTime = (rand() % 20) * 100; }
protected:
	virtual void Update_Rect() override;
	// Rotate 
	virtual void Rotate() override;
	// Spin
	void Spin();
	// Under
	virtual void Fall() override;
public:
	const float& Get_SpinAngle() const { return m_fSpinAngle; }
	const float& Get_SpinDistance() const { return m_fSpinDistance; }
	const float& Get_Speed() const { return m_fSpeed; }
	

	void Set_SpinAngle(const float& fSpinAngle) { this->m_fSpinAngle = fSpinAngle; }
	void Set_SpinDistance(const float& fSpinDistance) { this->m_fSpinDistance = m_fSpinDistance; }
	void Set_Speed(const float& fSpeed) { this->m_fSpeed = fSpeed; }
	void Set_SpawnPos(const VECTOR2& v2SpawnPos) { this->m_v2SaveSpawnPos = v2SpawnPos; }
};

