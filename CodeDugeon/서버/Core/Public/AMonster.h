#pragma once
#include "APawn.h"

BEGIN(Core)
class ASession;
class ACell;
class AJobTimer;

using ANIMFUNC = std::function<void*>(const _double&);

class CORE_DLL AMonster abstract : public APawn {
	using PATHCONTAINER = VECTOR<Vector3>;
public:
	AMonster(OBJCON_CONSTRUCTOR, SESSIONID _ID, SHPTR<AJobTimer> _spMonsterJobTimer);
	DESTRUCTOR(AMonster)
public:
	virtual _bool Start(const VOIDDATAS& _ReceiveDatas = {}) PURE;
	void FindPlayer(SHPTR<ASession> _spSession);
	virtual void Tick(const _double& _dTimeDelta) PURE;
	virtual void State(SHPTR<ASession> _spSession, _int _MonsterState = 0) PURE;
	virtual void ProcessPacket(_int _type, void* _pData) PURE;
	void InsertMobJobTimer(SESSIONID _PlayerID);
	_bool IsCanMove(const Vector3 _vPos);
public: /* get set */
	const MONSTERSTATE GetMonsterState() const { return m_MobState; }
	const Vector3 GetNextPos() const { return m_vNextPos; }
	const _int GetMonsterType() const { return m_iMonsterType; }
	const _float GetDistanceToPlayer() const { return m_fDistanceToPlayer; }
	_bool IsFoundPlayerFirstTime() const { return m_isFoundPlayerFistTime; }
	const SESSIONID GetCurrentTargetPlayerID() const { return m_CurrentTargetPlayerID; }
	const _bool IsCurrentAtkPlayer() const {return m_isCurrentAtkPlayer; }
	const _bool IsCurrentFindPlayer() const { return m_isCurrentFindPlayer; }
	const _bool IsCurrentJustMove() const { return m_isCurrentJustMove; }
	const SESSIONID& GetOwnerMonsterSessionID() const { return m_OwnerMonsterSessionID; }

	void SetNextPos(const Vector3 _vNextPos) { this->m_vNextPos = _vNextPos; }
	void SetMonsterState(const MONSTERSTATE _MonsterState);
	void SetFoundPlayerFirstTime(_bool _isFindFirstTime);
	void SetOwnerMonsterSessionID(const SESSIONID& _OwnerMonsterSessionID) { this->m_OwnerMonsterSessionID = _OwnerMonsterSessionID; }
protected:
	void ComputeNextDir(const _double _dTimeDelta);
	void UpdateTargetPos(SHPTR<ATransform> _ArriveTr);
	void UpdateFindRange(const _float _fActiveRange, const _float _fDeactiveRange);
	void UpdateSelfStateToPlayerDistance(_bool _isCurAtkPlayer, _bool _isCurFindPlayer, _bool _isCurJustMove);
	virtual void ChangeCurrentFindPlayer(SESSIONID _CurPlayerSessionID, SESSIONID _ChangePlayerSessionID);
protected: /* get set*/
	SHPTR<AJobTimer> GetMonsterJobTimer() { return m_wpMonsterJobTimer.lock(); }
	const _float GetActiveRange() const { return m_fActiveRange; }
	const _float GetDeactiveRange() const { return m_fDeactiveRange; }
	const _float GetAttackRange() const { return m_fAttackRange; }
	const PATHCONTAINER& GetPathList() const { return m_PathList; }

	void SetMonsterJobTimer(SHPTR<AJobTimer> _spJobTimer) { m_wpMonsterJobTimer = _spJobTimer; }
	void SetMonsterType(_int _iMonsterType) { this->m_iMonsterType = _iMonsterType; }
	void SetActiveRange(const _float _fActiveRange) { this->m_fActiveRange = _fActiveRange ; }
	void SetDeactiveRange(const _float _fDeactiveRange) { this->m_fDeactiveRange = _fDeactiveRange ; }
	void SetAttackRange(const _float _fAttackRange) { this->m_fAttackRange = _fAttackRange ; }
	void SetTargetPlayerID(const SESSIONID _SessionID){ m_CurrentTargetPlayerID = _SessionID; }
	void SetCurrentAtkPlayer(const _bool _isCurrentAtkPlayer) { this->m_isCurrentAtkPlayer = _isCurrentAtkPlayer; }
	void SetCurrentFindPlayer(const _bool _isCurrentFindPlayer) {	m_isCurrentFindPlayer = _isCurrentFindPlayer; }
	void SetCurrentJustMove(const _bool _isCurrentJustMove) { m_isCurrentJustMove = _isCurrentJustMove; }
	void SetMoveSpeed(const _float _fMoveSpeed) { this->m_fMoveSpeed = _fMoveSpeed; }
private:
	void ChangePlayerTargetID(SESSIONID _SessionID);
private:
	virtual void Free() override;
private:
	// 가야할 다음 장소
	Vector3											m_vNextPos;
	CUSTIMER										m_FindNextPosTimer;

	ATOMIC<MONSTERSTATE>		m_MobState;
	_int													m_RoomIndex;
	_int													m_iMonsterType;
	ATOMIC<SESSIONID>					m_CurrentTargetPlayerID;

	PATHCONTAINER							m_PathList;

	_string											m_strAnimTriggerName;
	_string											m_strCurAnimName;

	WKPTR<AJobTimer>					m_wpMonsterJobTimer;

	std::atomic_bool							m_isFoundPlayerFistTime;
	std::atomic_bool							m_isCurrentAtkPlayer;
	std::atomic_bool							m_isCurrentFindPlayer;
	std::atomic_bool							m_isCurrentJustMove;

	_float												m_fActiveRange;
	_float												m_fDeactiveRange;
	_float												m_fAttackRange;
	_float												m_fMoveSpeed;
	ATOMIC<_float>							m_fDistanceToPlayer;
	std::atomic_bool							m_isPathFinding;

	PathFindingState						m_PathFindState;
	ATOMIC<_bool>							m_isRecvPathFindState;
	AFastSpinLock								m_TargetPosLock;
	ATOMIC<_int>								m_iNextPathIndex;
	Vector3											m_vTargetPos;
	ATOMIC<_bool>							m_isCurrentPlayerTargetChangeSituation;
	SESSIONID									m_OwnerMonsterSessionID;
};


END