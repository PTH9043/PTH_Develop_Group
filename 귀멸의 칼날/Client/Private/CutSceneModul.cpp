#include "CutSceneModul.h"
#include "GameInstance.h"
#include "CutSceneTex.h"
#include "Camera_CutScene.h"
#include "Battle_Manager.h"
#include "CutScene_ModelController.h"
#include "CutSceneTex.h"
#include "Character.h"
#include "Camera_Manager.h"
#include "CutScene_Manager.h"
#include "Camera_Free.h"


CCutSceneModul::CCutSceneModul()
{
}

CCutSceneModul::CCutSceneModul(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
	Safe_AddRef_DV(m_pDevice);
	Safe_AddRef_DV(m_pContext);
}

HRESULT CCutSceneModul::NativeConstruct(VOIDDATAS* _pDatas)
{
	m_pCutScene_OwnerModelCom = CCutScene_ModelController::Create(m_pDevice, m_pContext);
	m_pCutScene_TargetModelCom = CCutScene_ModelController::Create(m_pDevice, m_pContext);
	return S_OK;
}

HRESULT CCutSceneModul::Tick(const _double& _dTimeDelta)
{
	if (false == m_bPlayCutChar)
		return E_FAIL;

	_float fOwnerAnimSpeed = 1.f;
	if (m_iCutChar_Index < m_CutCharDataVector.size())
	{
		if (0.f == fOwnerAnimSpeed)
			fOwnerAnimSpeed = 1.f;

		fOwnerAnimSpeed = m_CutCharDataVector[m_iCutChar_Index].fControlTotalUpdateSpeed;
	}

	Update_CutScene(_dTimeDelta * fOwnerAnimSpeed * m_fUpdateTime);
	return S_OK;
}

HRESULT CCutSceneModul::Late_Tick(const _double& _dTimeDelta)
{
	if (false == m_bPlayCutScene)
		return E_FAIL;

	_float fOwnerAnimSpeed = 1.f;
	if (m_iCutChar_Index < m_CutCharDataVector.size())
	{
		if (0.f == fOwnerAnimSpeed)
			fOwnerAnimSpeed = 1.f;

		fOwnerAnimSpeed = m_CutCharDataVector[m_iCutChar_Index].fControlTotalUpdateSpeed;
	}

	LateUpdate_CutScene(_dTimeDelta * fOwnerAnimSpeed * m_fUpdateTime);
	return S_OK;
}

HRESULT CCutSceneModul::Load_Data(FILEDATA* _pFileData)
{
	if (nullptr == _pFileData)
		return E_FAIL;

	HANDLE hFile = CMethod::Begin_Load(_pFileData->strFilePath.c_str());
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;


	_wstring str = L"";
	DWORD dwByte = 0;
	ReadFile(hFile, &m_dTotalTime, sizeof(_double), &dwByte, nullptr);
	ReadFile(hFile, &m_fUpdateTime, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &m_fUpdateLateTime, sizeof(_float), &dwByte, nullptr);
	ReadFile(hFile, &m_bRootBone_Owner, sizeof(_bool), &dwByte, nullptr);
	ReadFile(hFile, &m_bRootBone_Target, sizeof(_bool), &dwByte, nullptr);
	m_strName = CMethod::Load_File_WString(hFile, dwByte);
	m_strInputTrigger = CMethod::Load_File_String(hFile, dwByte);

	_uint iSize = 0;
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iSize; ++i)
	{
		CUTSCENEDATA tData;
		// ========================================================
		ReadFile(hFile, &tData, sizeof(CUTSCENEDATA), &dwByte, nullptr);
		m_CutSceneDataVector.push_back(tData);
	}

	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iSize; ++i)
	{
		CUTCHARDATA tData;
		ReadFile(hFile, &tData, sizeof(CUTCHARDATA), &dwByte, nullptr);
		m_CutCharDataVector.push_back(tData);
	}

	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iSize; ++i)
	{
		CUTDMGDATA tData;
		ReadFile(hFile, &tData, sizeof(CUTDMGDATA), &dwByte, nullptr);
		m_CutDmgDataVector.push_back(tData);
	}

	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < iSize; ++i)
	{
		CCutSceneTex* pInstance = CCutSceneTex::Create(m_pDevice, m_pContext);
		pInstance->Load_CutSceneTex(hFile, &dwByte);
		m_CutSceneTexVector.push_back(pInstance);
	}

	if (m_CutSceneTexVector.size() > 0)
	{
		_wstring TextureLoad = CMethod::Load_File_WString(hFile, dwByte);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		{
			FILEGROUP* pFileGroup =  pGameInstance->Find_Folder(TextureLoad);
			m_pTexture = CTexture::Create(m_pDevice, m_pContext, pFileGroup);
		
			if (nullptr != m_pTexture)
			{
				m_VIBuffer_CutSceneTexture = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
				m_pCutScene_Shader = (CShader*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_SHADER_VTXTEX);
			}
		}
		RELEASE_INSTANCE(CGameInstance);

		XMStoreFloat4x4(&m_Texture_WorldMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_Texture_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_Texture_ProjMatrix, XMMatrixIdentity());

		XMStoreFloat4x4(&m_Texture_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f)));

		m_Texture_WorldMatrix._11 = g_iWinSizeX;
		m_Texture_WorldMatrix._22 = g_iWinSizeY;
		XMStoreFloat4x4(&m_Texture_WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_Texture_WorldMatrix)));
	}

	if (nullptr != m_pCutScene_OwnerModelCom)
	{
		m_pCutScene_OwnerModelCom->Load(hFile, &dwByte);
		m_dTotalTime = m_pCutScene_OwnerModelCom->Get_CutSceneAnimDesc()->dTotalPlayTime;
	}

	if (nullptr != m_pCutScene_TargetModelCom)
		m_pCutScene_TargetModelCom->Load(hFile, &dwByte);

	CloseHandle(hFile);
	return S_OK;
}

void CCutSceneModul::Start_CutScene(CCharacter* _pChar1, CCharacter* _pChar2)
{
	if (nullptr == _pChar1 || nullptr == _pChar2)
		return;

	if (nullptr != m_pCutScene_OwnerModelCom)
		m_pCutScene_OwnerModelCom->Recieve_Data(_pChar1);

	if (nullptr != m_pCutScene_TargetModelCom)
		m_pCutScene_TargetModelCom->Recieve_Data(_pChar2);

	m_pOwnerCharacter = _pChar1;
	m_pTargetCharacter = _pChar2;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	{
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
		{
			for (auto& iter : pInstance->Find_GameObjectList(g_iCurrentLevel, LAYER_CAM))
			{
				CCamera_CutScene* pCam = dynamic_cast<CCamera_CutScene*>(iter);
				if (nullptr != pCam)
				{
					m_pCamCutScene = pCam;
				}
				else
				{
					CCamera_Free* CamFree = dynamic_cast<CCamera_Free*>(iter);
					if (nullptr != CamFree)
						m_pCameraFree = CamFree;
				}
			}
		}
		RELEASE_INSTANCE(CGameInstance);
	}
	RELEASE_INSTANCE(CGameInstance);

	m_bPlayCutScene = false;
	m_bPlayCutChar = false;
}

