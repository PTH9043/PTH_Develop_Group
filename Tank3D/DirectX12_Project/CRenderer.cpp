#include "CRenderer.h"
#include "CActor.h" 
#include "CGameInstance.h"

shared_ptr<CComponent> CRenderer::Clone(const VOIDDATAS& _tDatas)
{
	return ThisShared<CComponent>();
}

void CRenderer::Free()
{
}

HRESULT CRenderer::NativeConstruct()
{
	m_pGameInstance = GET_INSTNACE(CGameInstance);
	return S_OK;
}

HRESULT CRenderer::RenderPriority(const HDC& _HDC)
{
	for (auto& iter : m_RenderArrays[RENDER_PRIORITY])
		iter->Render(_HDC);

	m_RenderArrays[RENDER_PRIORITY].clear();
	return S_OK;
}

HRESULT CRenderer::RenderActor(const HDC& _HDC)
{
	for (auto& iter : m_RenderArrays[RENDER_ACTOR])
		iter->Render(_HDC);

	m_RenderArrays[RENDER_ACTOR].clear();
	return S_OK;
}

HRESULT CRenderer::RenderUI(const HDC& _HDC)
{
	for (auto& iter : m_RenderArrays[RENDER_UI])
		iter->Render(_HDC);

	m_RenderArrays[RENDER_UI].clear();
	return S_OK;
}
#ifdef _DEBUG
HRESULT CRenderer::RenderDebug(const HDC& _HDC)
{
	for (auto& iter : m_RenderArrays[RENDER_DEBUG])
		iter->Render(_HDC);

	m_RenderArrays[RENDER_DEBUG].clear();
	return S_OK;
}
#endif
void CRenderer::Add_Render(const RENDERID& _iRenderID, const shared_ptr<CActor>& _pActor)
{
	if (_iRenderID >= m_RenderArrays.size())
		return;

	m_RenderArrays[_iRenderID].push_back(_pActor);
}

void CRenderer::Render(const HDC& _HDC)
{
	// Bind
	m_pGameInstance.lock()->BindCameraMatrix();
	RenderPriority(_HDC);
	RenderActor(_HDC);
	RenderUI(_HDC);
#ifdef _DEBUG
	RenderDebug(_HDC);
#endif
}
