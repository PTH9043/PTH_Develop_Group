#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CCollider;
END

BEGIN(Client)
class CCharacter;

class CBattle_Manager : public CBase
{
	DECLARE_SINGLETON(CBattle_Manager);
public:
	enum CHARACTER_TYPE
	{
		CHAR_PLAYER, CHAR_AI, CHAR_END
	};

	enum BATTLEMAP_TYPE
	{
		MAP_UROKODAKI, MAP_HUJIKASANE, MAP_TAMAYO, MAP_ASAKUSA
	};

	typedef struct tagBattleInfo
	{
		// 배틀 맵
		BATTLEMAP_TYPE		eBattleMap = MAP_UROKODAKI;
		// 전투 초기화
		CUSTIMER			tBattleTimer = CUSTIMER(99.f);
		CUSTIMER			tStartTimer = CUSTIMER(0.f);
		_bool				bBattleInit = false;
		_bool				bBattleStart = false;
		// 배틀 종료
		_bool				bBattleEnd = false;
		// 배틀 라운드
		_uint				iRound = 0;
		// 사비토 AI와 싸우고 싶으면 true로 바꾸면 알아서 적용됨
		_bool				bSabitoAI = false;
		// 슬로우
		_float				fUpdateRatio = 1.f;
	}BATTLEINFO;

protected:
	explicit CBattle_Manager();
	virtual ~CBattle_Manager() = default;

public:
	CCharacter*				Get_BattleCharacter(const CHARACTER_TYPE& _eCharType);
	vector<CCharacter*>		Get_OniArr() { return m_pOniArr; }
	const _uint&			Get_CurrentOniLayer() const { return m_iCurrentOniLayer; }
	const _bool&			IsStoryBattle() const { return m_bIsStoryBattle; }
	const BATTLEINFO&		Get_BattleInfo() const { return m_tBattleInfo; }
	const _bool& Get_BattleStart() const { return m_bInit; }
	
	_bool Is_MotionBlurOn();
	// 배틀 끝났는지 체크
	void	Set_BattleEnd(_bool bBattleEnd) { m_tBattleInfo.bBattleEnd = bBattleEnd; }
	// 배틀 모드의 맵을 지정해준다. 
	void	Set_BattleMapType(BATTLEMAP_TYPE eMapType) { m_tBattleInfo.eBattleMap = eMapType; }
	// 스토리 모드에서 배틀 상태를 설정한다.
	void	Set_IsStoryBattle(_bool bIsStoryBattle) { m_bIsStoryBattle = bIsStoryBattle; }
	// 딜레이를 설정한다.
	void	Set_DelayTime(_float _fDelayTime) { m_tDelayTimer.Reset_Timer(); m_tDelayTimer.fStandardTime = _fDelayTime; }

public:
	HRESULT NativeConstruct_StartCutScene();
	void Start_Battle(CCharacter* _pCharacter, const CHARACTER_TYPE& _eCharType);
	// Update
	void Update(const _double& _dTimeDelta);

private:
	// 현재 오니 Layer에 존재하는 오니들을 가져온다.
	void SetUp_OniLayer();
	// 대전 충돌시 밀어낸다
	void Push();
	// 전투 타이머
	void InitBattle(const _double& _dTimeDelta);
	void Change_UpdateRatio(_float fUpdateRatio);
	// 딜레이 설정
	void Check_BattleDelayTimer(const _double& _dTimeDelta);
	void Check_StoryDelayTimer(const _double& _dTimeDelta);

private:
	// Player
	CCharacter* m_pCharacterArr[CHAR_END] = { nullptr };

	// Oni
	vector<CCharacter*> m_pOniArr;
	_uint				m_iCurrentOniLayer = 1;
	_bool				m_bIsStoryBattle = false;
	// 공용
	_bool				m_bSuccessDash = false;
	// 특정 공격 이벤트가 들어왔을때 멈추는등의 행동을 한다
	CUSTIMER			m_tDelayTimer;
	
private:
	BATTLEINFO			m_tBattleInfo;

	_bool				m_bFadeOut = false;
	_bool				m_bInit = false;

protected:
	virtual void Free() override;
};

END