void CCutSceneModul::Update_CutScene(const _double& _dTimeDelta)
{
	if (false == m_bNotChangeCutScene)
	{
		if (nullptr != m_pCutScene_OwnerModelCom)
		{
			m_pCutScene_OwnerModelCom->Get_CutSceneAnimDesc()->Control_AnimClip(m_fCurrentCharTime);
			m_pCutScene_OwnerModelCom->Tick(_dTimeDelta);
		}

		if (nullptr != m_pCutScene_TargetModelCom)
		{
			m_pCutScene_TargetModelCom->Get_CutSceneAnimDesc()->Control_AnimClip(m_fCurrentCharTime);
			m_pCutScene_TargetModelCom->Tick(_dTimeDelta);
		}
	}
	else
	{
		if ((_double)(m_fCurrentCharTime) < m_dTotalTime - 0.05)
		{
			if (nullptr != m_pCutScene_OwnerModelCom)
			{
				m_pCutScene_OwnerModelCom->Get_CutSceneAnimDesc()->Control_AnimClip(m_fCurrentCharTime);
				m_pCutScene_OwnerModelCom->Tick(_dTimeDelta);
			}

			if (nullptr != m_pCutScene_TargetModelCom)
			{
				m_pCutScene_TargetModelCom->Get_CutSceneAnimDesc()->Control_AnimClip(m_fCurrentCharTime);
				m_pCutScene_TargetModelCom->Tick(_dTimeDelta);
			}
		}
	}

	if (nullptr != m_pCamCutScene)
	{
		if (nullptr != m_pCamCutScene->Get_Transform())
			m_vSaveCamPosition = m_pCamCutScene->Get_Transform()->Get_Pos();
	}

	if (true == m_bPlayCutChar)
	{
		// End
		if (m_fCurrentCharTime >= m_dTotalTime)
		{
			return;
		}
		// Ing
		else
		{
			if (m_iCutDmg_Index < m_CutDmgDataVector.size())
			{
				if (m_CutDmgDataVector[m_iCutDmg_Index].fCurTime <= m_fCurrentCharTime)
				{
					if (nullptr != m_pTargetCharacter)
					{
						m_pTargetCharacter->Get_BattleData().Decrease_HP(m_CutDmgDataVector[m_iCutDmg_Index].fDmgData);
					}
					++m_iCutDmg_Index;
				}
			}

			if (m_iCutChar_Index < m_CutCharDataVector.size())
			{
				if (m_CutCharDataVector[m_iCutChar_Index].fCurTime <= m_fCurrentCharTime)
				{
					++m_iCutChar_Index;
					// Cut Scene Data Lis 
					if (m_CutCharDataVector.size() > m_iCutChar_Index)
					{
						Bind_NextCharData();
					}
				}
			}
			MoveTo_Char(_dTimeDelta);
			RotateTo_Char(_dTimeDelta);
			JumpTo_Target(_dTimeDelta);

			if (m_iCutChar_Index < m_CutCharDataVector.size())
			{
				m_fOwnerDeltaSpeed += (_float)(_dTimeDelta)*m_CutCharDataVector[m_iCutChar_Index].fOwnerDeltaSpeed;
				m_fTargetDeltaSpeed += (_float)(_dTimeDelta)*m_CutCharDataVector[m_iCutChar_Index].fTargetDeltaSpeed;
				m_vOwnerRotationDeltaSpeed = m_CutCharDataVector[m_iCutChar_Index].vOwnerRotationDeltaSpeed *
					(_float)(_dTimeDelta);
				m_vTargetRotationDeltaSpeed = m_CutCharDataVector[m_iCutChar_Index].vTargetRotationDeltaSpeed *
					(_float)(_dTimeDelta);

				if (nullptr != m_pOwnerCharacter)
					m_pOwnerCharacter->Set_Active(m_CutCharDataVector[m_iCutChar_Index].bOwnerActive);
				if (nullptr != m_pTargetCharacter)
					m_pTargetCharacter->Set_Active(m_CutCharDataVector[m_iCutChar_Index].bTargetActive);
			}

			// Cut Scene DataList
			m_fCurrentCharTime += (_float)(_dTimeDelta);
			// Cut Scene DataList
		}
	}

	if (nullptr != m_pOwnerNavigation)
	{
		if (!m_pOwnerNavigation->Is_Move(m_pCutScene_OwnerModelCom->Get_Transform()->Get_Pos()))
			m_pOwnerCharacter->Get_Transform()->Set_Pos(m_vSaveOwnerPosition.Get_Vector());
		else
		{
			if (nullptr != m_pOwnerCharacter->Get_Transform())
				m_vSaveOwnerPosition = m_pOwnerCharacter->Get_Transform()->Get_Pos();
		}
	}

	if (nullptr != m_pTargetNavigation)
	{
		if (!m_pTargetNavigation->Is_Move(m_pCutScene_TargetModelCom->Get_Transform()->Get_Pos()))
			m_pTargetCharacter->Get_Transform()->Set_Pos(m_vSaveTargetPosition.Get_Vector());
		else
		{
			if (nullptr != m_pTargetCharacter->Get_Transform())
				m_vSaveTargetPosition = m_pTargetCharacter->Get_Transform()->Get_Pos();
		}
	}
}

