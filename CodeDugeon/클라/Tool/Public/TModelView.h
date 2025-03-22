#pragma once 
#include "TImGuiView.h"

BEGIN(Engine)
class UModel;
class UAnimModel;
class UPawn;
class UMapLayout;
END

BEGIN(Tool)
class TShowModelObject;
class TShowAnimModelObject;
class TGuizmoManager;
/*
@ Date: 2024-02-04, Writer: 박태현
@ Explain
- Model들을 로드해서 보여주기 위한 클래스
*/
class TModelView final : public TImGuiView {
public:
	using MODELPAIR = std::pair < _string, SHPTR<UModel>>;
	using ANIMMODELPAIR = std::pair < _string, SHPTR<UAnimModel>>;
	TModelView(CSHPTRREF<UDevice> _spDevice);
	NO_COPY(TModelView)
	DESTRUCTOR(TModelView)
public:
	virtual void Free() override;

	virtual HRESULT NativeConstruct() override;
protected:
	virtual HRESULT LoadResource() override;
	virtual HRESULT ReleaseResource() override;
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDetla) override;
	virtual void RenderActive() override;

private:
	void DockBuildInitSetting();

	void ShowModels();

	// Show Model List
	void ShowModelList();
	void LoadObjectLayoutsFromFile();
	void LoadMobsLayoutsFromFile();
	void ShowAnimModelList();

	void ConvertModels();
	void ConvertAnimModels();

	void LoadAssimpModelDatas(CSHPTRREF<FILEGROUP> _spFolder);
	void LoadAnimModelData(CSHPTRREF<FILEGROUP> _spFolder);
	void ResetModels();
	void ResetAnimModels();

	void EditModel();

	void AddModelstoMobsLayout();

	void AddModelstoMapLayout();

	void SaveCurrentMapLayouts();
	void SaveCurrentMobsLayouts();

	void ClearCurrentModel();
	void ClearAllShowModels();
	void ClearCurrentAnimModel();

	void MouseInput();
	void KeyboardInput();

	HRESULT CopyCurrentModel();

private:
	using MODELS = UNORMAP<_string, SHPTR<UModel>>;
	using ANIMMODEL = UNORMAP<_string, SHPTR<UAnimModel>>;
	using SHOWMODELS = UNORMAP<_string, SHPTR<TShowModelObject>>;
	using SHOWANIMEMODELS = UNORMAP<_string, SHPTR<TShowAnimModelObject>>;

	using DELETEMODELS = VECTOR<_string>;
	using DELETEANIMMODELS = VECTOR<_string>;

	MAINDESC											m_stMainDesc;
	DOCKDESC											m_stModelDockDesc;
	DOCKDESC											m_stAnimModelDockDesc;
	DOCKDESC											m_stTransformEditorDesc;

	MODELS												m_ModelsContainer;
	ANIMMODEL											m_AnimModelContainer;
	SHOWMODELS											m_ShowModelsContainer;
	SHOWANIMEMODELS										m_ShowAnimModelsContainer;

	DELETEMODELS										m_DeleteModelsContainer;
	DELETEANIMMODELS									m_DeleteAnimModelsContainer;

	SHPTR<FILEGROUP>									m_spModelFileFolder;
	SHPTR<FILEGROUP>									m_spAnimModelFileFolder;

	SHPTR<UPawn>										m_spSelectedModel;
	_string												m_SelectedModelName;
	_string												m_CopiedModelName;

	SHPTR<UPawn>										m_spCopiedModel;

	SHPTR<TShowAnimModelObject>							m_spShowAnimModelObject;
	SHPTR<TShowModelObject>								m_spShowModelObject;

	_uint												m_iFileIndex;

	_bool												m_isAllConverter;
	_float3												m_vModelScale;
	_float3												m_vAnimModelScale;

	_bool												m_isInitSetting;
	_bool												m_isResetModel;
	_bool												m_isResetAnimModel;

	SHPTR<TGuizmoManager>								m_spGuizmoManager;

	_bool												m_bSelectedhasAnim;
	_bool												m_bColliderActive;

	SHPTR<UMapLayout>									m_spMapLayout;
	_bool												m_bshowLayoutAddPopup;
	_bool												m_bLayoutSavePopup;
	_bool												m_bLayoutLoadPopup;
	_bool												m_bErrorPopup;
	_bool												m_bEditPosByPicking;
	_bool												m_bisModelsLoaded;
	_bool												m_bisAnimModelsLoaded;
};

END
