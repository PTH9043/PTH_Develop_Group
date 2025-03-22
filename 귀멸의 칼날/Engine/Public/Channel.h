#pragma once
#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class  ENGINE_DLL CChannel final : public CBase
{
private:
	explicit CChannel();
	virtual ~CChannel() = default;
public:
	// Get Name
	const _string& Get_Name() const { return m_strName; }
	// Get Num Max Key Frames
	const _uint& Get_NumMaxKeyFrames() const { return m_iNumMaxKeyFrames; }
	// Get KEy Frame
	KEYFRAME Get_KeyFrame(const _uint& _iIndex) { return m_pKeyFrames[_iIndex]; }
	// Set Key Frame
	void Set_KeyFrame(const _uint& _iIndex) { m_iCurrentKeyFrame = _iIndex; }
	// Reset Key Framses
	void Reset_KeyFrames(const _uint& _iKeyFrames) { this->m_iCurrentKeyFrame = _iKeyFrames; }
public:
	// NativeConstruct
	HRESULT NativeConstruct(class CModel* _pModel, const CHANNELDESC& _tChannel);
	void Update_TransformationMatrices(const _double& _dTimeAcc, class CAnimation* _pAnimation);
	void Supplement_NextAnim_TransformMatrix(const _double& _dTimeAcc, CChannel* _pNextChannel, const  _float& _fRatio, class CAnimation* _pAnimation
	, const _bool& _bSupplement);
	void Reset_TransformMatrix(const _double& _dTimeAcc);
	void Reset_TransformMatrix();

	CChannel* Clone_Channel(class CModel* _pModel);

	// 현재 TimeAcc값으로 CurKeyFrame을 구합니다.
	_uint Compute_CurKeyFrame(const _double& _dTimeAcc);
private:
	_string		m_strName = " ";
	_uint		m_iNumMaxKeyFrames = 0;
	KEYFRAME*	m_pKeyFrames = nullptr;
	class CHierarchyNode* m_pHierachyNode = nullptr;
	_uint									m_iCurrentKeyFrame = 0;
public:
	void Save_Data(const HANDLE& _hFile, DWORD& _dwByte);
	static CChannel* Create(class CModel* _pModel, const CHANNELDESC& _tChannel);
	virtual void Free() override;
};

END