void CCutSceneModul::LateUpdate_CutScene(const _double& _dTimeDelta)
{
	if (nullptr == m_pCamCutScene)
		return;

	if (true == m_bPlayCutScene)
	{
		// End
		if (m_fCurrentCamTime >= m_dTotalTime)
		{
			if (false == m_bNotChangeCutScene)
			{
				if (nullptr != m_pOwnerCharacter)
					m_pOwnerCharacter->Set_ActiveCutScene(false);

				if (nullptr != m_pTargetCharacter)
					m_pTargetCharacter->Set_ActiveCutScene(false);
				
				Reset_Char();
				Reset_Cam();
			}

			m_bIsEnd = true;
			return;
		}
		// Ing
		else
		{
			if (m_iCutScene_Index < m_CutSceneDataVector.size())
			{
				if (m_CutSceneDataVector[m_iCutScene_Index].fCurTime <= m_fCurrentCamTime)
				{
					++m_iCutScene_Index;
					// Cut Scene Data Lis 
					if (m_CutSceneDataVector.size() > m_iCutScene_Index)
					{
						Bind_NextSceneData();
					}
				}
				else
				{
					if (false == m_CutSceneDataVector[m_iCutScene_Index].bCurNodeNearDisNotPlus)
					{
						if (m_CutSceneDataVector[m_iCutScene_Index].bCamMove || m_CutSceneDataVector[m_iCutScene_Index].bSplineCam
							|| m_CutSceneDataVector[m_iCutScene_Index].bUseBezier)
						{
							_float3 v3Length = (m_CutSceneDataVector[m_iCutScene_Index].vCamPosition.Coord4x4(m_StartWorldMatrix)) -
								m_pCamCutScene->Get_Transform()->Get_PosF3();
							if (v3Length.Length() <= 1.f)
							{
								++m_iCutScene_Index;
								// Cut Scene Data Lis 
								if (m_CutSceneDataVector.size() > m_iCutScene_Index)
								{
									Bind_NextSceneData();
								}
							}
						}
					}
				}

				// Look At Cam
				LookAt_ToCam(_dTimeDelta );
				// Move To Cam
				MoveTo_Cam(_dTimeDelta );
				// Lerp To cam
				LerpTo_Cam(_dTimeDelta );
				// Shake
				CamShake(_dTimeDelta );
				if (m_iCutScene_Index < m_CutSceneDataVector.size())
					m_fCamDeltaSpeed += (_float)(_dTimeDelta )*m_CutSceneDataVector[m_iCutScene_Index].fDeltaSpeed;
			}
			// Cut Scene DataList
			m_fCurrentCamTime += (_float)(_dTimeDelta);
		}
	}
}

