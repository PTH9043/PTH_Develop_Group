#pragma once
#include "UScene.h"
BEGIN(Engine)
class UGuard;
class UFire;
class UFont;
END

BEGIN(Client)

class CMainCamera;
class CMap;
class CWarriorPlayer;
class CMummy;
class CSarcophagus;
class CImageUI;
class CButtonUI;
class CLoadingUI;
class CHpBarUI;
class CFreeModeCamera;

class CMainScene final : public UScene{
public:
	CMainScene(CSHPTRREF<UDevice> _spDevice);
	NO_COPY(CMainScene)
	DESTRUCTOR(CMainScene)
private:
	void TurnLightsOnRange();
	void TurnRoomsOnRange();
	void TurnGuardsOnRange(const _double& _dTimeDelta);
	void UpdateMobsStatus();
public:
	virtual void Free() override;
	virtual HRESULT LoadSceneData() override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void LateTick(const _double& _dTimeDelta) override;
	void CollisionTick(const _double& _dTimeDelta);

	void DrawStartSceneUI(const _double& _dTimeDelta);
	void CreateStartSceneUI();
	void CreateGameSceneUI();
	void CreateKeyInfoUI();
	void CreateAbilityUI();
	void CreateAttackUI();
	void CreateInteractUI();
	void CreateDeactivateUI();
private:
	// Main
	SHPTR<CMainCamera>			m_spMainCamera;

	SHPTR<CMap>					m_spMap;
	SHPTR<CWarriorPlayer>		m_spWarriorPlayer;

	SHPTR<UFire>	m_stFireOne;
	SHPTR<UFire>	m_stFireTwo;

	_bool			m_bIsFoundPlayer_Minotaur=false;
	_bool			m_bisFoundPlayer_Harlequinn = false;
	_bool			m_bisFoundPlayer_Anubis = false;

	_bool			m_bIsDead_Minotaur;
	_bool			m_bisDead_Harlequinn;
	_bool			m_bisDead_Anubis;

	_int			m_iMinotaurMaxHP;
	_int			m_iMinotaurCurHP;
	_int			m_iHarlequinnMaxHP;
	_int			m_iHarlequinnCurHP;
	_int			m_iAnubisMaxHP;
	_int			m_iAnubisCurHP;

	_bool			m_bisMobsAllDead_Interior_Hallway_E;
	_bool			m_bisMobsAllDead_Interior_Room_D;
	_bool			m_bisMobsAllDead_Interior_Room_F;
	_bool			m_bisMobsAllDead_Interior_Room_G;

	_float r_AbilityCoolTime = 0;
	_float r_AbilityDurationTime = 0;
	_bool r_AbilityisAvailable = true;
private:
	//------------Á¶ÀÛ¹ý Controll Keys UI-----------
	SHPTR<CImageUI>			m_spBOOKPAGEUI;
	SHPTR<CImageUI>			m_spKEYBOARDCONTROLSUI;

	SHPTR<CImageUI>			m_spMOVEUI;
	SHPTR<CImageUI>			m_spMOVETextUI; //wasd - move

	SHPTR<CImageUI>			m_spRUNUI;
	SHPTR<CImageUI>			m_spRUNTextUI; //shift - run

	SHPTR<CImageUI>			m_spJUMPSTANDUPUI;
	SHPTR<CImageUI>			m_spJUMPSTANDUPTextUI;

	SHPTR<CImageUI>			m_spSHORTATTACKUI;
	SHPTR<CImageUI>			m_spSHORTATTACKTextUI;

	SHPTR<CImageUI>			m_spULTIMATEATTACKONESUI;
	SHPTR<CImageUI>			m_spULTIMATEATTACKONETextUI;

	SHPTR<CImageUI>			m_spULTIMATEATTACKTWOUI;
	SHPTR<CImageUI>			m_spULTIMATEATTACKTWOTextUI;

	SHPTR<CImageUI>			m_spDETACTABILITYUI;
	SHPTR<CImageUI>			m_spDETACTABILITYTextUI;

	SHPTR<CImageUI>			m_spROLLUI;
	SHPTR<CImageUI>			m_spROLLTextUI;


