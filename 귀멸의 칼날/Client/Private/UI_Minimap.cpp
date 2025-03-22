#include "UI_Minimap.h"
#include "GameInstance.h"
#include "Zenitsu.h"
#include "Minimap_Track.h"
#include "Camera_Free.h"

CUI_Minimap::CUI_Minimap(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CUI_Base(_pDevice, _pContext)
{
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

CUI_Minimap::CUI_Minimap(const CUI_Minimap& _rhs)
	: CUI_Base(_rhs)
{
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

HRESULT CUI_Minimap::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CUI_Minimap::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	FileInfo* fileInfo = static_cast<FileInfo*>(pArg);


	if (pArg != nullptr && fileInfo != nullptr)
	{
		m_fX = fileInfo->m_fX;
		m_fY = fileInfo->m_fY;
		m_fSizeX = fileInfo->m_fSizeX;
		m_fSizeY = fileInfo->m_fSizeY;
		m_Depth = fileInfo->m_Depth;
		lstrcpy(m_pName, fileInfo->m_pName);
		lstrcpy(m_TextureName, fileInfo->m_TextureName);
		lstrcpy(m_PrototypeTag, fileInfo->m_PrototypeTag);

		m_State = fileInfo->m_State;
		m_Speed = fileInfo->m_Speed;
		m_MaxSize = fileInfo->m_Max;
		m_MinSize = fileInfo->m_Min;
		m_MoveX = fileInfo->m_MoveX;
		m_MoveY = fileInfo->m_MoveX;
		m_CreatTime = fileInfo->m_CreatTime;
		m_DeadTime = fileInfo->m_DeadTime;

		m_fAlpha = fileInfo->m_Alpha;
		m_fAlphaCreat = fileInfo->m_AlphaCreat;
		m_fAlphaEnd = fileInfo->m_AlphaDead;
		m_AlphaState = fileInfo->m_AlphaState;
	}
	else
	{
		m_fX = 100.f;
		m_fY = 100.f;
		m_fSizeX = 300.f;
		m_fSizeY = 300.f;
		m_Depth = 1;
	}

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CPlayer* Player = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_STORY, LAYER_TESTPLAYER, 0));
	CTransform* PlayerTransform = Player->Get_Transform();
	_vector vPlayerPos = PlayerTransform->Get_Pos();
	m_PrevPos = vPlayerPos;
	Safe_Release(pGameInstance);

	return S_OK;
}

void CUI_Minimap::Tick(const _double& _dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	__super::Tick(_dTimeDelta);
	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_Pos(XMVectorSet(m_fX, m_fY, 0.0f, 1.f));


	//카메라
	//CCamera_Free* Player = dynamic_cast<CCamera_Free*>(pGameInstance->Find_GameObject(LEVEL_STORY, LAYER_CAM, 0));

	//플레이어
	CPlayer* Player = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_STORY, LAYER_TESTPLAYER, 0));
	CTransform* PlayerTransform = Player->Get_Transform();
	_vector vPlayerPos = PlayerTransform->Get_Pos();

	m_MoveDist = vPlayerPos - XMLoadFloat4(&m_PrevPos);

	m_PrevPos = vPlayerPos;

	m_TickFrame += (_float)_dTimeDelta;

	Map_Move();
	Map_Track();
	Turn_PlayerIcon();

	Safe_Release(pGameInstance);
}

void CUI_Minimap::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}
HRESULT CUI_Minimap::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (Is_MyName(L"Mini_Map") == true)
		m_pShaderCom->Begin(1); //8

	else
		m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CUI_Minimap::Extern_ImGuiClass()
{
	ImGui::DragFloat("m_MapX", &m_MapX, 0.01f, -500.f, 500.f);
	ImGui::DragFloat("m_MapY" ,&m_MapY, 0.01f, -500.f, 500.f);

	ImGui::DragFloat("m_FX", &m_fX, 0.01f, -500.f, 900.f);
	ImGui::DragFloat("m_FY", &m_fY, 0.01f, -500.f, 900.f);

	ImGui::DragFloat("m_MoveRatio", &m_MoveRatio, 0.01f, -0.001f, 900.f);

}