void CCutSceneModul::Active_CutScene()
{
	if (nullptr == m_pCamCutScene || nullptr == m_pCameraFree)
	{
		CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
		{
			for (auto& iter : pInstance->Find_GameObjectList(g_iCurrentLevel, LAYER_CAM))
			{
				CCamera_CutScene* pCam = dynamic_cast<CCamera_CutScene*>(iter);
				if (nullptr != pCam)
				{
					m_pCamCutScene = pCam;
				}
				else
				{
					CCamera_Free* CamFree = dynamic_cast<CCamera_Free*>(iter);
					if (nullptr != CamFree)
						m_pCameraFree = CamFree;
				}
			}
		}
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (true == m_bNotReceivePosition)
	{
		m_StartWorldMatrix = CMethod::XMMatrixIdentity_P();
		m_RotationWorldMatrix = CMethod::XMMatrixIdentity_P();
	}
	else
	{
		m_StartWorldMatrix = m_pOwnerCharacter->Get_Transform()->Get_WorldFloat4x4();
		{
			OUTMATRIX OutMatrix = CMethod::Out_Matrix(m_StartWorldMatrix);
			m_RotationWorldMatrix = CMethod::XMMatrix4x4SetFloat3(_float3(0.f, 0.f, 0.f), MATROW_POS, m_StartWorldMatrix);
			_float3 vPos = OutMatrix.vPos;
			vPos = vPos / 2.f;
			vPos.y = OutMatrix.vPos.y;
			m_pOwnerCharacter->Get_Transform()->Set_Pos(vPos.Get_Vector()); 
			m_StartWorldMatrix = m_pOwnerCharacter->Get_Transform()->Get_WorldFloat4x4();
			m_vOwnerRotAngles = OutMatrix.vRot;
		}
		{
			OUTMATRIX OutMatrix = CMethod::Out_Matrix(m_pTargetCharacter->Get_Transform()->Get_WorldFloat4x4());
			m_vTargetRotAngles = OutMatrix.vRot;
		}
	}


	if (nullptr != m_pCamCutScene)
	{
		m_pCamCutScene->Get_Transform()->Bind_WorldMatrix(XMMatrixIdentity());
	}

	if (nullptr != m_pOwnerCharacter)
	{
		m_pOwnerCharacter->Set_ActiveCutScene(true);
		m_pOwnerCharacter->Set_Active(true);
	}

	if (nullptr != m_pTargetCharacter)
	{
		m_pTargetCharacter->Set_ActiveCutScene(true);
		m_pTargetCharacter->Set_Active(true);
	}

	m_pCamCutScene->Set_Active(true);
	m_bPlayCutChar = true;
	m_bPlayCutScene = true;
	m_bIsEnd = false;
	CCamera_Manager* pManager = GET_INSTANCE(CCamera_Manager)
	{
		pManager->Set_ActiveCamera(PROTO_GOB_CAMERA_FREE);
	}
	RELEASE_INSTANCE(CCamera_Manager);

	m_iCutScene_Index = 0;
	m_iCutChar_Index = 0;
	m_fCurrentCharTime = 0.f;
	m_fCurrentCamTime = 0.f;

	if (nullptr != m_pCamCutScene)
	{
		CTransform* pTransform = m_pCamCutScene->Get_Transform();
		if (nullptr != pTransform)
		{
			pTransform->Set_Pos(Coord_Position(m_CutSceneDataVector[0].vCamPosition));
			pTransform->Rotate_Turn(Rotation_Convert(m_CutSceneDataVector[0].vCamRotation));
		}
	}

	if (nullptr != m_pCutScene_OwnerModelCom)
	{
		m_pCutScene_OwnerModelCom->Get_CutSceneAnimDesc()->Reset();

		m_pOwnerCharacter->Get_Transform()->Set_Pos(Coord_Position(m_CutCharDataVector[0].vOwnerPosition));
		m_pOwnerCharacter->Get_Transform()->Rotate_Turn(Rotation_Convert(m_CutCharDataVector[0].vOwnerRotation));
		m_pOwnerNavigation = m_pOwnerCharacter->Get_Transform()->Get_Navigation();
		m_pOwnerCharacter->Get_Transform()->Update_Nav(nullptr);
	}

	if (nullptr != m_pCutScene_OwnerModelCom)
	{
		m_pCutScene_TargetModelCom->Get_CutSceneAnimDesc()->Reset();

		m_pTargetCharacter->Get_Transform()->Set_Pos(Coord_Position(m_CutCharDataVector[0].vTargetPosition));
		m_pTargetCharacter->Get_Transform()->Rotate_Turn(Rotation_Convert(m_CutCharDataVector[0].vTargetRotation));
		m_pTargetNavigation = m_pTargetCharacter->Get_Transform()->Get_Navigation();
		m_pTargetCharacter->Get_Transform()->Update_Nav(nullptr);

		if (true == m_bNotChangeCutScene)
		{
			m_pTargetCharacter->Set_RenderEnable(false);
		}
		else
		{
			m_pTargetCharacter->Set_RenderEnable(true);
		}
	}
}

void CCutSceneModul::Render_Texture()
{
	if (nullptr == m_pTexture)
		return;

	if (m_iCutScene_Index < m_CutSceneDataVector.size())
	{
		if (true == m_CutSceneDataVector[m_iCutScene_Index].bCutSceneTexShow)
		{
			if (m_iCutSceneTextureIndex < m_CutSceneTexVector.size())
			{
				if (nullptr != m_CutSceneTexVector[m_iCutSceneTextureIndex])
				{
					if (m_CutSceneTexVector[m_iCutSceneTextureIndex]->Get_CurTime() >= m_fCurrentCamTime)
					{
						m_CutSceneTexVector[m_iCutSceneTextureIndex]->Render_Screen(m_Texture_WorldMatrix, m_Texture_ViewMatrix, m_Texture_ProjMatrix,
							m_pCutScene_Shader, m_pTexture, m_VIBuffer_CutSceneTexture);
					}
					else
					{
						++m_iCutSceneTextureIndex;
						if (m_iCutScene_Index < m_CutSceneTexVector.size())
						{
							m_CutSceneTexVector[m_iCutSceneTextureIndex]->Render_Screen(m_Texture_WorldMatrix, m_Texture_ViewMatrix, m_Texture_ProjMatrix,
								m_pCutScene_Shader, m_pTexture, m_VIBuffer_CutSceneTexture);
						}
					}
				}
			}
		}
	}
}

void CCutSceneModul::Bind_NextSceneData()
{
	if (nullptr == m_pCamCutScene)
		return;

	m_fLerpAddValue = 0.f;
	m_fMoveSplineValue = 0.f;
	m_fMoveBezierValue = 0.f;
	m_fCamDeltaSpeed = 0.f;
	if (false == m_CutSceneDataVector[m_iCutScene_Index].bCamMove && false == m_CutSceneDataVector[m_iCutScene_Index].bSplineCam
		&& false == m_CutSceneDataVector[m_iCutScene_Index].bUseBezier)
	{
		m_pCamCutScene->Get_Transform()->Set_Pos(Coord_Position(m_CutSceneDataVector[m_iCutScene_Index].vCamPosition));
	}

	if (false == m_CutSceneDataVector[m_iCutScene_Index].bCamOwnerLook && false == m_CutSceneDataVector[m_iCutScene_Index].bCamTargetLook
		&& false == m_CutSceneDataVector[m_iCutScene_Index].bCamLerp)
	{
		m_pCamCutScene->Get_Transform()->Rotate_Turn(Rotation_Convert(m_CutSceneDataVector[m_iCutScene_Index].vCamRotation));
	}

	if (m_CutSceneDataVector[m_iCutScene_Index].bCutSceneTexShow)
		++m_iCutSceneTextureIndex;

	m_tMoveShakeTimer = CUSTIMER(m_CutSceneDataVector[m_iCutScene_Index].fMoveShakePassActiveTime);
	m_tRotShakeTimer = CUSTIMER(m_CutSceneDataVector[m_iCutScene_Index].fRotShakePassActiveTime);
	m_tMoveShakeTimer.bPass = true;
	m_tRotShakeTimer.bPass = true;
}

void CCutSceneModul::LookAt_ToCam(const _double& _dTimeDelta)
{
	if (m_CutSceneDataVector.size() > m_iCutScene_Index)
	{
		if (nullptr == m_pCamCutScene)
			return;

		CTransform* pCamTransform = m_pCamCutScene->Get_Transform();
		if (nullptr == pCamTransform)
			return;

		if (true == m_CutSceneDataVector[m_iCutScene_Index].bCamOwnerLook)
		{
			_float3 vLook;
			if (nullptr == m_pCutScene_OwnerModelCom)
				return;

			CTransform* pOwnerTr = m_pCutScene_OwnerModelCom->Get_Transform();
			if (nullptr == pOwnerTr)
				return;

			vLook = pOwnerTr->Get_PosF3();
			vLook.y = 0.f;
			pCamTransform->LookAt(pCamTransform->Get_Pos() + vLook.Get_Vector());
		}
		else if (true == m_CutSceneDataVector[m_iCutScene_Index].bCamTargetLook)
		{
			_float3 vLook;
			if (nullptr == m_pCutScene_TargetModelCom)
				return;

			CTransform* pOwnerTr = m_pCutScene_TargetModelCom->Get_Transform();
			if (nullptr == pOwnerTr)
				return;

			vLook = pOwnerTr->Get_PosF3();
			vLook.y = 0.f;
			pCamTransform->LookAt(pCamTransform->Get_Pos() + vLook.Get_Vector());
		}
	}
}

void CCutSceneModul::MoveTo_Cam(const _double& _dTimeDelta)
{
	if (m_CutSceneDataVector.size() > m_iCutScene_Index)
	{
		if (true == m_CutSceneDataVector[m_iCutScene_Index].bCamMove)
		{
			MoveTo_CamJust(_dTimeDelta);
		}
		else if (true == m_CutSceneDataVector[m_iCutScene_Index].bSplineCam)
		{
			MoveTo_CamSpline(_dTimeDelta);
		}
		else if (true == m_CutSceneDataVector[m_iCutScene_Index].bUseBezier)
		{
			MoveTo_CamBezier(_dTimeDelta);
		}
	}
}

void CCutSceneModul::MoveTo_CamJust(const _double& _dTimeDelta)
{
	if (0 == m_iCutScene_Index)
		return;

	CTransform* pCamTransform = m_pCamCutScene->Get_Transform();
	if (nullptr == pCamTransform)
		return;

	pCamTransform->Translate_Pos(Coord_Position(m_CutSceneDataVector[m_iCutScene_Index].vCamPosition), _dTimeDelta,
		m_CutSceneDataVector[m_iCutScene_Index].fCamSpeed + m_fCamDeltaSpeed , 0.5f);
}

void CCutSceneModul::MoveTo_CamSpline(const _double& _dTimeDelta)
{
	if (m_CutSceneDataVector.size() > m_iCutScene_Index)
	{
		if (false == m_CutSceneDataVector[m_iCutScene_Index].bSplineCam)
			return;

		if (nullptr == m_pCamCutScene)
			return;

		if (0 == m_iCutScene_Index)
			return;

		CTransform* pCamTransform = m_pCamCutScene->Get_Transform();
		if (nullptr == pCamTransform)
			return;

		if (m_CutSceneDataVector.size() > m_iCutScene_Index + 1)
		{
			if (m_CutSceneDataVector.size() > m_iCutScene_Index + 2)
			{
				_vector vPrevPos = Coord_Position(m_CutSceneDataVector[m_iCutScene_Index - 1].vCamPosition);
				_vector vPos = Coord_Position(m_CutSceneDataVector[m_iCutScene_Index].vCamPosition);
				_vector vNext1 = Coord_Position(m_CutSceneDataVector[m_iCutScene_Index + 1].vCamPosition);
				_vector vNext2 = Coord_Position(m_CutSceneDataVector[m_iCutScene_Index + 2].vCamPosition);

				_float fMulti = m_CutSceneDataVector[m_iCutScene_Index].fSplineCamSpeed;
				if (0 == fMulti)
					fMulti = 1.f;

				if (m_fMoveSplineValue > 1.f)
				{
					m_fMoveSplineValue = 1.f;
					_vector Pos = CMethod::Compute_SplineCurve(vPrevPos, vPos, vNext1, vNext2, m_fMoveSplineValue);
					pCamTransform->Translate_Pos(Pos, _dTimeDelta * fMulti, m_CutSceneDataVector[m_iCutScene_Index].fCamSpeed + m_fCamDeltaSpeed, 0.5f);
				}
				else
				{
					_vector Pos = CMethod::Compute_SplineCurve(vPrevPos, vPos, vNext1, vNext2, m_fMoveSplineValue);
					pCamTransform->Translate_Pos(Pos, _dTimeDelta * fMulti, m_CutSceneDataVector[m_iCutScene_Index].fCamSpeed + m_fCamDeltaSpeed, 0.5f);
					m_fMoveSplineValue += (_float)(_dTimeDelta)*fMulti;
				}
			}
			else
			{
				MoveTo_CamJust(_dTimeDelta);
			}
		}
		else
		{
			MoveTo_CamJust(_dTimeDelta);
		}
	}
}

void CCutSceneModul::MoveTo_CamBezier(const _double& _dTimeDelta)
{
	if (m_CutSceneDataVector.size() > m_iCutScene_Index)
	{
		if (false == m_CutSceneDataVector[m_iCutScene_Index].bUseBezier)
			return;

		if (nullptr == m_pCamCutScene)
			return;

		if (0 == m_iCutScene_Index)
			return;

		CTransform* pCamTransform = m_pCamCutScene->Get_Transform();
		if (nullptr == pCamTransform)
			return;

		if (m_CutSceneDataVector.size() > 0)
		{
			_vector vP1 = Coord_Position(m_CutSceneDataVector[m_iCutScene_Index - 1].vCamPosition);
			_vector vB1 = Coord_Position(m_CutSceneDataVector[m_iCutScene_Index].vBezierPrevPos);
			_vector vP2 = Coord_Position(m_CutSceneDataVector[m_iCutScene_Index].vCamPosition);
			_vector vB2 = Coord_Position(m_CutSceneDataVector[m_iCutScene_Index].vBezierPos);

			_float fMulti = m_CutSceneDataVector[m_iCutScene_Index].fBezierSpeed;
			if (0 == fMulti)
				fMulti = 1.f;

			if (m_fMoveBezierValue > 1.f)
			{
				m_fMoveBezierValue = 1.f;
				_vector Pos = CMethod::Compute_BezierCurve(vP1, vB1, vB2, vP2, m_fMoveBezierValue);
				pCamTransform->Translate_Pos(Pos, _dTimeDelta, m_CutSceneDataVector[m_iCutScene_Index].fCamSpeed + m_fCamDeltaSpeed, 0.5f);
			}
			else
			{
				_vector Pos = CMethod::Compute_BezierCurve(vP1, vB1, vB2, vP2, m_fMoveBezierValue);
				pCamTransform->Translate_Pos(Pos, _dTimeDelta, m_CutSceneDataVector[m_iCutScene_Index].fCamSpeed + m_fCamDeltaSpeed, 0.5f);
				m_fMoveBezierValue += (_float)(_dTimeDelta)*fMulti;
			}
		}
		else
		{
			MoveTo_CamJust(_dTimeDelta);
		}
	}
}

void CCutSceneModul::LerpTo_Cam(const _double& _dTimeDelta)
{
	if (m_CutSceneDataVector.size() > m_iCutScene_Index)
	{
		if (nullptr == m_pCamCutScene)
			return;

		CTransform* pCamTransform = m_pCamCutScene->Get_Transform();
		if (nullptr == pCamTransform)
			return;

		if (0 == m_iCutScene_Index)
			return;


		if (true == m_CutSceneDataVector[m_iCutScene_Index].bCamLerp)
		{
			OUTMATRIX OutMatrix = CMethod::Out_Matrix(pCamTransform->Get_WorldFloat4x4());

			if (OutMatrix.vRot.y >= 180.f)
				OutMatrix.vRot.y -= 360.f;
			else if (OutMatrix.vRot.y <= -180.f)
				OutMatrix.vRot.y += 360.f;

			_float3 v3Length = OutMatrix.vPos - pCamTransform->Get_Pos();
			if (v3Length.Length() <= 0.0001f)
				return;

			if (false == m_CutSceneDataVector[m_iCutScene_Index].bDistanceToRotLerp)
			{
				_float fValue = m_CutSceneDataVector[m_iCutScene_Index].fCamLerpSpeed;
				if (fValue == 0.f)
					fValue = 1.f;

				_float fSpeed = 0.f;
				if (0 != fValue)
					fSpeed = (_float)(_dTimeDelta)*fValue * 0.25f;
				else
					fSpeed = (_float)(_dTimeDelta) * 0.25f;

				if (m_fLerpAddValue <= m_CutSceneDataVector[m_iCutScene_Index].fLerpRestrict)
				{
					_float3 vRotation = _float3(0.f, 0.f, 0.f);

					_float3 v3ConvertRotation = Rotation_Convert(m_CutSceneDataVector[m_iCutScene_Index].vCamRotation);
					_float fValue = OutMatrix.vRot.y - v3ConvertRotation.y;
					if (fabs(fValue) >= 180.f)
					{
						if (v3ConvertRotation.y > 0)
							v3ConvertRotation.y -= 360.f;
						else
							v3ConvertRotation.y += 360.f;
					}

					vRotation = XMVectorLerp(OutMatrix.vRot.Get_Vector(), v3ConvertRotation.Get_Vector(), fSpeed);
					pCamTransform->Rotate_Turn(vRotation);
				}
				m_fLerpAddValue += fSpeed;
			}
			else
			{
				_float fDistnace = m_CutSceneDataVector[m_iCutScene_Index].fPrevDistance;
				if (fDistnace == 0.f)
					fDistnace = 1.f;

				_float fValue = m_CutSceneDataVector[m_iCutScene_Index].fCamLerpSpeed * (1 / fDistnace) *
					m_CutSceneDataVector[m_iCutScene_Index].fCamSpeed;
				if (fValue == 0.f)
					fValue = 1.f;

				_float fSpeed = 0.f;
				if (0 != fValue)
					fSpeed = (_float)(_dTimeDelta)*fValue * 0.25f;
				else
					fSpeed = (_float)(_dTimeDelta) * 0.25f;

				if (m_fLerpAddValue <= m_CutSceneDataVector[m_iCutScene_Index].fLerpRestrict)
				{

					_float3 vRotation = _float3(0.f, 0.f, 0.f);

					_float3 v3ConvertRotation = Rotation_Convert(m_CutSceneDataVector[m_iCutScene_Index].vCamRotation);
					_float fValue = OutMatrix.vRot.y - v3ConvertRotation.y;
					if (fabs(fValue) >= 180.f)
					{
						if (v3ConvertRotation.y > 0)
							v3ConvertRotation.y -= 360.f;
						else
							v3ConvertRotation.y += 360.f;
					}

					vRotation = XMVectorLerp(OutMatrix.vRot.Get_Vector(), v3ConvertRotation.Get_Vector(), fSpeed);
					pCamTransform->Rotate_Turn(vRotation);
				}
				m_fLerpAddValue += fSpeed;
			}
		}
	}
}

void CCutSceneModul::CamShake(const _double& _dTimeDelta)
{
	if (m_CutSceneDataVector.size() > m_iCutScene_Index)
	{
		if (nullptr == m_pCamCutScene)
			return;

		CTransform* pCamTransform = m_pCamCutScene->Get_Transform();
		if (nullptr == pCamTransform)
			return;

		if (true == m_CutSceneDataVector[m_iCutScene_Index].bCamMoveShake)
		{
			if (true == m_tMoveShakeTimer.Is_Over(_dTimeDelta))
			{
				_float3 v3Position = Coord_Position(m_CutSceneDataVector[m_iCutScene_Index].vCamPosition);
				if (0 != m_CutSceneDataVector[m_iCutScene_Index].v3ShakePosValue.x)
					v3Position.x += (_float)(rand() % 100 * m_CutSceneDataVector[m_iCutScene_Index].v3ShakePosValue.x * Multiple_Minus_Random());
				if (0 != m_CutSceneDataVector[m_iCutScene_Index].v3ShakePosValue.y)
					v3Position.y += (_float)(rand() % 100 * m_CutSceneDataVector[m_iCutScene_Index].v3ShakePosValue.y * Multiple_Minus_Random());
				if (0 != m_CutSceneDataVector[m_iCutScene_Index].v3ShakePosValue.z)
					v3Position.z += (_float)(rand() % 100 * m_CutSceneDataVector[m_iCutScene_Index].v3ShakePosValue.z * Multiple_Minus_Random());

				if (false == m_CutSceneDataVector[m_iCutScene_Index].bCamStrongShake)
				{
					pCamTransform->Translate_Pos(v3Position.Get_Vector(), _dTimeDelta, 1.f);
				}
				else
				{
					pCamTransform->Set_Pos(v3Position.Get_Vector());
				}

				m_tMoveShakeTimer.Reset_Timer();
			}
		}
		if (true == m_CutSceneDataVector[m_iCutScene_Index].bCamRotShake)
		{
			if (true == m_tRotShakeTimer.Is_Over(_dTimeDelta))
			{
				_float3 v3Rotation = Rotation_Convert(m_CutSceneDataVector[m_iCutScene_Index].vCamRotation);
				if (0 != m_CutSceneDataVector[m_iCutScene_Index].u3ShakeRotValue.x)
					v3Rotation.x += (_float)(rand() % m_CutSceneDataVector[m_iCutScene_Index].u3ShakeRotValue.x * Multiple_Minus_Random());
				if (0 != m_CutSceneDataVector[m_iCutScene_Index].u3ShakeRotValue.y)
					v3Rotation.y += (_float)(rand() % m_CutSceneDataVector[m_iCutScene_Index].u3ShakeRotValue.y * Multiple_Minus_Random());
				if (0 != m_CutSceneDataVector[m_iCutScene_Index].u3ShakeRotValue.z)
					v3Rotation.z += (_float)(rand() % m_CutSceneDataVector[m_iCutScene_Index].u3ShakeRotValue.z * Multiple_Minus_Random());

				pCamTransform->Rotate_Turn(v3Rotation);
				m_tRotShakeTimer.Reset_Timer();
			}
		}
	}
}

_int CCutSceneModul::Multiple_Minus_Random()
{
	_int iRand = 0;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		iRand = pGameInstance->Get_RandomCount(100) % 2;
	}
	RELEASE_INSTANCE(CGameInstance);

	if (iRand == 0)
		return -1;

	return 1;
}

