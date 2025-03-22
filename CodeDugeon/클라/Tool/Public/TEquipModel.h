#pragma once
#include "UEquipment.h"

BEGIN(Tool)

class TEquipModel final : public UEquipment {
public:
	TEquipModel(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	TEquipModel(const TEquipModel& _rhs);
	DESTRUCTOR(TEquipModel)
public:
	// UPawn을(를) 통해 상속됨
	CLONE_MACRO(TEquipModel, "TEquipModel::Clone To Failed")
	virtual void Free() override;
public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _Datas) override;
public:
	using UEquipment::UpdateBoneNode;
	void UpdateBoneNode(const EQUIPTYPE _eEquipType, CSHPTRREF<UAnimModel> _spAnimModel,  CSHPTRREF<UModel> _spEquipModel, const _wstring& _wstrBoneNode,
		const _int _iWeaponOrShieldValue);
	void UpdateBoneNode(const EQUIPTYPE _eEquipType, CSHPTRREF<UCharacter> _spCharacter, CSHPTRREF<UModel> _spEquipModel, const _wstring& _wstrBoneNode,
		const _int _iWeaponOrShieldValue);
	void ChangeBoneNode(CSHPTRREF<UBoneNode> _spBoneNode);
	void ChangeOwner(CSHPTRREF<UPawn> _spOwner);

	void SetEventActive(const _bool _isEventActive) { this->m_isEventActive = _isEventActive; }
protected:
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta) override;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass = true) override;
	virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) override;
protected:
	_int									m_iWeaponOrShieldValue;
	_wstring							m_wstrBoneNodeName;
	_wstring							m_wstrModelName;
	_bool								m_isEventActive;
};

END