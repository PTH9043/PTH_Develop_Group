#pragma once
#include "ModelDefines.h"
#include "CBase.h"

BEGIN(Model)

class CChannel_FBX final : public CBase
{
private:
	explicit CChannel_FBX();
	virtual ~CChannel_FBX() = default;

public:
	void Get_Data(CHANNELDESC& _tDesc);

public:
	HRESULT NativeConstruct(aiNodeAnim* _pChannel, class CModel_FBX* _pModel);
	void Update_TransformationMatrices(const _double& _dTimeAcc);
	CChannel_FBX* Clone_Channel(class CModel_FBX* _pModel);
private:
	char												m_szName[MAX_PATH] = "";
	_uint												m_iNumMaxKeyFrames;
	KEYFRAME*									m_pKeyFrames;
	class CHierarchyNode_FBX*		m_pHierachyNode = nullptr;

	_uint					m_iCurrentKeyFrame = 0;
public:
	static CChannel_FBX* Create(aiNodeAnim* _pChannel, class CModel_FBX* _pModel);
	virtual void Free() override;
};

END