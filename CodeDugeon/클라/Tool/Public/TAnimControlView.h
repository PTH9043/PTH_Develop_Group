#pragma once

#include "TImGuiView.h"

BEGIN(Engine)
class UAnimModel;
class UAnimation;
class UAnimEvent;
class UModel;
class UCollider;
END

BEGIN(Tool)
class TShowAnimModelObject;
class TAnimControlModel;
class TEquipModel;

using ANIMFILECONTAINER = UNORMAP<_string, SHPTR<FILEDATA>>;
using ITEMMODELCONTAINER = UNORMAP<_string, SHPTR<UModel>>;

/*
@ Date: 2024-02-04, Writer: 박태현
@ Explain
- Imgui Animation Tool
*/
class TAnimControlView final : public TImGuiView {
	using ANIMEVENTCOLLIDERCONTAINER = UNORMAP<_int, SHPTR<UCollider>>;
public:
	TAnimControlView(CSHPTRREF<UDevice> _spDevice);
	NO_COPY(TAnimControlView)
	DESTRUCTOR(TAnimControlView)
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
	void DockBuildInitSetting();
	void AnimModelSelectView();
	void AnimModifyView();
	void EquipView();
	void MeshFilterView();
private:
	void MakeAnimEvent();

	void AnimSectionShow(CSHPTRREF<UAnimation> _spAnim, ImGuiTableFlags _flags,  const VECTOR<SHPTR<UAnimEvent>>& _AnimEvent);
	void AnimColliderShow(CSHPTRREF<UAnimation> _spAnim, ImGuiTableFlags _flags, const VECTOR<SHPTR<UAnimEvent>>& _AnimEvent);
	void AnimSoundShow(CSHPTRREF<UAnimation> _spAnim, ImGuiTableFlags _flags, const VECTOR<SHPTR<UAnimEvent>>& _AnimEvent);
	
	void FindEquipModel(CSHPTRREF<FILEGROUP> _spFileGroup);
private:
	MAINDESC												m_stMainDesc;
	DOCKDESC											m_stAnimModelSelectDesc;
	DOCKDESC											m_stAnimModifyDesc;
	DOCKDESC											m_stEquipViewDesc;
	DOCKDESC											m_stAnimMeshFilterDesc;
	_bool														m_isInitSetting;

	SHPTR<TAnimControlModel>			m_spAnimControlModel;
	SHPTR<UAnimModel>						m_spShowAnimModel;
	ANIMFILECONTAINER							m_AnimFileContainer;
	SHPTR<FILEGROUP>							m_spSelectAnimFileFolder;
	SHPTR<FILEDATA>								m_spSelectAnimFileData;

	SHPTR<UAnimation>							m_spSelectAnim;
	_int															m_iSelectAnimEvent;
	_int															m_AnimMaxTagCount;
	_string													m_strSelectAnimationName;

	static 	const _char*								s_AnimTags[1000];

	LIST<_string>										m_FindSoundNames;

	ITEMMODELCONTAINER						m_EquipModelContainer;
	SHPTR<UModel>									m_spSelectEquipModelData;
	_string													m_strSelectedEquipModelName;
	EQUIPTYPE											m_eEquipType;
	_int															m_iWeaponOrShieldValue;
	_wstring													m_wstrBoneNodeName;

	ANIMEVENTCOLLIDERCONTAINER	m_AnimEventColliderContainer;
};

END