void CUI_Minimap::Map_Move()  //캐릭터 원래대로 방향키 수정하고 코드 수정하기 
{
	if (Is_MyName(L"Mini_Map") == true)
	{
		////미니맵의 중심(오른쪽 위의 작은거)
		///*_float baseX = 498.4f + 123.f;
		//_float baseY = 223.6f + 423.f;*/

		//_float baseX = 498.4f;
		//_float baseY = 223.6f;


		//// 미니맵 텍스쳐에서 중심이 되는 UV좌표
		//// 유저가 앞뒤방향키를 누르면 실제 플레이어의 z좌표가 바뀌어야 하는데 x좌표가바뀜(왜지)
		//// 좌우도 똑같음, 그래서 m_MapX에 z좌표를 넣어줌, m_MapY도 같은 이유로 x좌표를 넣음
		//m_MapX = m_MovePlayerX / m_fSizeX;
		//m_MapY = m_MovePlayerZ/ m_fSizeY;


		//m_MapY = 1.f - m_MapY;
		//// 플레이어가 앞으로 가면 미니맵(지도)에서 봤을 때 위로 올라감
		//// 앞으로 간다는건 z값이 증가하는 것이고 미니맵에서 올라간다는건 UV의y좌표가 감소하는 것
		//// 반대의 경우, 뒤로 가면 z가 감소, UV의y가 증가
		//// 그래서 플레이어의 위치란 UV좌표랑 반대로 왔다갔다 해주려고 m_MapY = 1.f - m_MapY;이걸 써줌
		//// m_MapY의 값이 올라갈수록 1.f - m_MapY의 결과는 내려감

		////시작위치로 맞추기 위해 미니맵의 중점을 이동시킴 
		////등나무 맵이 중앙인 위치값
		////m_MapX += 0.623f;
		////m_MapY += (0.923f + -1.f);

		////산입구가 중앙인 위치값
		//m_MapX += 1.0244f;
		//m_MapY += 0.1383f;



		//m_fX = (0.5f - m_MapX) * m_fSizeX + 498.4f;
		//m_fY = (m_MapY - 0.5f) * m_fSizeY + baseY;


		//// 미니맵을 움직여서 미니맵 텍스쳐의 잘려진 부분이 가운데로 오게 한 다음
		//// 오른쪽 위의 미니맵의 좌표를 더해서 옮겨줌

		//==============새로운 방식 
		//m_MapX += (m_MoveDist.x * m_MoveRatio);
		//m_MapY -= (m_MoveDist.z * m_MoveRatio);

		//m_fX -= (m_MoveDist.x * m_MoveRatio) * m_fSizeX;
		//m_fY -= (m_MoveDist.z * m_MoveRatio) * m_fSizeY;

		//m_fX += 212.8f; //중앙 위치를 맞추기 위해
		//m_fY += 585.71f;

		//========다시 수정 

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		
		CPlayer* Player = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_STORY, LAYER_TESTPLAYER, 0));
		CTransform* PlayerTransform = Player->Get_Transform();
		_float3 fPlayerPos = PlayerTransform->Get_PosF3();
		
		m_fX = (fPlayerPos.x * -1.f) * m_MoveRatio;
		
		m_fY = fPlayerPos.z * m_MoveRatio;

			Safe_Release(pGameInstance);
	}

}

void CUI_Minimap::Map_Track()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	if (Is_MyName(L"Mini_Map") == true)
	{
		if (m_TickFrame >= 5.0f)
		{
			m_TickFrame = 0.f;

			//CMinimap_Track* obj = dynamic_cast<CMinimap_Track*>(pGameInstance->Clone_GameObject_Add(LEVEL_GAMEPLAY, LAYER_UI, PROTO_GOB_UI_MINITRACK));
			//if (nullptr != obj)
				//obj->SetWorldPos(XMVectorSet(m_MovePlayerX, 0.f, m_MovePlayerZ, 1.f));
		}


	}


	Safe_Release(pGameInstance);
}

_float2 CUI_Minimap::Make_UIPos_Form_WorldPos(_vector pos)
{
	if (Is_MyName(L"Mini_Map") == true)
	{
		_float2 fpos = { XMVectorGetX(pos), XMVectorGetZ(pos) };
		fpos.x /= m_fSizeX;
		fpos.y /= m_fSizeY;


		fpos.y = 1.f - fpos.y;
		//fpos.x += 0.623f;
		//fpos.y += (0.923f + -1.f);

		fpos.x += 1.0244f;
		fpos.y += 0.1383f;

		fpos.x -= m_MapX;
		fpos.y -= m_MapY;

		fpos.x *= m_fSizeX;
		fpos.y *= m_fSizeY;

		fpos.x += m_fX;
		fpos.y += m_fY;

		return fpos;
	}

	return _float2{ 0,0 };
}

void CUI_Minimap::Turn_PlayerIcon()
{


	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCamera_Free* camera = dynamic_cast<CCamera_Free*>(pGameInstance->Find_GameObject(LEVEL_STORY, LAYER_CAM, 0));
	_vector cameralook = camera->Get_Transform()->Get_Look();

	_float3 fcameralook;

	XMStoreFloat3(&fcameralook, cameralook);

	_float angle = XMConvertToDegrees(atan2f(fcameralook.x, fcameralook.z));

	if (Is_MyName(L"Mini_Player") == true)
	{
		m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 0.f, 1.f, 0.f), -angle);
	}

	Safe_Release(pGameInstance);
}

HRESULT CUI_Minimap::SetUp_Components()
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

HRESULT CUI_Minimap::SetUp_ConstantTable()
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

	/*if (FAILED(m_pShaderCom->Set_RawValue("g_MapX", &m_MapX, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_MapY", &m_MapY, sizeof(_float))))
		return E_FAIL;*/

	if (FAILED(m_pTextureCom->SetUp_ConstantTable_FileName(m_pShaderCom, "g_DiffuseTexture", m_TextureName)))
		return E_FAIL;


	return S_OK;
}

CUI_Minimap* CUI_Minimap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Minimap* pInstance = new CUI_Minimap(pDevice, pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Minimap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI_Base* CUI_Minimap::Clone(void* pArg)
{
	CUI_Minimap* pInstance = new CUI_Minimap(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Minimap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Minimap::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
