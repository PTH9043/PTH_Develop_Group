#include "Camera_Cinematic.h"
#include "GameInstance.h"
#include "ImGuiClass_Manager.h"

#include "Camera_Manager.h"
#include "Level_Story.h"
#include "Player.h"

CCamera_Cinematic::CCamera_Cinematic(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CCamera(_pDevice, _pContext)
{
}

CCamera_Cinematic::CCamera_Cinematic(const CCamera_Cinematic& _rhs)
    : CCamera(_rhs)
{
}


HRESULT CCamera_Cinematic::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCamera_Cinematic::NativeConstruct_Clone(void* _pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(_pArg)))
        return E_FAIL;

	m_bActive = false;
	m_iCurIndex = 0;
	m_bInitCameraSetting = false;

    return S_OK;
}

void CCamera_Cinematic::Tick(const _double& _dTimeDelta)
{
	if (false == m_bActive)
		return;

    __super::Tick(_dTimeDelta);
}

void CCamera_Cinematic::Late_Tick(const _double& _dTimeDelta)
{
	if (false == m_bActive)
		return;

    __super::Late_Tick(_dTimeDelta);

	Move_Camera_Position(_dTimeDelta);
}

HRESULT CCamera_Cinematic::Render()
{
    return S_OK;
}

void CCamera_Cinematic::Extern_ImGuiClass()
{

}

void CCamera_Cinematic::Move_Camera_Position(const _double& _dTimeDelta)
{
	if (m_CinematicDatas.size() <= 1)
		return;

	if (false == m_bInitCameraSetting)
	{
		m_pTransformCom->Set_Pos(m_CinematicDatas[m_iCurIndex].vCamPos.Get_Vector());
		m_bInitCameraSetting = true;
		m_fCamLerpSpeed = 0.f;
	}
	else
	{
		_uint iStartIndex = m_iCurIndex;
		_uint iMoveIndex = m_iCurIndex + 1;
		if (iMoveIndex < m_CinematicDatas.size())
		{
			_float3 v3Length = m_CinematicDatas[iMoveIndex].vCamPos - m_pTransformCom->Get_PosF3();
			_float fLength = v3Length.Length();
			_float3 v3TotalLength = m_CinematicDatas[iMoveIndex].vCamPos - m_CinematicDatas[iStartIndex].vCamPos;
			_float fTotalLength = v3TotalLength.Length();

			m_pTransformCom->Translate_Pos(m_CinematicDatas[iMoveIndex].vCamPos.Get_Vector(),
				_dTimeDelta, m_CinematicDatas[iStartIndex].fSpeed * 3.f);

			_vector vValue = XMQuaternionSlerp(m_CinematicDatas[iStartIndex].vRotValue.Get_Vector(), m_CinematicDatas[iMoveIndex].vRotValue.Get_Vector(),
				m_fCamLerpSpeed);
			m_vSlerpRotation = vValue;
			XMMATRIX matWorld = XMMatrixRotationQuaternion(vValue);
			matWorld.r[3] = m_pTransformCom->Get_Pos();
			m_pTransformCom->Bind_WorldMatrix(matWorld);

			if (0 != m_CinematicDatas[iStartIndex].fSpeed)
				m_fCamLerpSpeed += (_float)(_dTimeDelta)*m_CinematicDatas[iStartIndex].fSpeed * 0.1f;

			if (m_fCamLerpSpeed >= 1.f)
				m_fCamLerpSpeed = 1.f;

			if (fLength <= 1.f)
			{
				++m_iCurIndex;
				if (iMoveIndex <= m_CinematicDatas.size())
				{
					_vector vValue = m_CinematicDatas[iMoveIndex].vRotValue.Get_Vector();
					//XMQuaternionSlerp(m_vSlerpRotation.Get_Vector(), 
					//m_CinematicDatas[iMoveIndex].vRotValue.Get_Vector(),0.95f);
					XMMATRIX matWorld = XMMatrixRotationQuaternion(vValue);
					m_vSlerpRotation = vValue;
					matWorld.r[3] = m_pTransformCom->Get_Pos();
					m_pTransformCom->Bind_WorldMatrix(matWorld);

					m_fCamLerpSpeed = 0.f;
				}
			}
		}
		else
		{
			CCamera_Manager::Get_Instance()->Set_ActiveCamera(CM_CAMERA_FREE);
		}
	}
}

void CCamera_Cinematic::Load_Data(const _wstring& _strName)
{
    _wstring Path = L"..\\..\\Resources\\Data\\Camera_Cinematic\\";
    Path.append(_strName.c_str());
    Path.append(L".bin");
	//m_strCamName.append(_strName.c_str());

    m_CinematicDatas.clear();
    HANDLE hFile = CMethod::Begin_Load(Path.c_str());
    if (INVALID_HANDLE_VALUE == hFile)
        return;
    DWORD dwByte = 0;
    _uint Value = 0;
    ReadFile(hFile, &Value, sizeof(Value), &dwByte, nullptr);
    for (_uint t = 0; t < Value; ++t)
    {
        CINEMATIC_CAMDATA tData;
        ReadFile(hFile, &tData, sizeof(tData), &dwByte, nullptr);
        m_CinematicDatas.push_back(tData);
    }

    CloseHandle(hFile);
}

CCamera_Cinematic* CCamera_Cinematic::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCamera_Cinematic* pInstance = new CCamera_Cinematic(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CCamera_Cinematic::Create To Failed");
#endif
    }
    return pInstance;
}

CGameObject* CCamera_Cinematic::Clone(void* pArg)
{
    CCamera_Cinematic* pInstance = new CCamera_Cinematic(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CCamera_Cinematic::Create To Failed");
#endif
    }
    return pInstance;
}

void CCamera_Cinematic::Free()
{
    __super::Free();
}
