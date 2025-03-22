#pragma once
#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Tool)

class CAnimation_Tool final : public CBase
{
private:
	explicit CAnimation_Tool();
	virtual ~CAnimation_Tool() = default;
public:
	const _bool& Is_Finished() const { return m_bFinished; }
	const char* Get_Name() { return m_szName; }
public:
	HRESULT NativeConstruct(struct aiAnimation* _pAIAnim, class CModel_Tool* _pModel);
public:
	HRESULT NativeConstruct(struct aiAnimation* _pAIAnim, class CModel_Tool* _pModel, const _wstring& _strName);
public:
	// Update Transform Matrices
	void Update_TransformMatrices(const _double& _dTimeDelta);
	CAnimation_Tool* Clone_Animation(class CModel_Tool* _pModel);
private:
	_char										m_szName[MAX_PATH] = "";
	_uint										m_iNumChannels = 0;
	_double									m_dTickPerSecond = 0.0;
	_double									m_dDuration = 0.0;
	vector<class CChannel_Tool*> m_Channels;
private:
	_double									m_dTimeAcc = 0.0;
	_bool										m_bFinished = false;
public:
public:
	void Get_Data(ANIMDESC& _tDesc);
	// Animation
	static CAnimation_Tool* Create(struct aiAnimation* _pAIAnim, class CModel_Tool* _pModel);
	// Animation
	static CAnimation_Tool* Create(struct aiAnimation* _pAIAnim, class CModel_Tool* _pModel, const _wstring& _strName);
	// override
	virtual void Free() override;
};

END