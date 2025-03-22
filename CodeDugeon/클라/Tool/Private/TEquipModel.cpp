#include "ToolDefines.h"
#include "TEquipModel.h"
#include "UEquipment.h"
#include "UAnimModel.h"
#include "UTransform.h"
#include "UBoneNode.h"

TEquipModel::TEquipModel(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, 
	const CLONETYPE& _eCloneType) : 
	UEquipment(_spDevice, _wstrLayer, _eCloneType, EQUIP_END),
	m_iWeaponOrShieldValue{ 0 },
	m_wstrBoneNodeName{ L"" },
	m_wstrModelName{ L"" },
	m_isEventActive{false}
{
}

TEquipModel::TEquipModel(const TEquipModel& _rhs) :
	UEquipment(_rhs), 
	m_iWeaponOrShieldValue{ 0 },
	m_wstrBoneNodeName{ L"" },
	m_wstrModelName{ L"" },
	m_isEventActive{ false }
{
}

void TEquipModel::Free()
{
}

HRESULT TEquipModel::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT TEquipModel::NativeConstructClone(const VOIDDATAS& _Datas)
{
	return __super::NativeConstructClone(_Datas);
}

void TEquipModel::UpdateBoneNode(const EQUIPTYPE _eEquipType, CSHPTRREF<UAnimModel> _spAnimModel, CSHPTRREF<UModel> _spEquipModel,
	const _wstring& _wstrBoneNode, const _int _iWeaponOrShieldValue)
{
	SetEquipType(_eEquipType);
	m_iWeaponOrShieldValue = _iWeaponOrShieldValue;
	m_wstrBoneNodeName = _wstrBoneNode;
	SetEquipModel(_spEquipModel);
	UpdateBoneNode(_spAnimModel, _wstrBoneNode);
}

void TEquipModel::UpdateBoneNode(const EQUIPTYPE _eEquipType, CSHPTRREF<UCharacter> _spCharacter, CSHPTRREF<UModel> _spEquipModel
	, const _wstring& _wstrBoneNode, const _int _iWeaponOrShieldValue)
{
	SetEquipType(_eEquipType);
	m_iWeaponOrShieldValue = _iWeaponOrShieldValue;
	m_wstrBoneNodeName = _wstrBoneNode;
	SetEquipModel(_spEquipModel);
	UpdateBoneNode(_spCharacter, _wstrBoneNode);
}

void TEquipModel::ChangeBoneNode(CSHPTRREF<UBoneNode> _spBoneNode)
{
	RETURN_CHECK(nullptr == _spBoneNode, ;);
	SetEquipBoneNode(_spBoneNode);
}

void TEquipModel::ChangeOwner(CSHPTRREF<UPawn> _spOwner)
{
	RETURN_CHECK(nullptr == _spOwner, ;);
	SetOwner(_spOwner);
}


void TEquipModel::TickActive(const _double& _dTimeDelta)
{
	__super::TickActive(_dTimeDelta);
}

void TEquipModel::LateTickActive(const _double& _dTimeDelta)
{
	__super::LateTickActive(_dTimeDelta);
}

HRESULT TEquipModel::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	if (FAILED(__super::RenderActive(_spCommand, _spTableDescriptor)))
		return E_FAIL;

	return S_OK;
}

HRESULT TEquipModel::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return __super::RenderShadowActive(_spCommand, _spTableDescriptor);
}
HRESULT TEquipModel::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor,_pass);
}
void TEquipModel::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}