	SHPTR<CImageUI>			m_spCOMBOATTACKONEUI;
	SHPTR<CImageUI>			m_spCOMBOATTACKONETextUI;

	SHPTR<CImageUI>			m_spCOMBOATTACKTWOUI;
	SHPTR<CImageUI>			m_spCOMBOATTACKTWOTextUI;

	SHPTR<CImageUI>			m_spREVIVEUI;
	SHPTR<CImageUI>			m_spREVIVETextUI;

	//------------START SCENE UI-------------------
	SHPTR<CImageUI>			m_spBackgroundUI;
	SHPTR<CImageUI>			m_spMainTitleUI;
	SHPTR<CImageUI>			m_spMainTitleEffectUI;
	SHPTR<CImageUI>			m_spLineEffectUI;
	SHPTR<CImageUI>			m_spBackEffectUI;

	SHPTR<CImageUI>			m_spLoadingBackgroundUI;
	SHPTR<CLoadingUI>			m_spLoadingFillingUI;

	SHPTR<CImageUI>			m_spLoadingTextUI;
	SHPTR<CLoadingUI>			m_spLoadingDotsUI;
	SHPTR<CLoadingUI>			m_spPleaseWaitTextUI;

	SHPTR<CButtonUI>			m_spEnterButtonUI;
	_bool wasMouseEnterOverButton = false;
	SHPTR<CButtonUI>			m_spExitButtonUI;
	_bool wasMouseExitOverButton = false;
	_float m_fStartSceneLoadingTimer = 0;
	_bool m_bStartSceneForUI = false;
	_bool m_bStartGameForUI = false;
	_bool m_bStartGameDefault = true;
	//-------------------------------------------
	//------------GAME SCENE UI-------------------
	SHPTR<CHpBarUI>			m_spHpBarUI;
	SHPTR<CImageUI>			m_spBackPlayerFrameUI;
	SHPTR<CImageUI>			m_spBackDragonPlayerFrameUI;
	SHPTR<CImageUI>			m_spFrontPlayerFrameUI;
	SHPTR<CImageUI>			m_spPlayerNameUI;

	SHPTR<CImageUI>			m_spTABUI;
	SHPTR<CImageUI>			m_spTABTEXTUI;
	//--------------ATTACK UI------------------
	SHPTR<CImageUI>			m_spUltimateAttackOneFrameUI;
	SHPTR<CLoadingUI>			m_spUltimateAttackOneUI;
	SHPTR<CImageUI>			m_spUltimateAttackTwoFrameUI;
	SHPTR<CLoadingUI>			m_spUltimateAttackTwoUI;
	SHPTR<CImageUI>			m_spDetactAbilityIconFrameUI;
	SHPTR<CLoadingUI>			m_spDetactAbilityIconUI;
	SHPTR<CImageUI>			m_spDetactAbilityKeyIconUI;
	SHPTR<CImageUI>			m_spShortAttackIconFrameUI;
	SHPTR<CLoadingUI>			m_spShortAttackIconUI;
	SHPTR<CImageUI>			m_spShortAttackKeyIconUI;
	
	//==============Boss Frame UI-=================
	SHPTR<CImageUI>			m_spBossHpBarFrameUI;
	//MinoTaur
	SHPTR<CImageUI>			m_spMinotaurFrameUI;
	SHPTR<CHpBarUI>			m_spMinotaurHpBarUI;
	//Harlequinn
	SHPTR<CImageUI>			m_spHarlequinnFrameUI;
	SHPTR<CHpBarUI>			m_spHarlequinnHpBarUI;
	//Anubis
	SHPTR<CImageUI>			m_spAnubisFrameUI;
	SHPTR<CHpBarUI>			m_spAnubisHpBarUI;
	// -------------- Font --------------------
	SHPTR<UFont>			m_spPlayerHpFont;
	SHPTR<UFont>			m_spMinotaurHpFont;
	SHPTR<UFont>			m_spHarlequinnHpFont;
	SHPTR<UFont>			m_spAnubisHpFont;
	SHPTR<UFont>			m_spPlayerAbilityLeftTimeFont;
	

	//============Ability UI=======================
	SHPTR<CImageUI>			m_spAbilityFrameUI;
	SHPTR<CLoadingUI>			m_spRecUI;

