#pragma once

#include "TImGuiView.h"

BEGIN(Engine)
class UStageManager;
class URegion;
class UDefaultDebugging;
class UCell;

END

BEGIN(Tool)
class TGuizmoManager;
class TNavigationView : public TImGuiView {
public:
	enum SELECT_OBJ
	{
		SEL_1, SEL_2, SEL_3, SEL_END
	};
	TNavigationView(CSHPTRREF<UDevice> _spDevice);
	NO_COPY(TNavigationView)
	DESTRUCTOR(TNavigationView)
public:
	// TImGuiView을(를) 통해 상속됨
	virtual void Free() override;

	virtual HRESULT NativeConstruct() override;
protected:
	virtual HRESULT LoadResource() override;
	virtual HRESULT ReleaseResource() override;
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDetla) override;
	virtual void RenderActive() override;
private:
	void RenderMenu();
	void DockBuildInitSetting();

	void NavigationView();
	void ModifyNavigation(CSHPTRREF<URegion> _spRegion);

	void SaveCurrentRegions();
	void LoadRegionsFromFile();

private:
	MAINDESC												m_stMainDesc;
	DOCKDESC												m_stNavigationView;

	VECTOR<SHPTR<TImGuiView>>								m_CloseImGuies;
	SET<SHPTR<TImGuiView>>									m_OpenImGuies;

	_double													m_dShowDeltaTime;
	_bool													m_isInitSetting;

	SHPTR<UStageManager>									m_spStageManager;
	VECTOR<_float3>											m_vecPosList;
	SHPTR<UDefaultDebugging>								m_spCubePosArr[SEL_END];
	_uint													m_iSelIndex;
	_bool													m_bSelEnd;
private:
	_bool													m_bNavigationModify;
	_bool													m_bAllRender;
	_bool													m_bRenderWireFrame;
	_bool													m_bNavigationDebugColor;
	_bool													m_bOnWindow;
	_bool													m_bPickOnCell;

	_uint													m_iCreateRegionIndex;
	_uint													m_iRegionIndex;
	_uint													m_iCellIndex;
	_int													m_iWillRenderCellIndexBegin;
	_int													m_iWillRenderCellIndexEnd;

	SHPTR<UCell>											m_spSelectedCell;
};
END