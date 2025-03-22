#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)
class CCharacter;
class CCutSceneModul;

class CCutScene_Manager final : public CBase
{
	DECLARE_SINGLETON(CCutScene_Manager);
protected:
	explicit CCutScene_Manager();
	virtual ~CCutScene_Manager() = default;
public:
	CCutSceneModul* Get_SelectCutSceneModel() { return m_pSelectCutSceneModul; }
	// Game Init
	const _bool& Is_GameInit() const { return m_bInit; }
	const _bool Is_PlayCutScene();
	void   Set_JudgeMotionBlur(const _bool& _bIsMotionBlur) { this->m_bJudgeMotionBlur = _bIsMotionBlur; }
public:
	HRESULT NativeConstruct(ID3D11Device* _pDevice , ID3D11DeviceContext* _pContext);
	void Tick(const _double& _dTimeDelta);
	void Late_Tick(const _double& _dTimeDelta);
	void Render();

	void Set_CutSceneModul(CCutSceneModul* _pModul); 

	void Texture_Load(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);

	CTexture* Get_TextureToMap(const _wstring& _strName);

	void Active_CutScene(const _string& _strInput);
	// Update 
	void Update_Battle(CCharacter* _p1, CCharacter* _p2);
protected:
	// SetUp Cut Scnee 
	HRESULT Add_CutScene(FILEDATA* _pFileData);
private:
	CCutSceneModul*									m_pSelectCutSceneModul = nullptr;
	// Texture Map
	map<_wstring, CTexture*>					m_TextureMap;
	// Cut Scene Modul 
	map<_string, CCutSceneModul*>		m_CutSceneModulMap;

	ID3D11Device*										m_pDevice = nullptr;
	ID3D11DeviceContext*							m_pContext = nullptr;

	_bool															m_bInit = false;
	_bool															m_bJudgeMotionBlur = false;
	_bool															m_bEndCutScene = false;
	CUSTIMER													m_tCutTimer = CUSTIMER();

	_string														m_strCurrentTrigger = "";
public:
	virtual void Free() override;
};

END