#pragma once

#include "TImGuiView.h"

BEGIN(Engine)
class UModel;
class UPawn;
END

BEGIN(Tool)
class TShowModelObject;

using MAPMODELCONTAINER = UNORMAP<_string, SHPTR<UModel>>;
using SHOWMAPMODELCONTAINER = UNORMAP<_string, SHPTR<TShowModelObject>>;
using MAPOBJECTSPOSLAYOUT = UNORMAP<_string, _float3>;
using DELETEMODELS = VECTOR<_string>;

class TMapView final : public TImGuiView {
public:
	TMapView(CSHPTRREF<UDevice> _spDevice);
	NO_COPY(TMapView)
	DESTRUCTOR(TMapView)
	// TImGuiView을(를) 통해 상속됨
	void Free() override;
	HRESULT NativeConstruct() override;
protected:
	HRESULT LoadResource() override;
	HRESULT ReleaseResource() override;
	void TickActive(const _double& _dTimeDelta) override;
	void LateTickActive(const _double& _dTimeDetla) override;
	void RenderActive() override;

private:
	void DockBuildInitSetting();
	void MapView();
	void LoadAssimpModelDatas(CSHPTRREF<FILEGROUP> _spFolder);
	void LoadMapModels();
	void ShowModelList();
	//void ClearCurrentModel();
	//void ClearAllShowModels();
	void MouseInput();

private:
	MAINDESC												m_stMainDesc;
	DOCKDESC												m_stMapView;

	_double													m_dShowDeltaTime;
	_bool													m_isInitSetting;
	_bool													m_bRenderColliders;
	_bool													m_bAddtoPicking;

	MAPMODELCONTAINER									m_MapModelContainer;
	SHOWMAPMODELCONTAINER								m_ShowMapModelContainer;
	_uint												m_iModelSuffix;

	SHPTR<UPawn>										m_spSelectedModel;
	_string												m_SelectedModelName;

	DELETEMODELS										m_DeleteModelsContainer;

	_bool												m_bisMapModelsLoaded;
};

END