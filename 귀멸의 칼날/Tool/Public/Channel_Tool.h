#pragma once
#include "AssimpFIleLoad_ Tool.h" 
#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Tool)
class CModel_Tool;

class CChannel_Tool final : public CBase
{
private:
	explicit CChannel_Tool();
	virtual ~CChannel_Tool() = default;

public:
	HRESULT NativeConstruct(struct aiNodeAnim* _pChannel, class CModel_Tool* _pModel);
	void Update_TransformationMatrices(const _double& _dTimeAcc);
	CChannel_Tool* Clone_Channel(class CModel_Tool* _pModel);
private:
	char												m_szName[MAX_PATH] = "";
	_uint												m_iNumMaxKeyFrames;
	KEYFRAME*									m_pKeyFrames;
	class CHierarchyNode_Tool*	m_pHierachyNode = nullptr;
	_uint												m_iCurrentKeyFrame = 0;
public:
	void Get_Data(CHANNELDESC& _tDesc);
public:
	static CChannel_Tool* Create(struct aiNodeAnim* _pChannel, class CModel_Tool* _pModel);
	virtual void Free() override;
};

END