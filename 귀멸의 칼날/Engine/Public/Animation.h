#pragma once
#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CChannel;

class  ENGINE_DLL CAnimation final : public CBase
{
private:
	explicit CAnimation();
	virtual ~CAnimation() = default;
public:
	const _string& Get_Name() const { return m_strName; }
	const _string& Get_RootBoneName() const { return m_strRootBoneName; }
	void Set_RootBoneName(const _string& _strRootBoneName) { this->m_strRootBoneName = _strRootBoneName; }
	void Set_Finished(const _bool& _bIsFinished) { this->m_bFinished = _bIsFinished; }
	const _bool& Is_Finished() const { return m_bFinished; }
	const _bool& Is_RootBone() const { return m_bRootBone; }
	void Set_RootBone(const _bool& _bIsRootBone) { this->m_bRootBone = _bIsRootBone; }
	// 매 프레임마다 RootBone의 이동값을 받아옵니다.
	void Set_RootBoneMatrix(_fmatrix _RootBoneMatrix);
	// 현재 프레임에서 RootBone의 이동값을 받아옵니다.
	_matrix Get_RootBoneMatrix() { return XMLoadFloat4x4(&m_RootBoneMatrix); }	
	// 현재 프레임에서 Root의 이동값을 설정합니다. ( 애니메이션 마다의 Root옵션 추가하기 )
	void Set_RootTransform(_fvector _vPos) { XMStoreFloat3(&m_RootTransform, _vPos); }
	// RootTransform값을 얻어온다.
	_vector Get_RootTransform() { return XMLoadFloat3(&m_RootTransform); }

	const _double& Get_TimeAcc() const { return m_dTimeAcc; }
	const _double& Get_Duration() const { return m_dDuration; }
	const _double& Get_TickPerSecond() const { return m_dTickPerSeconds; }
	// Return Animation Progress Ratio
	_double Get_Ratio() const { return m_dTimeAcc / m_dDuration; }
	// Return Animation PlayTime
	_double Get_PlayTime() const { return m_dDuration / m_dTickPerSeconds; }

	_bool Get_AnimTimeSituation(const _float& _divineValue);
	_bool Is_BetweenTime(const _double& _dStartTime, const _double& _dEndTime);
	_bool Is_BetweenTime_FastTime(const _uint& _iIndex = 0);
	_bool Is_BetweenTime_Clip(const _char* _pName);
	_bool Is_OverTime(const _double& _dTime);
	_bool Is_OverTime_AnimClip(const _char* _pName, const _double& _dSupTime = 0.0);

	void Reset_Data();
	// 클립에 속한 애니메이션들의 ResetData
	void Reset_Data_ClipAnimation();
public:
	vector<class CChannel*> Get_Channels() { return m_Channels; }
	class CChannel* Get_Channel(const _uint& _iIndex) { return m_Channels[_iIndex]; }

	void Set_FastValue(const _float& _fFastValue) { this->m_fFastValue = _fFastValue; }
	const _float& Get_FastValue() const { return m_fFastValue; }

	void Set_SuppleFastValue(const _float& _fFastValue) { this->m_fFastValue = _fFastValue; }

	const _float& Get_SupplmentValue() const { return m_fSupplementValue; }

	const _bool& Is_SupplementSituation() const { return m_bSupplementSituation; }
	void Set_SupplementSituation(const _bool& _bSituation) { m_bSupplementSituation = _bSituation; }

	 const _bool& Is_StopAnimation() const { return m_bStopAnimation; }
	 void Set_StopAnimation(const _bool& _bIsStopAnim) { this->m_bStopAnimation = _bIsStopAnim; }

	 // Adjust TimeAcc
	 void Set_TimeAcc(_double dTimeAcc) { m_dTimeAcc = dTimeAcc; }

public:
	void Add_AnimFastSection(const ANIMFASTSECTION& _tAnimFastSection) { m_AnimFastSection.push_back(_tAnimFastSection); }
	vector<ANIMFASTSECTION>* Get_AnimFastSection() { return &m_AnimFastSection; }
	void ReMove_FastSection(const ANIMFASTSECTION& _tAnimFastSection);

	void Add_AnimClipSection(const ANIMCLIPSECTION& _tAnimClipSection) { m_AnimClipSection.push_back(_tAnimClipSection); }
	vector<ANIMCLIPSECTION>* Get_AnimClipSection() { return &m_AnimClipSection; }
	void Remove_AnimClipSection(const ANIMCLIPSECTION& _tAnimClipSection);
public:
	HRESULT NativeConstruct(class CModel* _pModel, const ANIMDESC& _tAnimDesc);
	// Update Transform Matrices
	void Update_TransformMatrices(const _double& _dTimeDelta);
	// Update Transform Matrices from Animator
	void Update_TransformMatrices(const _double& _dTimeDelta, const _bool& bIsLoop);
	// Update Trnasform Matrices
	void Update_TransformMatrices();
	// Update Sub Transform Matrices
	void Update_SubTransformMatrices(const _double& _dTimeDelta);
	// Supplement_Transform Matrix
	void Updatet_NextAnim_TransformMatrix(const _double& _dTimeDelta, CAnimation* _pNextAnim, const _bool& _bIsSupplement = true);
	// 다음 애니메이션 클립의 처음 부분이 아닌곳과 보간한다. ( 애니메이션 클립의 데이터들이 인자로 들어옴. ) 
	void Update_NextAnimClip_TransformMatrix(const _double& _dTimeDelta, const _double& _dNextAnimTime, 
		const _double& _dLerpSpeed, CAnimation* _pNextAnim, const _bool& _bIsSupplement = true);
	// Clone Animation
	CAnimation* Clone_Animation(class CModel* _pModel);
public:
	// Fast Section
	void Save_AnimationFastSection(FILEGROUP* _pFileGroup);
	void Load_AnimationFastSection(FILEGROUP* _pFileGroup);
	// Animation Clip 
	void Save_AnimationClipSection(FILEGROUP* _pFileGroup);
	void Load_AnimationClipSection(FILEGROUP* _pFileGroup);
private:
	_string						m_strName;
	// RootBone의 이름
	_string						m_strRootBoneName = "Root";
	_uint						m_iNumChannels = 0;
	_double						m_dTickPerSeconds = 0.0;
	_double						m_dDuration = 0.0;
	vector<class CChannel*>		m_Channels;
private:
	_double						m_dTimeAcc = 0.0;
	_bool						m_bFinished = false;
	_bool						m_bRootBone = false;
private:
	_float						m_fFastValue = 1.f;
	_float						m_fSuppleFastValue = 1.f;
	_float						m_fSupplementValue = 0.0f;
	_bool						m_bSupplementSituation = false;
	_bool						m_bStopAnimation = false;
private:
	// Root본 애니메이션으로 구한 행렬 ( 애니메이션에서 등록되어있는 이동값을 받아옴. )
	_float4x4					m_RootBoneMatrix;
	// 현재 프레임에서 사용할 Root본의 이동값
	_float3						m_RootTransform;
private:
	vector<ANIMFASTSECTION>	m_AnimFastSection;
	vector<ANIMCLIPSECTION>	m_AnimClipSection;
public:
	void Save_Data(const HANDLE& _hFile, DWORD& _dwByte);
	// Animation
	static CAnimation* Create(class CModel* _pModel, const ANIMDESC& _tAnimDesc);
	// override
	virtual void Free() override;
};

END