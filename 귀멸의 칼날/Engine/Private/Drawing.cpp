#include "..\Public\Drawing.h"

#include "DebugDraw.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CDrawing)

CDrawing::CDrawing() 
	: m_fWinSizeX(0.f)
	,m_fWinSizeY(0.f),
	CBase()
{
}


HRESULT CDrawing::Initialize(HWND hWnd, ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	m_hWnd = hWnd;
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef_DV(m_pDevice);
	Safe_AddRef_DV(m_pContext);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	m_fWinSizeX = (_float)(rcClient.right - rcClient.left);
	m_fWinSizeY = (_float)(rcClient.bottom - rcClient.top);

	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	if (nullptr == m_pBatch)
		return E_FAIL;

	m_pEffect = new BasicEffect(m_pDevice);
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = nullptr;
	size_t	ShaderByteCodeLength = 0;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &ShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, ShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDrawing::DrawTriangle(const _fvector vPoint1, const _fvector vPoint2, const _fvector vPoint3, VIEWTYPE eType, _fvector color)
{
	if (VIEW_2D == eType)
	{
		m_pEffect->SetWorld(XMMatrixIdentity());
		m_pEffect->SetView(XMMatrixIdentity());
		m_pEffect->SetProjection(XMMatrixOrthographicLH(m_fWinSizeX, m_fWinSizeY, 0.f, 1.f));
	}
	else
	{
		m_pEffect->SetWorld(XMMatrixIdentity());
		m_pEffect->SetView(CPipeLine::Get_Instance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
		m_pEffect->SetProjection(CPipeLine::Get_Instance()->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	}

	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();

	DX::DrawTriangle(m_pBatch, vPoint1, vPoint2, vPoint3, color);

	m_pBatch->End();

	return S_OK;
}

HRESULT CDrawing::DrawLine(const _fvector vPoint1, const _fvector vPoint2, VIEWTYPE eType, _fvector color)
{
	if (VIEW_2D == eType)
	{
		m_pEffect->SetWorld(XMMatrixIdentity());
		m_pEffect->SetView(XMMatrixIdentity());
		m_pEffect->SetProjection(XMMatrixOrthographicLH(m_fWinSizeX, m_fWinSizeY, 0.f, 1.f));
	}
	else
	{
		m_pEffect->SetWorld(XMMatrixIdentity());
		m_pEffect->SetView(CPipeLine::Get_Instance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
		m_pEffect->SetProjection(CPipeLine::Get_Instance()->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	}

	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();

	DX::DrawLine(m_pBatch, vPoint1, vPoint2, color);

	m_pBatch->End();

	return S_OK;
}

HRESULT CDrawing::Draw(const BoundingBox & box, VIEWTYPE eType, _fvector color)
{
	if (VIEW_2D == eType)
	{
		m_pEffect->SetWorld(XMMatrixIdentity());
		m_pEffect->SetView(XMMatrixIdentity());
		m_pEffect->SetProjection(XMMatrixOrthographicLH(m_fWinSizeX, m_fWinSizeY, 0.f, 1.f));
	}
	else
	{
		m_pEffect->SetWorld(XMMatrixIdentity());
		m_pEffect->SetView(CPipeLine::Get_Instance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
		m_pEffect->SetProjection(CPipeLine::Get_Instance()->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	}
	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();

	DX::Draw(m_pBatch, box, color);

	m_pBatch->End();


	return S_OK;
}

HRESULT CDrawing::Draw(const BoundingOrientedBox & box, VIEWTYPE eType, _fvector color)
{
	if (VIEW_2D == eType)
	{
		m_pEffect->SetWorld(XMMatrixIdentity());
		m_pEffect->SetView(XMMatrixIdentity());
		m_pEffect->SetProjection(XMMatrixOrthographicLH(m_fWinSizeX, m_fWinSizeY, 0.f, 1.f));
	}
	else
	{
		m_pEffect->SetWorld(XMMatrixIdentity());
		m_pEffect->SetView(CPipeLine::Get_Instance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
		m_pEffect->SetProjection(CPipeLine::Get_Instance()->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	}
	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();

	DX::Draw(m_pBatch, box, color);

	m_pBatch->End();

	return S_OK;
}

HRESULT CDrawing::Draw(const BoundingSphere& sphere, VIEWTYPE eType, _fvector color)
{
	if (VIEW_2D == eType)
	{
		m_pEffect->SetWorld(XMMatrixIdentity());
		m_pEffect->SetView(XMMatrixIdentity());
		m_pEffect->SetProjection(XMMatrixOrthographicLH(m_fWinSizeX, m_fWinSizeY, 0.f, 1.f));
	}
	else
	{
		m_pEffect->SetWorld(XMMatrixIdentity());
		m_pEffect->SetView(CPipeLine::Get_Instance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
		m_pEffect->SetProjection(CPipeLine::Get_Instance()->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	}

	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();

	DX::Draw(m_pBatch, sphere, color);

	m_pBatch->End();

	return S_OK;
}


void CDrawing::Free()
{

	Safe_Release_DV(m_pDevice);
	Safe_Release_DV(m_pContext);

	Safe_Release_DV(m_pInputLayout);
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);

}

