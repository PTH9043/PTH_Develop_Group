#include "RenderHandler.h"
#include "GameInstance.h"
#include "Light_Manager.h"

CRenderHandler::CRenderHandler(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
	Safe_AddRef_DV(m_pDevice);
	Safe_AddRef_DV(m_pContext);
}

HRESULT CRenderHandler::NativeConstruct()
{
	m_pTargetManager = CTarget_Manager::Get_Instance();
	Safe_AddRef(m_pTargetManager);

	m_pVIBufferRect = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (Is_Nullptr(m_pVIBufferRect))
		return E_FAIL;

	PATHS Path = { L"Effect", L"Deferred" };
	m_pDeferredTexture = CTexture::Create(m_pDevice, m_pContext, &Path);
	if (Is_Nullptr(m_pDeferredTexture))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderHandler::Begin_MRT(const _tchar* _pMrtName, CShader* _pShader, RH_VALUE_VEC _valueVec, RH_TEX_VEC _TexVec)
{
	if (nullptr == m_pTargetManager || nullptr == _pShader)
		return E_FAIL;

	if (FAILED(m_pTargetManager->Begin_MRT(m_pContext, _pMrtName)))
		return E_FAIL;

	for (auto& iter : _valueVec)
	{
		if (FAILED(_pShader->Set_RawValue(iter.strShaderName.c_str(), iter.pData, iter.dwValue)))
			return E_FAIL;
	}

	for (auto& iter : _TexVec)
	{
		if (FAILED(_pShader->Set_SRV(iter.strShaderName.c_str(), m_pTargetManager->Get_SRV(iter.strTargetTag.c_str()))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderHandler::Begin_MRT(const _tchar* _pMrtName, const _tchar* _pShaderTag, RH_VALUE_VEC _valueVec, RH_TEX_VEC _TexVec)
{
	if (nullptr == m_pTargetManager)
		return E_FAIL;
	CShader* pShader = m_pTargetManager->Get_Shader(_pShaderTag);
	if (nullptr == pShader)
		return E_FAIL;

	if (FAILED(m_pTargetManager->Begin_MRT(m_pContext, _pMrtName)))
		return E_FAIL;

	for (auto& iter : _valueVec)
	{
		if (FAILED(pShader->Set_RawValue(iter.strShaderName.c_str(), iter.pData, iter.dwValue)))
			return E_FAIL;
	}

	for (auto& iter : _TexVec)
	{
		if (FAILED(pShader->Set_SRV(iter.strShaderName.c_str(), m_pTargetManager->Get_SRV(iter.strTargetTag.c_str()))))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CRenderHandler::Render_Lights(const _tchar* _pMrtName, const _tchar* _pShaderTag, RH_VALUE_VEC _valueVec, RH_TEX_VEC _TexVec)
{
	if (FAILED(Begin_MRT(_pMrtName, _pShaderTag, _valueVec, _TexVec)))
		return E_FAIL;

	CShader* pShader = m_pTargetManager->Get_Shader(_pShaderTag);
	if (nullptr == pShader)
		return E_FAIL;
	if (FAILED(CLight_Manager::Get_Instance()->Render_Lights(pShader, m_pVIBufferRect)))
		return E_FAIL;

	if (FAILED(End_MRT(_pMrtName)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderHandler::End_MRT(const _tchar* _pMRTName)
{	
	// 셰이드 타겟에 값을 기록해 주기 위해 사각형 버퍼를 그린다. 빛의 개수 만큼
	if (FAILED(m_pTargetManager->End_MRT(m_pContext, _pMRTName)))
		return E_FAIL;
	return S_OK;
}

HRESULT CRenderHandler::Rendering(CShader* _pShader, const _uint& _iPassIndex, CVIBuffer_Rect* _pRect, RH_VALUE_VEC _valueVec, RH_TEX_VEC _TexVec)
{
	if (nullptr == _pRect || nullptr == _pShader)
		return E_FAIL;

	for (auto& iter : _valueVec)
	{
		if (FAILED(_pShader->Set_RawValue(iter.strShaderName.c_str(), iter.pData, iter.dwValue)))
			return E_FAIL;
	}

	for (auto& iter : _TexVec)
	{
		if (FAILED(_pShader->Set_SRV(iter.strShaderName.c_str(), m_pTargetManager->Get_SRV(iter.strTargetTag.c_str()))))
			return E_FAIL;
	}

	// 3 : Blend // 4 : Shadow
	_pShader->Begin(_iPassIndex);
	_pRect->Render();

	return S_OK;
}

HRESULT CRenderHandler::Rendering(const _tchar* _pShaderTag, const _uint& _iPassIndex, RH_VALUE_VEC _valueVec, RH_TEX_VEC _TexVec)
{
	if (nullptr == m_pTargetManager)
		return E_FAIL;
	CShader* pShader = m_pTargetManager->Get_Shader(_pShaderTag);
	if (nullptr == pShader)
		return E_FAIL;

	for (auto& iter : _valueVec)
	{
		if (FAILED(pShader->Set_RawValue(iter.strShaderName.c_str(), iter.pData, iter.dwValue)))
			return E_FAIL;
	}

	for (auto& iter : _TexVec)
	{
		if (FAILED(pShader->Set_SRV(iter.strShaderName.c_str(), m_pTargetManager->Get_SRV(iter.strTargetTag.c_str()))))
			return E_FAIL;
	}

	pShader->Begin(_iPassIndex);
	m_pVIBufferRect->Render();

	return S_OK;
}

HRESULT CRenderHandler::Rendering_Texture(const _tchar* _pMrtName, CShader* _pShader, const _uint& _iPassIndex, CVIBuffer_Rect* _pRect, RH_VALUE_VEC _valueVec, RH_TEX_VEC _TexVec)
{
	if (nullptr == _pRect || nullptr == _pShader || nullptr == m_pTargetManager)
		return E_FAIL;

	if (FAILED(m_pTargetManager->Begin_MRT(m_pContext, _pMrtName)))
		return E_FAIL;


	for (auto& iter : _valueVec)
	{
		if (FAILED(_pShader->Set_RawValue(iter.strShaderName.c_str(), iter.pData, iter.dwValue)))
			return E_FAIL;
	}

	for (auto& iter : _TexVec)
	{
		if (FAILED(_pShader->Set_SRV(iter.strShaderName.c_str(), m_pTargetManager->Get_SRV(iter.strTargetTag.c_str()))))
			return E_FAIL;
	}

	_pShader->Begin(_iPassIndex);
	_pRect->Render();

	// 셰이드 타겟에 값을 기록해 주기 위해 사각형 버퍼를 그린다. 빛의 개수 만큼
	if (FAILED(m_pTargetManager->End_MRT(m_pContext, _pMrtName)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderHandler::Rendering_Texture(const _tchar* _pMrtName, const _tchar* _pShaderTag, const _uint& _iPassIndex, RH_VALUE_VEC _valueVec, RH_TEX_VEC _TexVec)
{
	if (nullptr == m_pTargetManager || nullptr == m_pVIBufferRect)
		return E_FAIL;
	CShader* pShader = m_pTargetManager->Get_Shader(_pShaderTag);
	if (nullptr == pShader)
		return E_FAIL;

	if (FAILED(m_pTargetManager->Begin_MRT(m_pContext, _pMrtName)))
		return E_FAIL;

	for (auto& iter : _valueVec)
	{
		if (FAILED(pShader->Set_RawValue(iter.strShaderName.c_str(), iter.pData, iter.dwValue)))
			return E_FAIL;
	}

	for (auto& iter : _TexVec)
	{
		if (FAILED(pShader->Set_SRV(iter.strShaderName.c_str(), m_pTargetManager->Get_SRV(iter.strTargetTag.c_str()))))
			return E_FAIL;
	}

	pShader->Begin(_iPassIndex);
	m_pVIBufferRect->Render();

	// 셰이드 타겟에 값을 기록해 주기 위해 사각형 버퍼를 그린다. 빛의 개수 만큼
	if (FAILED(m_pTargetManager->End_MRT(m_pContext, _pMrtName)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderHandler::Rendering_GameObject(const _tchar* _pMRTName, list<class CGameObject*>* _ObjectList)
{
	if (nullptr == m_pTargetManager || nullptr == _ObjectList)
		return E_FAIL;

	if (FAILED(m_pTargetManager->Begin_MRT(m_pContext, _pMRTName)))
		return E_FAIL;

	for (auto& pGameObject : *_ObjectList)
	{
		if (FAILED(pGameObject->Render()))
		{
			MSG_BOX("Failed To Render : GameObject");
			return E_FAIL;
		}

		Safe_Release(pGameObject);
	}
	_ObjectList->clear();

	// 셰이드 타겟에 값을 기록해 주기 위해 사각형 버퍼를 그린다. 빛의 개수 만큼
	if (FAILED(m_pTargetManager->End_MRT(m_pContext, _pMRTName)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderHandler::Rendering_GameObject(list<class CGameObject*>* _ObjectList)
{
	if (nullptr == _ObjectList)
		return E_FAIL;

	for (auto& pGameObject : *_ObjectList)
	{
		if (nullptr != pGameObject)
		{

			if (FAILED(pGameObject->Render()))
			{
				MSG_BOX("Failed To Render : GameObject");
				return E_FAIL;
			}
		}

		Safe_Release(pGameObject);
	}
	_ObjectList->clear();

	return S_OK;
}

HRESULT CRenderHandler::Bind_Texture(const _tchar* _pShaderTag, const char* _pConstantName, _uint iIndex)
{
	if (nullptr == m_pTargetManager)
		return E_FAIL;
	CShader* pShader = m_pTargetManager->Get_Shader(_pShaderTag);
	if (nullptr == pShader)
		return E_FAIL;

	if (FAILED(m_pDeferredTexture->SetUp_ConstantTable_Index(pShader, _pConstantName, iIndex)))
		return E_FAIL;

	return S_OK;
}

CRenderHandler* CRenderHandler::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CRenderHandler* pInstance = new CRenderHandler(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
#ifdef _DEBUG
		MSG_BOX("CRenderHandler::Create To Failed");
#endif
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRenderHandler::Free()
{
	Safe_Release_DV(m_pDevice);
	Safe_Release_DV(m_pContext);
	
	Safe_Release(m_pVIBufferRect);
	Safe_Release(m_pDeferredTexture);
	Safe_Release(m_pTargetManager);
}
