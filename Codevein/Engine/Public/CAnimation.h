#pragma once
#include "Engine_Defines.h"
#include "CBase.h"

BEGIN(Engine)

class  ENGINE_DLL CAnimation final : public CBase
{
public:
	enum ANIM_POS
	{
		POS_DEST, POS_SOUR, POS_END
	};

	enum ANIM_ROT
	{
		ROT_DEST, ROT_SOUR, ROT_END
	};

	enum ANIM_SUPINDEX
	{
		SUPINDEX_INIT, SUPINDEX_LAST, SUPINDEX_END
	};

	enum ANIM_SUPCONTROL
	{
		SUPCONTROL_SOUR, SUPCONTROL_DEST, SUPCONTROL_END
	};

	typedef struct tagAnimFastSection
	{
		_float fStartSpot = -1.f;
		_float fEndSpot = -1.f;
		_float fFastValue = 1.f;

		_double Convert(const _double& _fValue, const _double& _dTimeValue)
		{
			_double fValue = _dTimeValue;

			if (fStartSpot <= _fValue && _fValue <= fEndSpot)
				fValue *= fFastValue;

			return fValue;
		}
		_bool Pass(const _double& _dTimeValue)
		{
			if (fStartSpot <= _dTimeValue && _dTimeValue <= fEndSpot)
				return true;

			return false;
		}

	}ANIMFASTSECTION;

	typedef struct tagAnimClipSection
	{
		_float fChange = 0.f;
		_float fEndValue = 0.f;
		_string strName;
		_bool Pass(const _char* _strName, const _double& _dTimeValue, const _double& _dSupTime = 0.f)
		{
			if (!strcmp(_strName, strName.c_str()))
			{
				if (fEndValue - _dSupTime  <= _dTimeValue)
					return true;
			}
			return false;
		}
		_bool Is_Between(const char* _strName, const _double& _dTimeValue)
		{
			if (!strcmp(_strName, strName.c_str()))
			{
				if (fChange <= _dTimeValue && _dTimeValue <= fEndValue)
					return true;
			}
			return false;
		}

	}ANIMCLIPSECTION;
private:
	explicit CAnimation();
	virtual ~CAnimation() = default;
public:
	void Save_Data(const HANDLE& _hFile, DWORD& _dwByte);
public:
	const _string& Get_Name() const { return m_strName; }
	void Set_Finished(const _bool& _bIsFinished) { this->m_bFinished = _bIsFinished; }
	const _bool& Is_Finished() const { return m_bFinished; }

	const _double& Get_TimeAcc() const { return m_dTimeAcc; }
	const _double& Get_Duration() const { return m_dDuration; }
	_bool Get_AnimTimeSituation(const _float& _divineValue);
	_bool Is_BetweenTime(const _double& _dStartTime, const _double& _dEndTime);
	_bool Is_BetweenTime_FastTime(const _uint& _iIndex = 0);
	_bool Is_BetweenTime_Clip(const _char* _pName);
	_bool Is_OverTime(const _double& _dTime);
	_bool Is_OverTime_AnimClip(const _char* _pName, const _double& _dSupTime = 0.0);

	void Reset_Data();
public:
	vector<class CChannel*> Get_Channels() { return m_Channels; }
	class CChannel* Get_Channel(const _uint& _iIndex) { return m_Channels[_iIndex]; }

	void Set_FastValue(const _float& _fFastValue) { this->m_fFastValue = _fFastValue; }
	const _float& Get_FastValue() const { return m_fFastValue; }

	void Set_SuppleFastValue(const _float& _fFastValue) { this->m_fFastValue = _fFastValue; }

	const _float& Get_SupplmentValue() const { return m_fSupplementValue; }

	const _bool& Is_SupplementSituation() const { return m_bSupplementSituation; }
	void Set_SupplementSituation(const _bool& _bSituation) { m_bSupplementSituation = _bSituation; }

	const _bool& Is_SupplementEnd() const { return m_bSupplementEnd; }
	void Set_SupplementEnd(const _bool& _bIsSuppleEnd) { this->m_bSupplementEnd = _bIsSuppleEnd; }

	 void Set_NotCalculate(const _bool& _bIsNotCalculate) { this->m_bNotCalculate = _bIsNotCalculate; }
	 void Set_ChangeSupplement(const _bool& _bIsChangeSupplement) { this->m_bChangeSupplement = _bIsChangeSupplement; }
	 const _bool& Is_ChangeSupplement() const { return m_bChangeSupplement; }

	 void Set_NotRatio(const _bool& _bIsNotRatio) { this->m_bNotRatio = _bIsNotRatio; }
	 const _bool& Is_NotRatio() const { return m_bNotRatio; }

	 void Set_ChangeRotationCalculate(const _bool& _bIsChangeRotationCalculate) { this->m_bChangeRotationCaluclate = m_bChangePositionCalculate; }
	 const _bool& Is_ChangeRotationCalculate() const { return m_bChangeRotationCaluclate; }

