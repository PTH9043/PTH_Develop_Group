#pragma once
#include "ModelDefines.h"
#include "CBase.h"

BEGIN(Model)

class CAnimation_FBX final : public CBase
{
private:
	explicit CAnimation_FBX();
	virtual ~CAnimation_FBX() = default;
public:
	void Get_Data(ANIMDESC& _tDesc);
public:
	const _bool& Is_Finished() const { return m_bFinished; }
	const char* Get_Name() { return m_szName; }
public:
	HRESULT NativeConstruct(aiAnimation* _pAIAnim, class CModel_FBX* _pModel);
public:
	HRESULT NativeConstruct(aiAnimation* _pAIAnim, class CModel_FBX* _pModel, const _wstring& _strName);
public:
	// Update Transform Matrices
	void Update_TransformMatrices(const _double& _dTimeDelta);
	CAnimation_FBX* Clone_Animation(class CModel_FBX* _pModel);
private:
	_char										m_szName[MAX_PATH] = "";
	_uint										m_iNumChannels = 0;
	_double									m_dTickPerSecond = 0.0;
	_double									m_dDuration = 0.0;
	vector<class CChannel_FBX*> m_Channels;
private:
	_double									m_dTimeAcc = 0.0;
	_bool										m_bFinished = false;
public:
	// Animation
	static CAnimation_FBX* Create(aiAnimation* _pAIAnim, class CModel_FBX* _pModel);
	// Animation
	static CAnimation_FBX* Create(aiAnimation* _pAIAnim, class CModel_FBX* _pModel, const _wstring& _strName);
	// override
	virtual void Free() override;
};

END