void CCutSceneModul::Bind_NextCharData()
{
	if (nullptr == m_pCutScene_OwnerModelCom || nullptr == m_pCutScene_TargetModelCom
		|| nullptr == m_pOwnerCharacter || nullptr == m_pTargetCharacter)
		return;

	if (m_CutCharDataVector.size() <= 0)
		return;
	if (nullptr != m_pCutScene_OwnerModelCom)
	{
		if (false == m_CutCharDataVector[m_iCutChar_Index].bOwnerMove && false == m_bRootBone_Owner)
		{
			m_pOwnerCharacter->Get_Transform()->Set_Pos(Coord_Position(m_CutCharDataVector[m_iCutChar_Index].vOwnerPosition));
		}
	}
	m_pOwnerCharacter->Get_Transform()->Rotate_Turn(Rotation_Convert(m_CutCharDataVector[m_iCutChar_Index].vOwnerRotation));
	m_pOwnerCharacter->Set_Active(m_CutCharDataVector[m_iCutChar_Index].bOwnerActive);
	if (nullptr != m_pCutScene_TargetModelCom)
	{
		if (false == m_CutCharDataVector[m_iCutChar_Index].bTargetMove && false == m_CutCharDataVector[m_iCutChar_Index].bJump
			&& false == m_bRootBone_Target)
		{
			m_pTargetCharacter->Get_Transform()->Set_Pos(Coord_Position(m_CutCharDataVector[m_iCutChar_Index].vTargetPosition));
		}
	}
	m_pTargetCharacter->Get_Transform()->Rotate_Turn(Rotation_Convert(m_CutCharDataVector[m_iCutChar_Index].vTargetRotation));
	m_pTargetCharacter->Set_Active(m_CutCharDataVector[m_iCutChar_Index].bTargetActive);

	m_fOwnerDeltaSpeed = 0.f;
	m_fTargetDeltaSpeed = 0.f;
	m_vOwnerRotationDeltaSpeed = _float3(0.f, 0.f, 0.f);
	m_vTargetRotationDeltaSpeed = _float3(0.f, 0.f, 0.f);
}