	 void Set_ChangePositionCalculate(const _bool& _bIsChangePositionCalculate) { this->m_bChangePositionCalculate = _bIsChangePositionCalculate; }
	 const _bool& Is_ChangePositionCalculate() const { return m_bChangePositionCalculate; }

	 const _bool& Is_StopAnimation() const { return m_bStopAnimation; }
	 void Set_StopAnimation(const _bool& _bIsStopAnim) { this->m_bStopAnimation = _bIsStopAnim; }

	 const _bool& Is_SuppleControl() const { return m_bSuppleControl; }
	 void Set_SuppleControl(const ANIM_SUPCONTROL& _eAnimSupControl) { m_eAnimSupControl = _eAnimSupControl; }
public:
	void Add_AnimFastSection(const ANIMFASTSECTION& _tAnimFastSection) { m_AnimFastSection.push_back(_tAnimFastSection); }
	vector<ANIMFASTSECTION>* Get_AnimFastSection() { return &m_AnimFastSection; }
	void ReMove_FastSection(const ANIMFASTSECTION& _tAnimFastSection);

	void Add_AnimClipSection(const ANIMCLIPSECTION& _tAnimClipSection) { m_AnimClipSection.push_back(_tAnimClipSection); }
	vector<ANIMCLIPSECTION>* Get_AnimClipSection() { return &m_AnimClipSection; }
	void Remove_AnimClipSection(const ANIMCLIPSECTION& _tAnimClipSection);
public:
	void Set_SelectAnimPos(const ANIM_POS& _eAnimPos) { this->m_eSelectAnimPos = _eAnimPos; }
	const ANIM_POS& Get_SelectAnimPos() const { return m_eSelectAnimPos; }

	void Set_SelectAnimRot(const ANIM_ROT& _eAnimRot) { this->m_eSelectAnimRot = _eAnimRot; }
	const ANIM_ROT& Get_SelectAnimRot() const { return m_eSelectAnimRot; }

	void Set_AnimSupIndex(const ANIM_SUPINDEX& _eAnimSupIndex) { this->m_eAnimSupIndex = _eAnimSupIndex; }
	const ANIM_SUPINDEX& Get_AnimSupIndex() const { return m_eAnimSupIndex; }

	const ANIM_SUPCONTROL& Get_AnimSupControl() const { return m_eAnimSupControl; }
public:
	HRESULT NativeConstruct(class CModel* _pModel, const ANIMDESC& _tAnimDesc);
	// Update Transform Matrices
	void Update_TransformMatrices(const _double& _dTimeDelta);
	// Update Trnasform Matrices
	void Update_TransformMatrices();
	// Supplement_Transform Matrix
	void Updatet_NextAnim_TransformMatrix(const _double& _dTimeDelta, CAnimation* _pNextAnim); 
	// Clone Animation
	CAnimation* Clone_Animation(class CModel* _pModel);
public:
	void Save_AnimationFastSection(FILEGROUP* _pFileGroup);
	void Load_AnimationFastSection(FILEGROUP* _pFileGroup);
	// 
	void Save_AnimationClipSection(FILEGROUP* _pFileGroup);
	void Load_AnimationClipSection(FILEGROUP* _pFileGroup);
private:
	_string											 m_strName;
	_uint												 m_iNumChannels = 0;
	_double											m_dTickPerSeconds = 0.0;
	_double											m_dDuration = 0.0;
	vector<class CChannel*>			m_Channels;
private:
	_double											m_dTimeAcc = 0.0;
	_bool												m_bFinished = false;
private:		
	_float												m_fFastValue = 1.f;
	_float												m_fSuppleFastValue = 1.f;
	_float												m_fSupplementValue = 0.0f;
	_bool												m_bSupplementSituation = false;
	_bool												m_bSupplementEnd = false;
private:
	// Not Calculate Position 
	_bool												m_bNotCalculate = false;
	_bool												m_bChangeSupplement = false;
	_bool												m_bNotRatio = false;
	_bool												m_bChangeRotationCaluclate = false;
	_bool												m_bChangePositionCalculate = false;
	_bool												m_bStopAnimation = false;
	_bool												m_bSuppleControl = false;
private:
	vector< ANIMFASTSECTION>	m_AnimFastSection;
	vector < ANIMCLIPSECTION>	m_AnimClipSection;
private:
	ANIM_POS										m_eSelectAnimPos = POS_END;
	ANIM_ROT										m_eSelectAnimRot = ROT_END;
	ANIM_SUPINDEX							m_eAnimSupIndex = SUPINDEX_END;
	ANIM_SUPCONTROL					m_eAnimSupControl = SUPCONTROL_END;
public:
	// Animation
	static CAnimation* Create(class CModel* _pModel, const ANIMDESC& _tAnimDesc);
	// override
	virtual void Free() override;
};

END