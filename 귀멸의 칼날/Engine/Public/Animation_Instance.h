#pragma once
#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CChannel_Instance;
class CModel_Instance;

class ENGINE_DLL CAnimation_Instance final : public CBase
{
private:
	explicit CAnimation_Instance();
	virtual ~CAnimation_Instance() = default;

public:
	const _string& Get_Name() const { return m_strName; }
	void Set_Finished(const _bool& _bIsFinished) { this->m_bFinished = _bIsFinished; }
	const _bool& Is_Finished() const { return m_bFinished; }

	const _double& Get_TimeAcc() const { return m_dTimeAcc; }
	const _double& Get_Duration() const { return m_dDuration; }
	const _double& Get_TickPerSecond() const { return m_dTickPerSeconds; }
	// Return Animation Progress Ratio
	_double Get_Ratio() const { return m_dTimeAcc / m_dDuration; }

	_bool Get_AnimTimeSituation(const _float& _divineValue);
	_bool Is_BetweenTime(const _double& _dStartTime, const _double& _dEndTime);
	_bool Is_BetweenTime_FastTime(const _uint& _iIndex = 0);
	_bool Is_BetweenTime_Clip(const _char* _pName);
	_bool Is_OverTime(const _double& _dTime);
	_bool Is_OverTime_AnimClip(const _char* _pName, const _double& _dSupTime = 0.0);

	void Reset_Data();
public:
	vector<CChannel_Instance*> Get_Channels() { return m_Channels; }
	CChannel_Instance* Get_Channel(const _uint& _iIndex) { return m_Channels[_iIndex]; }

	void Set_FastValue(const _float& _fFastValue) { this->m_fFastValue = _fFastValue; }
	const _float& Get_FastValue() const { return m_fFastValue; }

	void Set_SuppleFastValue(const _float& _fFastValue) { this->m_fFastValue = _fFastValue; }

	const _float& Get_SupplmentValue() const { return m_fSupplementValue; }

	const _bool& Is_SupplementSituation() const { return m_bSupplementSituation; }
	void Set_SupplementSituation(const _bool& _bSituation) { m_bSupplementSituation = _bSituation; }

	const _bool& Is_StopAnimation() const { return m_bStopAnimation; }
	void Set_StopAnimation(const _bool& _bIsStopAnim) { this->m_bStopAnimation = _bIsStopAnim; }

#ifdef _DEBUG
	// Adjust TimeAcc
	void Set_TimeAcc(_double dTimeAcc) { m_dTimeAcc = dTimeAcc; }
#endif // _DEBUG

public:
	void Add_AnimFastSection(const ANIMFASTSECTION& _tAnimFastSection) { m_AnimFastSection.push_back(_tAnimFastSection); }
	vector<ANIMFASTSECTION>* Get_AnimFastSection() { return &m_AnimFastSection; }
	void ReMove_FastSection(const ANIMFASTSECTION& _tAnimFastSection);

	void Add_AnimClipSection(const ANIMCLIPSECTION& _tAnimClipSection) { m_AnimClipSection.push_back(_tAnimClipSection); }
	vector<ANIMCLIPSECTION>* Get_AnimClipSection() { return &m_AnimClipSection; }
	void Remove_AnimClipSection(const ANIMCLIPSECTION& _tAnimClipSection);
public:
	HRESULT NativeConstruct(CModel_Instance* _pModel, const ANIMDESC& _tAnimDesc);
	// Update Transform Matrices
	void Update_TransformMatrices(const _double& _dTimeDelta);
	// Update Trnasform Matrices
	void Update_TransformMatrices();
	// Supplement_Transform Matrix
	void Updatet_NextAnim_TransformMatrix(const _double& _dTimeDelta, CAnimation_Instance* _pNextAnim);
	// Clone Animation
	CAnimation_Instance* Clone_Animation(CModel_Instance* _pModel, const _uint& _iGroupID = 0);
public:
	// Fast Section
	void Save_AnimationFastSection(FILEGROUP* _pFileGroup);
	void Load_AnimationFastSection(FILEGROUP* _pFileGroup);
	// Animation Clip 
	void Save_AnimationClipSection(FILEGROUP* _pFileGroup);
	void Load_AnimationClipSection(FILEGROUP* _pFileGroup);
private:
	_string												m_strName;
	_uint													m_iNumChannels = 0;
	_double												m_dTickPerSeconds = 0.0;
	_double												m_dDuration = 0.0;
	vector<CChannel_Instance*>		m_Channels;
private:
	_double												m_dTimeAcc = 0.0;
	_bool													m_bFinished = false;
private:
	_float													m_fFastValue = 1.f;
	_float													m_fSuppleFastValue = 1.f;
	_float													m_fSupplementValue = 0.0f;
	_bool													m_bSupplementSituation = false;
	_bool													m_bStopAnimation = false;
private:
	vector< ANIMFASTSECTION>		m_AnimFastSection;
	vector < ANIMCLIPSECTION>		m_AnimClipSection;
public:
	void Save_Data(const HANDLE& _hFile, DWORD& _dwByte);
	// Animation
	static CAnimation_Instance* Create(CModel_Instance* _pModel, const ANIMDESC& _tAnimDesc);
	// override
	virtual void Free() override;
};

END