void CCutSceneModul::MoveTo_Char(const _double& _dTimeDelta)
{
	if (nullptr == m_pCutScene_OwnerModelCom || nullptr == m_pCutScene_TargetModelCom)
		return;

	if (m_CutCharDataVector.size() > m_iCutChar_Index)
	{
		if (false == m_bRootBone_Owner)
		{
			if (true == m_CutCharDataVector[m_iCutChar_Index].bOwnerMove)
			{
				m_pOwnerCharacter->Get_Transform()->Translate_Pos(Coord_Position(m_CutCharDataVector[m_iCutChar_Index].vOwnerPosition)
					, _dTimeDelta, m_CutCharDataVector[m_iCutChar_Index].fOwnerSpeed + m_fOwnerDeltaSpeed, 0.5f);
			}
		}
		else
		{
			m_pCutScene_OwnerModelCom->Get_CutSceneAnimDesc()->RootBoneTransform(m_StartWorldMatrix);
		}
		if (false == m_bRootBone_Target)
		{
			if (true == m_CutCharDataVector[m_iCutChar_Index].bTargetMove)
			{
				m_pTargetCharacter->Get_Transform()->Translate_Pos(Coord_Position(m_CutCharDataVector[m_iCutChar_Index].vTargetPosition),
					_dTimeDelta, m_CutCharDataVector[m_iCutChar_Index].fTargetSpeed + m_fTargetDeltaSpeed, 0.5f);
			}
		}
		else
		{
			m_pCutScene_TargetModelCom->Get_CutSceneAnimDesc()->RootBoneTransform(m_StartWorldMatrix);
		}
	}
}

