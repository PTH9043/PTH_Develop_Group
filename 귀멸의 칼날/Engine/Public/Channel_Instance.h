#pragma once
#include "Engine_Defines.h"
#include "Base.h"


BEGIN(Engine)
class CModel_Instance;
class CAnimation_Instance;

class ENGINE_DLL CChannel_Instance  final : public CBase
{
private:
	explicit CChannel_Instance();
	virtual ~CChannel_Instance() = default;
public:
	// Get Name
	const _string& Get_Name() const { return m_strName; }
	// Get Num Max Key Frames
	const _uint& Get_NumMaxKeyFrames() const { return m_iNumMaxKeyFrames; }
	// Get KEy Frame
	KEYFRAME Get_KeyFrame(const _uint& _iIndex) { return m_pKeyFrames[_iIndex]; }
	// Reset Key Framses
	void Reset_KeyFrames(const _uint& _iKeyFrames) { this->m_iCurrentKeyFrame = _iKeyFrames; }
public:
	// NativeConstruct
	HRESULT NativeConstruct(CModel_Instance* _pModel, const CHANNELDESC& _tChannel);
	void Update_TransformationMatrices(const _double& _dTimeAcc, CAnimation_Instance* _pAnimation);
	void Supplement_NextAnim_TransformMatrix(const _double& _dTimeAcc, CChannel_Instance* _pNextChannel, 
		const  _float& _fRatio,  CAnimation_Instance* _pAnimation);
	void Reset_TransformMatrix(const _double& _dTimeAcc);
	void Reset_TransformMatrix();

	CChannel_Instance* Clone_Channel(CModel_Instance* _pModel, const _uint& _iGroupID = 0);
private:
	_string															m_strName = " ";
	_uint																m_iNumMaxKeyFrames = 0;
	KEYFRAME*													m_pKeyFrames = nullptr;
	class CHierarchyNode_Instance*			m_pHierachyNode = nullptr;
	_uint																m_iCurrentKeyFrame = 0;
	_uint																m_iGroupID = 10000000;
public:
	void Save_Data(const HANDLE& _hFile, DWORD& _dwByte);
	static CChannel_Instance* Create(CModel_Instance* _pModel, const CHANNELDESC& _tChannel);
	virtual void Free() override;
};

END