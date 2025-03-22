#include "CutScene_ModelController.h"
#include "GameInstance.h"
#include "Animation.h"
#include "HierarchyNode.h"
#include "Channel.h"
#include "Animator.h"
#include "AnimationClip.h"
#include "BonesCollider.h"

#pragma region ANIM

void CCutScene_ModelController::CUTSCENEANIMDESC::Control_AnimClip(const _float& _fTimeAcc)
{
	if (ClipOrder.size() > 1)
	{
		for (_uint i = 0; i < ClipOrder.size(); ++i)
		{
			if (_fTimeAcc >= ClipOrder[i].dTime)
			{
				strCurAnimClip = ClipOrder[i].strAnimClipName;
			}
		}

		if (nullptr != pOwnerAnimator)
		{
			if (strPrevAnimClip != strCurAnimClip)
			{
				pOwnerAnimator->Set_CurrentAnimationClip(strCurAnimClip);
				pOwnerAnimator->Set_Play(true);
				strPrevAnimClip = strCurAnimClip;
			}
		}
	}
	else
	{
		for (_uint i = 0; i < ClipOrder.size(); ++i)
		{
			if (_fTimeAcc >= ClipOrder[i].dTime)
			{
				strCurAnimClip = ClipOrder[i].strAnimClipName;
			}
		}

		if (nullptr != pOwnerAnimator)
		{
			if (strPrevAnimClip != strCurAnimClip)
			{
				pOwnerAnimator->Set_CurrentAnimationClip(strCurAnimClip);
				pOwnerAnimator->Set_Play(true);
				strPrevAnimClip = strCurAnimClip;
			}
		}
	}
}
void CCutScene_ModelController::CUTSCENEANIMDESC::RootBoneTransform(const _float4x4& _Matrix)
{
	if (nullptr == pOwnerAnimator || nullptr == pOwnerTransform)
		return;

	if (false == pOwnerAnimator->Is_RootBone())
		return;

	pOwnerTransform->Set_Pos(pOwnerAnimator->Get_RootBoneMatrix().r[3]);
}
void CCutScene_ModelController::CUTSCENEANIMDESC::Reset()
{
	strCurAnimClip = "";
	strPrevAnimClip = "";;
	strNextAnimClip = "";;
	strFindName = "";
}

#pragma endregion ANIM

CCutScene_ModelController::CCutScene_ModelController(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
	Safe_AddRef_DV(m_pDevice);
	Safe_AddRef_DV(m_pContext);
}

HRESULT CCutScene_ModelController::NativeConstrut()
{
	return S_OK;
}

void CCutScene_ModelController::Tick(const _double& _dTimeDelta)
{
	if (nullptr != m_tCutSceneAnimDesc.pOwnerAnimator)
	{
		m_tCutSceneAnimDesc.pOwnerAnimator->Tick(_dTimeDelta);
		
	}
}

void CCutScene_ModelController::Recieve_Data(CGameObject* _pGameObject)
{
	if (nullptr == _pGameObject)
		return;

	m_tCutSceneAnimDesc.pOwnerTransform =
		static_cast<CTransform*>(_pGameObject->Get_Component(COM_TRANSFORM));

	m_tCutSceneAnimDesc.pOwnerAnimator =
		static_cast<CAnimator*>(_pGameObject->Get_Component(COM_ANIMATOR));

	if (nullptr != m_tCutSceneAnimDesc.pOwnerAnimator)
	{
		for (auto& iter : m_tCutSceneAnimDesc.ClipOrder)
		{
			CAnimationClip* Clip = m_tCutSceneAnimDesc.pOwnerAnimator->Get_AnimationClip(iter.strAnimClipName);
			m_tCutSceneAnimDesc.AnimClipVector.push_back(Clip);
		}
	}
}

void CCutScene_ModelController::Recieve_Data(CAnimator* _pAnimator, CTransform* _pTransform)
{
	if (nullptr == _pAnimator || nullptr == _pTransform)
		return;

	m_tCutSceneAnimDesc.pOwnerTransform = _pTransform;
	m_tCutSceneAnimDesc.pOwnerAnimator = _pAnimator;
	if (nullptr != m_tCutSceneAnimDesc.pOwnerAnimator)
	{
		for (auto& iter : m_tCutSceneAnimDesc.ClipOrder)
		{
			CAnimationClip* Clip = m_tCutSceneAnimDesc.pOwnerAnimator->Get_AnimationClip(iter.strAnimClipName);
			m_tCutSceneAnimDesc.AnimClipVector.push_back(Clip);
		}
	}
}

void CCutScene_ModelController::Load(HANDLE& _hFile, DWORD* _pByte)
{
	list<_string> LIST;
	_uint iSize = 0;
	ReadFile(_hFile, &iSize, sizeof(_uint), _pByte, nullptr);
	for (_uint i = 0; i < iSize; ++i)
	{
		_string str = CMethod::Load_File_String(_hFile, *_pByte);
		LIST.push_back(str);
	}
	iSize = (_uint)m_tCutSceneAnimDesc.ClipOrder.size();
	ReadFile(_hFile, &iSize, sizeof(_uint), _pByte, nullptr);
	for (_uint i = 0; i < iSize; ++i)
	{
		_string Read = "";
		Read = CMethod::Load_File_String(_hFile, *_pByte);
		_double dTime;
		ReadFile(_hFile, &dTime, sizeof(_double), _pByte, nullptr);
		ORDERANIM tAnim;
		tAnim.strAnimClipName = Read;
		tAnim.dTime = dTime;
		m_tCutSceneAnimDesc.ClipOrder.push_back(tAnim);
		m_tCutSceneAnimDesc.dTotalPlayTime += dTime;
	}
	_double dLastTime = 0.0;
	ReadFile(_hFile, &dLastTime, sizeof(_double), _pByte, nullptr);
	m_tCutSceneAnimDesc.dTotalPlayTime += dLastTime;
}

CCutScene_ModelController* CCutScene_ModelController::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCutScene_ModelController* pInstance = new CCutScene_ModelController(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstrut()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CCutScene_ModelController::Create To Failed");
#endif
	}
	return pInstance;
}

void CCutScene_ModelController::Free()
{
	Safe_Release_DV(m_pDevice);
	Safe_Release_DV(m_pContext);
}
