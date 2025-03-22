#include "Test_Shader_Tool.h"
#include "GameInstance.h"

CTest_Shader_Tool::CTest_Shader_Tool(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CGameObject_Tool(_pDevice, _pContext)
{
}

CTest_Shader_Tool::CTest_Shader_Tool(const CTest_Shader_Tool& _rhs)
	: CGameObject_Tool(_rhs)
{
}

const char* CTest_Shader_Tool::Get_Name()
{
	return "Test_Shader_Tool";
}

const _tchar* CTest_Shader_Tool::Get_Proto()
{
	return PROTO_GOB_TEST_SHADER;
}

const _tchar* CTest_Shader_Tool::Get_Layer()
{
	return LAYER_TEST;
}

HRESULT CTest_Shader_Tool::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CTest_Shader_Tool::NativeConstruct_Clone(void* pArg)
{
	return S_OK;
}

void CTest_Shader_Tool::Tick(const _double& _dTimeDelta)
{
}

void CTest_Shader_Tool::Late_Tick(const _double& _dTimeDelta)
{
}

HRESULT CTest_Shader_Tool::Render()
{
	return S_OK;
}

HRESULT CTest_Shader_Tool::SetUp_ConstantValue()
{
	return S_OK;
}

HRESULT CTest_Shader_Tool::Ready_Components()
{
	return S_OK;
}
#pragma region USE_TOOL_METHOD
_bool CTest_Shader_Tool::Extern_EditValue()
{
	return _bool();
}

_bool CTest_Shader_Tool::Extern_HierakeyView(const string& _str)
{
	return _bool();
}

HRESULT CTest_Shader_Tool::Create_Self(CGameObject** _ppOut)
{
	return E_NOTIMPL;
}

HRESULT CTest_Shader_Tool::Delete_Self()
{
	return E_NOTIMPL;
}

HRESULT CTest_Shader_Tool::Save_Data()
{
	return E_NOTIMPL;
}

HRESULT CTest_Shader_Tool::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
	return E_NOTIMPL;
}

_bool CTest_Shader_Tool::Is_Picking(_float3* _pOut)
{
	return _bool();
}

void CTest_Shader_Tool::Reset_Data()
{
}
#pragma endregion USE_TOOL_METHOD
CTest_Shader_Tool* CTest_Shader_Tool::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CTest_Shader_Tool* pInstance = new CTest_Shader_Tool(_pDevice, _pContext);
	CHECK_CREATE_PROTOTYPE_NOTVALUE(pInstance, "CTest_Shader_Tool::Create To Failed");
	return pInstance;
}

CGameObject* CTest_Shader_Tool::Clone(void* _pArg)
{
	CTest_Shader_Tool* pInstance = new CTest_Shader_Tool(*this);
	CHECK_CREATE_CLONE(pInstance, _pArg,"CTest_Shader_Tool::Create To Failed");
	return pInstance;
}

void CTest_Shader_Tool::Free()
{
	Safe_Release(m_pVIBufferRect);
	__super::Free();
}
