#include "Camera_Cinematic_Tool.h"
#include "GameInstance.h"

CCamera_Cinematic_Tool::CCamera_Cinematic_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCamera(_pDevice, _pContext)
{
}

CCamera_Cinematic_Tool::CCamera_Cinematic_Tool(const CCamera_Cinematic_Tool& _rhs)
	: CCamera(_rhs)
{
}

void CCamera_Cinematic_Tool::Set_CinematicData(const _uint& _iIndex, CINEMATIC_CAMDATA*  _pData)
{
	if (nullptr == _pData)
		return;

	if (m_CinematicDatas.size() > _iIndex)
	{
		m_CinematicDatas[_iIndex] = *_pData;
	}
}

HRESULT CCamera_Cinematic_Tool::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Cinematic_Tool::NativeConstruct_Clone(void* _pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(_pArg)))
		return E_FAIL;

	m_bActive = false;
	return S_OK;
}

void CCamera_Cinematic_Tool::Tick(const _double& _dTimeDelta)
{
	if (false == m_bActive)
		return;

	Find_Camera();
	__super::Tick(_dTimeDelta);
}

void CCamera_Cinematic_Tool::Late_Tick(const _double& _dTimeDelta)
{
	if (false == m_bActive)
		return;

	__super::Late_Tick(_dTimeDelta);

	Move_Camera_Position(_dTimeDelta);
}

HRESULT CCamera_Cinematic_Tool::Render()
{
	_float3 v3Size(1.f, 1.f, 1.f);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		for (auto& iter : m_CinematicDatas)
		{
			Show_BoundingBox(v3Size, iter.vCamPos, pGameInstance);
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CCamera_Cinematic_Tool::Play_CameraMove()
{
	if (m_CinematicDatas.size() <= 1)
		return;

	m_bActive = true;
	m_iCurIndex = 0;
	m_bInitCameraSetting = false;

	m_pTransformCom->Set_Pos(m_CinematicDatas[m_iCurIndex].vCamPos.Get_Vector());
	_float3 v3Value = m_CinematicDatas[m_iCurIndex].vRotValue.Get_Float3();

	XMMATRIX matWorld = XMMatrixRotationQuaternion(m_CinematicDatas[m_iCurIndex].vRotValue.Get_Vector());
	m_vSlerpRotation = m_CinematicDatas[m_iCurIndex].vRotValue;
}

void CCamera_Cinematic_Tool::Stop_CameraMove()
{
	if (m_AssignCamList.size() > 0)
		m_AssignCamList.front()->Set_Active(true);
	m_bActive = false;
}

void CCamera_Cinematic_Tool::Insert_Data(const CINEMATIC_CAMDATA& _tData)
{
	m_CinematicDatas.push_back(_tData);
}

void CCamera_Cinematic_Tool::Save_Data(const _wstring& _strFolderName, const _wstring& _strName)
{
	_wstring Path = L"..\\..\\Resources\\Data\\";
	Path.append(_strFolderName);
	Path.append(L"\\");
	_wmkdir(Path.c_str());

	Path.append(_strName.c_str());
	Path.append(L".bin");

	HANDLE hFile = CMethod::Begin_Save(Path.c_str());
	if (INVALID_HANDLE_VALUE == hFile)
		return;
	DWORD dwByte = 0;
	_uint iLength = (_uint)m_CinematicDatas.size();
	WriteFile(hFile, &iLength, sizeof(iLength), &dwByte, nullptr);
	for (auto& iter : m_CinematicDatas)
	{
		WriteFile(hFile, &iter, sizeof(iter), &dwByte, nullptr);
	}

	CloseHandle(hFile);
}

void CCamera_Cinematic_Tool::Load_Data(const _wstring& _strFolderName, const _wstring& _strName)
{
	_wstring Path = L"..\\..\\Resources\\Data\\";
	Path.append(_strFolderName);
	Path.append(L"\\");
	Path.append(_strName.c_str());
	Path.append(L".bin");

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

void CCamera_Cinematic_Tool::Show_BoundingBox(const _float3& _v3Size, const _float3& _v3Pos, CGameInstance* _pGameInstance)
{
#ifdef _DEBUG
	BoundingBox bBox1 = BoundingBox(_v3Pos, _v3Size);
	_pGameInstance->Draw(bBox1, CDrawing::VIEW_3D);
#endif // _DEBUG
}

void CCamera_Cinematic_Tool::Move_Camera_Position(const _double& _dTimeDelta)
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
				m_fCamLerpSpeed += (_float)(_dTimeDelta) * m_CinematicDatas[iStartIndex].fSpeed * 0.1f;

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
			if (m_AssignCamList.size() > 0)
				m_AssignCamList.front()->Set_Active(true);
			m_bActive = false;
		}
	}
}

void CCamera_Cinematic_Tool::Find_Camera()
{
	if (m_AssignCamList.size() == 0)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		{
			list<CGameObject*> CamList = pGameInstance->Find_GameObjectList(LEVEL_TOOL, LAYER_CAM);
			m_AssignCamList.reserve(CamList.size());
			for (auto& iter : CamList)
			{
				if (nullptr != this)
				{
					m_AssignCamList.push_back(static_cast<CCamera*>(iter));
					iter->Set_Active(false);
				}
			}
		}
		RELEASE_INSTANCE(CGameInstance);
	}
}

CCamera_Cinematic_Tool* CCamera_Cinematic_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCamera_Cinematic_Tool* pInstance = new CCamera_Cinematic_Tool(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CCinematic_Camera_Tool::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* CCamera_Cinematic_Tool::Clone(void* pArg)
{
	CCamera_Cinematic_Tool* pInstance = new CCamera_Cinematic_Tool(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CCinematic_Camera_Tool::Clone To Failed");
#endif
	}
	return pInstance;
}

void CCamera_Cinematic_Tool::Free()
{
	__super::Free();
}
