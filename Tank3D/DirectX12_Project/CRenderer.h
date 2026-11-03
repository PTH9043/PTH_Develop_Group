#pragma once
#include "CComponent.h"

enum RENDERID
{
	RENDER_PRIORITY,
	RENDER_ACTOR,
	RENDER_UI,
	RENDER_DEBUG,
	RENDER_END
};
class CActor;
class CGameInstance;

class CRenderer : public CComponent{
protected:
	explicit CRenderer() = default;
	explicit CRenderer(const CRenderer& _rhs) : CComponent(_rhs){}
	virtual ~CRenderer() = default;
public:
	virtual shared_ptr<CComponent> Clone(const VOIDDATAS& _tDatas) override;
	virtual void Free() override;
public:
	HRESULT NativeConstruct();
private:
	HRESULT RenderPriority(const HDC& _HDC);
	HRESULT RenderActor(const HDC& _HDC);
	HRESULT RenderUI(const HDC& _HDC);
#ifdef _DEBUG
	HRESULT RenderDebug(const HDC& _HDC);
#endif
public:
	void Add_Render(const RENDERID& _iRenderID, const shared_ptr<CActor>& _pActor);
	void Render(const HDC& _HDC);

private:
	array<list<shared_ptr<CActor>>, RENDER_END> m_RenderArrays;
	// GameInstance;
	weak_ptr<CGameInstance> m_pGameInstance ;
};