	//============Interact UI=======================
	SHPTR<CImageUI>			m_spOpenChestTextUI;
	SHPTR<CImageUI>			m_spLiftCageTextUI;
	SHPTR<CImageUI>			m_spSaveCheckPointTextUI;
	SHPTR<CImageUI>			m_spCheckPointCompleteTextUI;
	_float CheckPointCompleteElapsedTime = 0;
	_float DeactivateElapsedTime = 0;
	SHPTR<CImageUI>			m_spFKeyOpenChestTextUI;
	SHPTR<CImageUI>			m_spFKeyLiftCageTextUI;
	SHPTR<CImageUI>			m_spFKeySaveCheckPointTextUI;

	SHPTR<CImageUI>			m_spLiftLoadGageBackgroundTextUI;
	SHPTR<CLoadingUI>			m_spLiftFillGageBackgroundTextUI;
	SHPTR<CImageUI>			m_spCheckPointLoadGageBackgroundTextUI;
	SHPTR<CLoadingUI>			m_spCheckPointFillGageBackgroundTextUI;

	SHPTR<CImageUI>			m_spCollideGuardTextUI;
	
	//================IF DIE============
	SHPTR<CLoadingUI>			m_spDieTextUI;
	SHPTR<CImageUI>			m_spDieKeyGUI;
	SHPTR<CImageUI>			m_spDieReviveTextUI;

	//===============Deactivate==============
	SHPTR<CImageUI>			m_spDeactivateMinotaurTextUI;
	SHPTR<CImageUI>			m_spDeActivatedMinotaurTextUI;
	SHPTR<CImageUI>			m_spDeActivateMinotaurBackgroundUI;
	SHPTR<CLoadingUI>		m_spDeActivateMinotaurGageUI;

	SHPTR<CImageUI>			m_spDeactivateHarelequinTextUI;
	SHPTR<CImageUI>			m_spDeActivatedHarelequinTextUI;
	SHPTR<CImageUI>			m_spDeActivateHarelequinBackgroundUI;
	SHPTR<CLoadingUI>		m_spDeActivateHarelequinGageUI;

	SHPTR<CImageUI>			m_spDeactivateAnubisTextUI;
	SHPTR<CImageUI>			m_spDeActivatedAnubisTextUI;
	SHPTR<CImageUI>			m_spDeActivateAnubisBackgroundUI;
	SHPTR<CLoadingUI>		m_spDeActivateAnubisGageUI;

	SHPTR<CImageUI>			m_spKeyFUIMinotaur;
	SHPTR<CImageUI>			m_spKeyFUIHarelequin;
	SHPTR<CImageUI>			m_spKeyFUIAnubis;

	SHPTR<CImageUI>			m_spGuardDeactivate_D;
	SHPTR<CImageUI>			m_spGuardDeactivate_E;
	SHPTR<CImageUI>			m_spGuardDeactivate_F;
	SHPTR<CImageUI>			m_spGuardDeactivate_G;
	_float m_fGuardTimer_D = 0;
	_float m_fGuardTimer_E = 0;
	_float m_fGuardTimer_F = 0;
	_float m_fGuardTimer_G = 0;
	_float m_fTextTimer_Final = 0;

	_bool m_fGuardSound_D = false;
	_bool m_fGuardSound_E = false;
	_bool m_fGuardSound_F = false;
	_bool m_fGuardSound_G= false;
	_bool m_fGuardSound_Final= false;

	SHPTR<CImageUI>			m_spEndingMent;

	//======================SOUND BOOL-=====================
	_bool EnterGameModeBGM = false;
	_bool EnterGameModeSound= false;
	_bool EnterAbilitySound= false;
	_bool ExitAbilitySound= false;
	_bool EnterMinoSound= false;
	_bool EnterQuinnSound= false;
	_bool EnterAnubisSound= false;
	_bool EnterBlindSound= false;
	_bool EndGameSound = false;

	_float EndingTimeElapsed = 0;

	///=================ENDING============================
	SHPTR<CImageUI>						m_spEndingText;
	SHPTR<CFreeModeCamera>	m_spFreeModeCamera;
};

END