void CCutSceneModul::RotateTo_Char(const _double& _dTimeDelta)
{
	if (nullptr == m_pOwnerCharacter || nullptr == m_pTargetCharacter)
		return;
	if (m_CutCharDataVector.size() > m_iCutChar_Index)
	{
		if (false == m_bRootBone_Owner)
		{
			if (true == m_CutCharDataVector[m_iCutChar_Index].bOwnerRotationUse)
			{
				OUTMATRIX OutMatrix = CMethod::Out_Matrix(m_pOwnerCharacter->Get_Transform()->Get_WorldMatrix());
				_float3 vRotation = (m_CutCharDataVector[m_iCutChar_Index].vOwnerRotationSpeed + m_vOwnerRotationDeltaSpeed)
					* (_float)_dTimeDelta;
				m_pOwnerCharacter->Get_Transform()->Rotate_Turn(vRotation + OutMatrix.vRot);
			}
		}
		if (false == m_bRootBone_Target)
		{
			if (true == m_CutCharDataVector[m_iCutChar_Index].bTargetRotationUse)
			{
				OUTMATRIX OutMatrix = CMethod::Out_Matrix(m_pTargetCharacter->Get_Transform()->Get_WorldMatrix());
				_float3 vRotation = (m_CutCharDataVector[m_iCutChar_Index].vTargetRotationSpeed + m_vTargetRotationDeltaSpeed) * (_float)_dTimeDelta;
				m_pTargetCharacter->Get_Transform()->Rotate_Turn(vRotation + OutMatrix.vRot);
			}
		}
	}
}

void CCutSceneModul::JumpTo_Target(const _double& _dTimeDelta)
{
	if (nullptr == m_pCutScene_TargetModelCom)
		return;

	if (m_CutCharDataVector.size() > m_iCutChar_Index)
	{
		CTransform* pTransform = m_pCutScene_TargetModelCom->Get_Transform();
		if (nullptr == pTransform)
			return;

		if (false == m_CutCharDataVector[m_iCutChar_Index].bNotResetJumpTarget)
		{
			if (true == m_CutCharDataVector[m_iCutChar_Index].bJump)
			{
				if (m_iJump_CheckCharIndex != m_iCutChar_Index)
				{
					m_dFallTimeAcc = 0.0;
					m_bPass = true;
					m_iJump_CheckCharIndex = m_iCutChar_Index;
				}
			}
		}

		if (pTransform->Get_PosF3().y < 0.f)
		{
			m_dFallTimeAcc = 0.0;
			m_bPass = false;
		}
		else
		{
			if (true == m_bPass)
			{
				_float3 vNormalize = m_CutCharDataVector[m_iCutChar_Index].vAttackDir.Normalize();
				_vector v3Pos = Compute_MovePos_ByDist(_dTimeDelta, vNormalize.Get_Vector(), m_CutCharDataVector[m_iCutChar_Index].fDist,
					m_CutCharDataVector[m_iCutChar_Index].fMaxHeight, m_CutCharDataVector[m_iCutChar_Index].fMaxHeightRatio, m_CutCharDataVector[m_iCutChar_Index].fPlayTime);

				m_pCutScene_TargetModelCom->Get_Transform()->Set_Pos(v3Pos + pTransform->Get_Pos());
				m_dFallTimeAcc += (_double)(_dTimeDelta) *m_CutCharDataVector[m_iCutChar_Index].fJumpDelayTime;
			}
		}
	}
}

_vector CCutSceneModul::Compute_MovePos(const _double& _dTimeDelta, _vector vMoveDir, _float fMoveSpeed, _float fMaxHeight, _float fMaxHeightRatio, _float fPlayTime)
{
	// 이동속도에 따른 Y를 제외한 이동값을 구한다.
	_vector vMovePos = XMVector3Normalize(vMoveDir) * fMoveSpeed * _float(_dTimeDelta);
	vMovePos = XMVectorSetY(vMovePos, 0.f);

	_float fGravity = 0.f;
	_float fHeight = 0.f;

	// 총 재생시간 * 최대 높이 도달하는 비율값이 0일때의 중력적용값
	if (fMaxHeightRatio * (_float)(fPlayTime) == 0.f)
	{
		fGravity = 400.f;
		fHeight = 0.f;
	}
	else
	{
		fGravity = 2 * fMaxHeight / pow(fMaxHeightRatio * fPlayTime, 2.f);
		fHeight = fGravity * (_float)(fPlayTime)*fMaxHeightRatio;
	}
	// FallTimeAcc값을 갖고 프레임당 Y값을 생성한다.
	_float fY = (fHeight - fGravity * (_float)m_dFallTimeAcc) * _float(_dTimeDelta);
	vMovePos = XMVectorSetY(vMovePos, fY);

	return vMovePos;
}

