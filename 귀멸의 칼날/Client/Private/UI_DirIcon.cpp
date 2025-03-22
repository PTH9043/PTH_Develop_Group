#include "UI_DirIcon.h"
#include "GameInstance.h"
#include "Camera_Free.h"
#include "NPC.h"
#include "Level_Story.h"
#include "Battle_Manager.h"

CUI_DirIcon::CUI_DirIcon(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CUI_Base(_pDevice, _pContext)
{
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

CUI_DirIcon::CUI_DirIcon(const CUI_DirIcon& _rhs)
    : CUI_Base(_rhs)
{
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

HRESULT CUI_DirIcon::NativeConstruct_Prototype()
{
    return S_OK;
}

HRESULT CUI_DirIcon::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;

    if (pArg != nullptr)
    {
        _int number = *(_int*)pArg;
        wsprintf(m_pName, TEXT("Icon_Oni%d"), number);
        m_fX = 0.f;
        m_fY = 307.f;
        m_fSizeX = 28.f;
        m_fSizeY = 28.f;
        m_Depth = 1;
        Set_Texture(L"Icon_Oni");
    }
    if (FAILED(SetUp_Components()))
        return E_FAIL;

    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

    return S_OK;
}

void CUI_DirIcon::Tick(const _double& _dTimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    __super::Tick(_dTimeDelta);
    m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
    m_pTransformCom->Set_Pos(XMVectorSet(m_fX, m_fY, 0.0f, 1.f));


	Dir_Oni();

    Safe_Release(pGameInstance);
}

void CUI_DirIcon::Late_Tick(const _double& _dTimeDelta)
{

    if (nullptr == m_pRendererCom)
        return;

    m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_DirIcon::Render()
{
    if (nullptr == m_pVIBufferCom)
        return E_FAIL;

    if (FAILED(SetUp_ConstantTable()))
        return E_FAIL;

    m_pShaderCom->Begin(1);

    m_pVIBufferCom->Render();

    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

  /*  if (Is_MyName(L"Icon_NPC") == true)
    {
        if (m_fAlpha == 1.f)
        {
            wsprintf(m_szFONTS, TEXT("%dm"), (_int)fNPCLengrh.x);
            pGameInstance->Render_Font(TEXT("Font_Maru10"), m_szFONTS, _float2(m_fX + 625.f, 65.f), XMVectorSet(255.f, 255.f, 255.f, 1.f));
        }
    }*/
	
    if (Is_MyName(L"Icon_Oni1") == true)
    {
        if (m_fAlpha == 1.f && (_int)OniLengrhlist[0].x >117.f)
        {
            wsprintf(m_szFONTS, TEXT("%dm"), (_int)OniLengrhlist[0].x);
            pGameInstance->Render_Font(TEXT("Font_Maru10"), m_szFONTS, _float2(m_fX + 625.f, 65.f), XMVectorSet(255.f, 255.f, 255.f, 1.f));
        }
    }

    Safe_Release(pGameInstance);
    return S_OK;
}


void CUI_DirIcon::Dir_Oni()
{
	//if (m_iCurOniLayer == CLevel_Story::m_iOniLayer)
		//return;

    CGameInstance* pGameInstance = CGameInstance::Get_Instance();
    Safe_AddRef(pGameInstance);

    //카메라
    CCamera_Free* camera = dynamic_cast<CCamera_Free*>(pGameInstance->Find_GameObject(LEVEL_STORY, LAYER_CAM, 0));
    CTransform* cameraTransform = camera->Get_Transform();
    //카메라의 위치
    _float3 fCameraPos = cameraTransform->Get_PosF3();

    vector <_float3> OniPoslist; //위치 리스트
    vector <_float> LRlist;//왼쪽 오른쪽 구별
    vector <_float> Onithetahlist;//각도 리스트
    vector <_vector> CameraToOni;//카메라에서 뺀 값 리스트

	m_iCurOniLayer = CBattle_Manager::Get_Instance()->Get_CurrentOniLayer();

	if (m_iCurOniLayer == 1) //첫번째 그륩 
	{
		list<CGameObject*> zakolist = pGameInstance->Find_GameObjectList(LEVEL_STORY, LAYER_ONI1);

		for (auto it = zakolist.begin(); it != zakolist.end(); it++)
		{

			CGameObject* pZako = *it;
			CTransform* pZakoTransform = pZako->Get_Transform();
			//오니의 위치
			_float3 fOniPos = pZakoTransform->Get_PosF3();

			OniPoslist.push_back(fOniPos);
		}
	}
	else if (m_iCurOniLayer == 2) //두번째 그륩 
	{
		list<CGameObject*> zakolist = pGameInstance->Find_GameObjectList(LEVEL_STORY, LAYER_ONI2);

		for (auto it = zakolist.begin(); it != zakolist.end(); it++)
		{

			CGameObject* pZako = *it;
			CTransform* pZakoTransform = pZako->Get_Transform();
			//오니의 위치
			_float3 fOniPos = pZakoTransform->Get_PosF3();

			OniPoslist.push_back(fOniPos);
		}
	}

	else if (m_iCurOniLayer == 3) //세번째 그륩 
	{
		list<CGameObject*> zakolist = pGameInstance->Find_GameObjectList(LEVEL_STORY, LAYER_ONI3);

		for (auto it = zakolist.begin(); it != zakolist.end(); it++)
		{

			CGameObject* pZako = *it;
			CTransform* pZakoTransform = pZako->Get_Transform();
			//오니의 위치
			_float3 fOniPos = pZakoTransform->Get_PosF3();

			OniPoslist.push_back(fOniPos);
		}
	}
	
	//카메라의 look벡터 
	_float3 fCameraLook = cameraTransform->Get_LookF3();
	fCameraLook.x *= -1.f;
	fCameraLook.y = -0.f;
	fCameraLook.z *= -1.f;
	_vector vCameraLook = XMLoadFloat3(&fCameraLook);


	for (auto it = OniPoslist.begin(); it != OniPoslist.end(); it++)
	{
		_float3 fCameraToOni = *it - fCameraPos;
		fCameraToOni.y = 0.f;
		_vector vCameraToOni = XMLoadFloat3(&fCameraToOni);
		CameraToOni.push_back(vCameraToOni);
	}

	_float3 fOniLengrh;
	OniLengrhlist.clear();
	for (auto it = CameraToOni.begin(); it != CameraToOni.end(); it++)
	{
		//거리 
		_vector vLength = XMVector3Length(*it);
		XMStoreFloat3(&fOniLengrh, vLength);
		OniLengrhlist.push_back(fOniLengrh);
	}


	for (auto it = CameraToOni.begin(); it != CameraToOni.end(); it++)
	{

		_vector vnCameraLook = XMVector3Normalize(vCameraLook);
		_vector vnCameraToOni = XMVector3Normalize(*it);

		//내적
		_float inner = XMVectorGetX(XMVector3Dot(vnCameraLook, vnCameraToOni));

		//각도를 구해준다 
		_float Onitheta = XMConvertToDegrees(acos(inner));
		Onithetahlist.push_back(Onitheta);
		//================왼쪽 오른쪽 판별
		_vector cross = XMVector3Cross(vnCameraLook, vnCameraToOni);
		_float3 LR;
		XMStoreFloat3(&LR, cross);
		LRlist.push_back(LR.y);
	}

	if (Is_MyName(L"Icon_Oni1") == true)
	{
		if (LRlist.size() <= 0)
		{
			m_fAlpha = 0.f;
			Safe_Release(pGameInstance);
			return;
		}
		
		if (LRlist[0] < 0)
			Onithetahlist[0] *= -1.f;

		if (Onithetahlist[0] <= 45.f && Onithetahlist[0] >= -45.f)
		{
			m_fAlpha = 1.f;
			m_fX = Onithetahlist[0] * 7.f;
		}
		else
			m_fAlpha = 0.f;
	}

	if (Is_MyName(L"Icon_Oni2") == true)
	{
		if (LRlist.size() <= 1)
		{
			m_fAlpha = 0.f;
			Safe_Release(pGameInstance);
			return;
		}
		if (LRlist[1] < 0)
			Onithetahlist[1] *= -1.f;

		if (Onithetahlist[1] <= 45.f && Onithetahlist[1] >= -45.f)
		{
			m_fX = Onithetahlist[1] * 7.f;
			if ((_int)OniLengrhlist[0].x > 117.f)
				m_fAlpha = 0.f;
			else
				m_fAlpha = 1.f;
		}
		else
			m_fAlpha = 0.f;
	}

	if (Is_MyName(L"Icon_Oni3") == true)
	{
		if (LRlist.size() <= 2)
		{
			m_fAlpha = 0.f;
			Safe_Release(pGameInstance);
			return;
		}
		if (LRlist[2] < 0)
			Onithetahlist[2] *= -1.f;

		if (Onithetahlist[2] <= 45.f && Onithetahlist[2] >= -45.f)
		{
			m_fX = Onithetahlist[2] * 7.f;
			if ((_int)OniLengrhlist[0].x > 117.f)
				m_fAlpha = 0.f;
			else
				m_fAlpha = 1.f;
		}
		else
			m_fAlpha = 0.f;
	}

	
	if (Is_MyName(L"Icon_Oni4") == true)
	{
		if (LRlist.size() <= 3)
		{
			m_fAlpha = 0.f;
			Safe_Release(pGameInstance);
			return;
		}
		if (LRlist[3] < 0)
			Onithetahlist[3] *= -1.f;

		if (Onithetahlist[3] <= 45.f && Onithetahlist[3] >= -45.f)
		{
			m_fX = Onithetahlist[3] * 7.f;
			if ((_int)OniLengrhlist[0].x > 117.f)
				m_fAlpha = 0.f;
			else
				m_fAlpha = 1.f;
		}
		else
			m_fAlpha = 0.f;
	}

	if (Is_MyName(L"Icon_Oni5") == true)
	{
		if (LRlist.size() <= 4)
		{
			m_fAlpha = 0.f;
			Safe_Release(pGameInstance);
			return;
		}
		if (LRlist[4] < 0)
			Onithetahlist[4] *= -1.f;

		if (Onithetahlist[4] <= 45.f && Onithetahlist[4] >= -45.f)
		{
			m_fX = Onithetahlist[4] * 7.f;
			if ((_int)OniLengrhlist[0].x > 117.f)
				m_fAlpha = 0.f;
			else
				m_fAlpha = 1.f;
		}
		else
			m_fAlpha = 0.f;
	}

	Safe_Release(pGameInstance);
}


HRESULT CUI_DirIcon::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(COM_TEXTURE, LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Test"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_UI, (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(COM_VIBUFFER, LEVEL_STATIC, PROTO_COMP_VIBUFFER_RECT, (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_DirIcon::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_float4x4		ViewMatrix;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());

	_float4x4 World = m_pTransformCom->Get_WorldFloat4x4_TP();

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &World, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, "g_DiffuseTexture", m_TextureName)))
		return E_FAIL;


	return S_OK;
}

CUI_DirIcon* CUI_DirIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_DirIcon* pInstance = new CUI_DirIcon(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_DirIcon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI_Base* CUI_DirIcon::Clone(void* pArg)
{
	CUI_DirIcon* pInstance = new CUI_DirIcon(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_DirIcon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_DirIcon::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
