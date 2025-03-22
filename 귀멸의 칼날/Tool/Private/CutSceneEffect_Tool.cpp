#include "CutSceneEffect_Tool.h"
#include "GameInstance.h"

CCutSceneEffect_Tool::CCutSceneEffect_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
	Safe_AddRef_DV(_pDevice);
	Safe_AddRef_DV(_pContext);
}

HRESULT CCutSceneEffect_Tool::NativeConstruct(VOIDDATAS* _pDatas)
{
	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);

	if(nullptr != _pDatas)
		m_strEffectName = *CMethod::ConvertTemplate_PopBack<_wstring*>(_pDatas);
	return S_OK;
}

HRESULT CCutSceneEffect_Tool::Render_Effect()
{

	return S_OK;
}

void CCutSceneEffect_Tool::Extern_EffectValue()
{
	OUTMATRIX outMatrix = CMethod::Out_Matrix(m_pTransformCom->Get_WorldFloat4x4());

	_bool bPos = ImGui::DragFloat3("Texture_Position", &outMatrix.vPos.x, 0.5f, -1000.f, 1000.f);
	_bool bRot = ImGui::DragFloat3("Texture_Rotation", &outMatrix.vRot.x, 0.5f, -1000.f, 1000.f);
	_bool bScale = ImGui::DragFloat3("Texture_Scale", &outMatrix.vScale.x, 0.5f, -1000.f, 1000.f);

	if (true == bPos)
		m_pTransformCom->Set_Pos(outMatrix.vPos.Get_Vector());
	if (true == bRot)
		m_pTransformCom->Rotate_Turn(outMatrix.vRot);
	if (true == bScale)
		m_pTransformCom->Set_Scale(outMatrix.vScale);
}

void CCutSceneEffect_Tool::Draw_EffectValue(CGameInstance* _pGameInstance)
{
	_float4 vColor = _float4(1.f, 0.5f, 0.2f, 1.f);
	OUTMATRIX OutMatrix = CMethod::Out_Matrix(m_pTransformCom->Get_WorldFloat4x4());
	BoundingOrientedBox bBox1 = BoundingOrientedBox(OutMatrix.vPos, _float3(3.f, 3.f, 3.f), OutMatrix.vQuaternionRot);
	_pGameInstance->Draw(bBox1, CDrawing::VIEW_3D, vColor.Get_Vector());
}

void CCutSceneEffect_Tool::Save_CutSceneTex(HANDLE& _hFile, DWORD* _pByte)
{
	CMethod::Save_File_WString(_hFile, *_pByte, m_strEffectName);
	OUTMATRIX OutMatrix = CMethod::Out_Matrix(m_pTransformCom->Get_WorldMatrix());
	WriteFile(_hFile, &OutMatrix, sizeof(OUTMATRIX), _pByte, nullptr);
}

void CCutSceneEffect_Tool::Load_CutSceneTex(HANDLE& _hFile, DWORD* _pByte)
{
	m_strEffectName = CMethod::Load_File_WString(_hFile, *_pByte);
	OUTMATRIX OutMatrix;
	ReadFile(_hFile, &OutMatrix, sizeof(OUTMATRIX), _pByte, nullptr);

	if (nullptr != m_pTransformCom)
	{
		m_pTransformCom->Set_Pos(OutMatrix.vPos.Get_Vector());
		m_pTransformCom->Rotate_Turn(OutMatrix.vRot);
		m_pTransformCom->Set_Scale(OutMatrix.vScale);
	}
}

CCutSceneEffect_Tool* CCutSceneEffect_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, VOIDDATAS* _pDatas)
{
	CCutSceneEffect_Tool* pInstance = new CCutSceneEffect_Tool(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct(_pDatas)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CCutSceneEffect_Tool");
#endif
	}
	return pInstance;
}

void CCutSceneEffect_Tool::Free()
{
	Safe_Release(m_pTransformCom);

	Safe_Release_DV(m_pDevice);
	Safe_Release_DV(m_pContext);
}