_vector CCutSceneModul::Compute_MovePos_ByDist(const _double& _dTimeDelta, _vector vMoveDir, _float fDist, _float fMaxHeight, _float fMaxHeightRatio, _float fPlayTime)
{
	// 최대 이동할 거리를 이용하여 총 플레이타임과의 계산을 통해 프레임당 이동거리를 계산한다.
	if (0.f == fPlayTime)
		return XMVectorZero();

	// 거리에 따른 프레임당 이동거리
	_vector vMovePos = XMVector3Normalize(vMoveDir) * (fDist / fPlayTime) * _float(_dTimeDelta);
	vMovePos = XMVectorSetY(vMovePos, 0.f);

	_float fGravity = 0.f;
	_float fHeight = 0.f;

	// 총 재생시간 * 최대 높이 도달하는 비율값이 0일때의 중력적용값
	if (fMaxHeightRatio * (_float)(fPlayTime)*fMaxHeight == 0.f)
	{
		fGravity = 400.f;
		fHeight = 0.f;
	}
	else
	{
		fGravity = 2 * fMaxHeight / pow(fMaxHeightRatio * fPlayTime, 2.f);
		fHeight = fGravity * (_float)(fPlayTime)*fMaxHeightRatio;
	}
	// FallTimeAcc값을 갖고 프레임당 Y값을 생성한다.
	_float fY = (fHeight - fGravity * (_float)m_dFallTimeAcc) * _float(_dTimeDelta);
	vMovePos = XMVectorSetY(vMovePos, fY);

	m_dFallTimeAcc += _dTimeDelta;

	return vMovePos;
}

_float3 CCutSceneModul::Rotation_Convert(_float3 _vRot)
{
	_vRot.x = XMConvertToRadians(_vRot.x);
	_vRot.y = XMConvertToRadians(_vRot.y);
	_vRot.z = XMConvertToRadians(_vRot.z);
	_matrix CamRotationMatrix =
		XMMatrixRotationRollPitchYawFromVector(_vRot.Get_Vector()) * CMethod::XMMatrixLoadFloat4x4_P(m_RotationWorldMatrix);

	OUTMATRIX CamOutMatrix = CMethod::Out_Matrix(CamRotationMatrix);

	if (CamOutMatrix.vRot.y >= 180.f)
		CamOutMatrix.vRot.y -= 360.f;
	else if (CamOutMatrix.vRot.y <= -180.f)
		CamOutMatrix.vRot.y += 360.f;

	return CamOutMatrix.vRot;
}

_float3 CCutSceneModul::Coord_PositionF3(_float3 _vPos)
{
	_matrix PosMatrix =
		XMMatrixTranslationFromVector(_vPos.Get_Vector()) * CMethod::XMMatrixLoadFloat4x4_P(m_StartWorldMatrix);
	return PosMatrix.r[3];
}

_vector CCutSceneModul::Coord_Position(_float3 _vPos)
{
	_matrix PosMatrix =
		XMMatrixTranslationFromVector(_vPos.Get_Vector()) * CMethod::XMMatrixLoadFloat4x4_P(m_StartWorldMatrix);
	return PosMatrix.r[3];
}

void CCutSceneModul::Reset_Cam()
{
	if (m_CutSceneDataVector.size() <= 0)
		return;
	m_fLerpAddValue = 0.f;
	m_fMoveSplineValue = 0.f;
	m_fMoveBezierValue = 0.f;
	m_dFallTimeAcc = 0.0;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		pGameInstance->Set_Current_FrameValue(60);
	}
	RELEASE_INSTANCE(CGameInstance);
	m_iCutScene_Index = 0;
	m_fCurrentCamTime = 0.f;
	m_iCutScene_Texture_Index = 0;
	m_iCutSceneTextureIndex = 0;
	m_fCamDeltaSpeed = 0.f;
	m_bPlayCutScene = false;

	if (false == m_bNotChangeCutScene)
	{
		if (nullptr != m_pCamCutScene)
			m_pCamCutScene->Set_Active(false);

		CCamera_Manager* pManager = GET_INSTANCE(CCamera_Manager)
		{
			pManager->Set_ActiveCamera(PROTO_GOB_CAMERA_FREE);
		}
		RELEASE_INSTANCE(CCamera_Manager);

		if (nullptr != m_pCameraFree)
		{
			//	if (true == m_bLastBindCameraFree)
			{
				CTransform* pCamCutTransform = m_pCamCutScene->Get_Transform();
				if (nullptr != pCamCutTransform)
				{
					CTransform* pCamFree = m_pCameraFree->Get_Transform();
					if (nullptr != pCamFree)
					{
						OUTMATRIX OutMatrix = CMethod::Out_Matrix(pCamCutTransform->Get_WorldMatrix());
						pCamFree->Set_Pos(OutMatrix.vPos.Get_Vector());
						pCamFree->Rotate_Turn(OutMatrix.vRot.Get_Vector());
					}
				}
			}
		}
	}
	else
	{
		m_bNotChangeCutScene = false;
	}
}

void CCutSceneModul::Reset_Char()
{
	if (m_CutCharDataVector.size() <= 0)
		return;
	m_iCutChar_Index = 0;
	m_fCurrentCharTime = 0.f;
	m_iJump_CheckCharIndex = -1;
	m_fOwnerDeltaSpeed = 0.f;
	m_fTargetDeltaSpeed = 0.f;
	m_vOwnerRotationDeltaSpeed = _float3(0.f, 0.f, 0.f);
	m_vTargetRotationDeltaSpeed = _float3(0.f, 0.f, 0.f);
	m_bPlayCutChar = false;
	m_bJumpEnd = false;
	m_bPass = false;
	m_dFallTimeAcc = 0.0;
	m_iCutDmg_Index = 0;

	if (nullptr != m_pCutScene_OwnerModelCom)
	{
		m_pCutScene_OwnerModelCom->Get_CutSceneAnimDesc()->Reset();
		m_pOwnerCharacter->Get_Transform()->Update_Nav(m_pOwnerNavigation);
		if (L"Zenitsu" == m_strName)
		{
			if ("ZENITSU_ATKSK" == m_strInputTrigger)
			{
				m_pOwnerCharacter->Get_Transform()->Rotate_TurnToNotFix(_float3(0.f, 180.f, 0.f));
			}
		}
	}

	if (nullptr != m_pCutScene_OwnerModelCom)
	{
		m_pCutScene_TargetModelCom->Get_CutSceneAnimDesc()->Reset();
		m_pTargetCharacter->Get_Transform()->Update_Nav(m_pTargetNavigation);
		m_pTargetCharacter->Get_Transform()->Update_Navigation_Cell(m_pTargetCharacter->Get_Transform()->Get_Pos());
	}
}

CCutSceneModul* CCutSceneModul::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, VOIDDATAS* _pDatas)
{
	CCutSceneModul* pInstance = new CCutSceneModul(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct(_pDatas)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CCutSceneModul::Create To Failed");
#endif
	}
	return pInstance;
}

void CCutSceneModul::Free()
{
	Safe_Release_DV(m_pDevice);
	Safe_Release_DV(m_pContext);

	Safe_Release(m_pCutScene_OwnerModelCom);
	Safe_Release(m_pCutScene_TargetModelCom);

	Safe_Release(m_pTexture);
	Safe_Release(m_VIBuffer_CutSceneTexture);
	Safe_Release(m_pCutScene_Shader);

	for (auto& iter : m_CutSceneTexVector)
		Safe_Release(iter);
}