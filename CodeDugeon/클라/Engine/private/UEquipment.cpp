#include "EngineDefine.h"
#include "UEquipment.h"
#include "UMethod.h"
#include "UShaderConstantBuffer.h"
#include "UModel.h"
#include "UTransform.h"
#include "UModelMaterial.h"
#include "UShader.h"
#include "UCharacter.h"
#include "UAnimModel.h"
#include "URootBoneNode.h"
#include "UMethod.h"
#include "URootBoneNode.h"
#include "UAnimModel.h"

/*
		SHPTR<UCharacter>	spOwner;
		SHPTR<UModel>			spEquipModel;
		EQUIPMENTINFO			EquipmentInfo;
		_wstring							wstrBoneNodeName;
		_int									iWeaponOrShieldValue;
		_float4x4						mPivotMatrix;
*/

UEquipment::EQDESC::EQDESC(CSHPTRREF<UModel> _spEquipModel, const _wstring& _wstrEquipDescPath) :
	spEquipModel{_spEquipModel}
{
	assert(nullptr != _spEquipModel);

	_wstring str = UMethod::MakePath(_wstrEquipDescPath, L"EquipDesc");
	str += L"\\";
	str += spEquipModel->GetModelName();

	Load(str);
}

UEquipment::EQDESC::EQDESC(CSHPTRREF<UModel> _spEquipModel, SHPTR<UCharacter> _spOwner, const _wstring& _wstrEquipDescPath) :
	spOwner{_spOwner},
	spEquipModel{ _spEquipModel }
{
	Load(_wstrEquipDescPath);
}

void UEquipment::EQDESC::Save(const _wstring& _wstrPath)
{
	std::ofstream Save{ _wstrPath, std::ios::binary };
	assert(Save.is_open());

	Save.write((_char*)&EquipmentInfo, sizeof(EQUIPMENTINFO));
	UMethod::SaveString(Save, wstrBoneNodeName);
	Save.write((_char*)&iWeaponOrShieldValue, sizeof(_int));
	Save.write((_char*)&mPivotMatrix, sizeof(_float4x4));
}

void UEquipment::EQDESC::Load(const _wstring& _wstrPath)
{
	std::ifstream Read{ _wstrPath, std::ios::binary };
	assert(Read.is_open());

	Read.read((_char*)&EquipmentInfo, sizeof(EQUIPMENTINFO));
	UMethod::ReadString(Read, wstrBoneNodeName);
	Read.read((_char*)&iWeaponOrShieldValue, sizeof(_int));
	Read.read((_char*)&mPivotMatrix, sizeof(_float4x4));
}

/*
=========================================
UEquipment::EQDESC
=========================================
UEquipment::Class
=========================================
*/

UEquipment::UEquipment(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, 
	const CLONETYPE& _eCloneType, const EQUIPTYPE _eEquipType) :
	UItem(_spDevice, _wstrLayer, _eCloneType, BACKINGTYPE::DYNAMIC, ITEMTYPE::ITEM_EQUIP),
	m_eEquipType{_eEquipType},
	m_spEquipModel{nullptr},
	m_EquipmentInfo{},
	m_spEquipBoneNode{ nullptr },
	m_SockMatrixParam{ _float4x4::Identity },
	m_spSocketMatrixBuffer{ nullptr },
	m_spTexCheckBuffer{nullptr},
	m_HasTexContainer{},
	m_PivotMatrix{_float4x4::Identity}
{
}

UEquipment::UEquipment(const UEquipment& _rhs) : 
	UItem(_rhs),
	m_spEquipModel{ nullptr },
	m_EquipmentInfo{},
	m_spEquipBoneNode{ nullptr },
	m_SockMatrixParam{ _float4x4::Identity },
	m_spSocketMatrixBuffer{ nullptr },
	m_spTexCheckBuffer{ nullptr },
	m_HasTexContainer{},
	m_PivotMatrix{ _float4x4::Identity }
{
}


void UEquipment::Free()
{
}

HRESULT UEquipment::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT UEquipment::NativeConstructClone(const VOIDDATAS& _Datas)
{
	if (FAILED(__super::NativeConstructClone(_Datas)))
		return E_FAIL;

	// 해당 VOIDDATAS에 존재할 때
	if (_Datas.size() >= 1)
	{
		UEquipment::EQDESC desc = UMethod::ConvertTemplate_Index<UEquipment::EQDESC>(_Datas, FIRST);
		m_spEquipModel = desc.spEquipModel;
		::memcpy(&m_EquipmentInfo, &desc.EquipmentInfo, sizeof(EQUIPMENTINFO));
		m_PivotMatrix = desc.mPivotMatrix;
		SHPTR<UCharacter> spCharacter = desc.spOwner;
		m_eEquipType = static_cast<EQUIPTYPE>(desc.iWeaponOrShieldValue);
		// Find Root Bone Node 
		if (nullptr != spCharacter)
		{
			m_spEquipBoneNode = spCharacter->GetAnimModel()->FindBoneNode(desc.wstrBoneNodeName);
			m_spCharacterAnimModel = spCharacter->GetAnimModel();
		}
		if (nullptr != m_spEquipModel)
		{
			_uint ModelBufferSize = m_spEquipModel->GetMeshContainerCnt();

			m_spSocketMatrixBuffer = CreateNative< UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::SOCKETMATRIX, GetTypeSize<SOCKETMATRIXPARAM>(), ModelBufferSize);
			m_spTexCheckBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::MODELCHECKBUF, GetTypeSize< HASBUFFERCONTAINER>(), ModelBufferSize);
		}
		m_wpOwner = spCharacter;
	}
	// Add Shader
	AddShader(PROTO_RES_EQUIPMENTSHADER);
	AddShadowShader(PROTO_RES_SHADOWEQUIPMENTSHADER, RES_SHADER);
	return S_OK;
}

void UEquipment::SaveEquipDesc(const _wstring& _wstrPath)
{
	assert(nullptr != m_spEquipModel);
	EQDESC desc{ m_EquipmentInfo, m_spEquipBoneNode->GetName(), m_eEquipType, m_PivotMatrix };

	_wstring str = UMethod::MakeFolderAndReturnPath(_wstrPath, L"EquipDesc");
	str += m_spEquipModel->GetModelName();
	desc.Save(str);
}

void UEquipment::ChangeEquipDescInfo(const EQDESC& _desc)
{
	m_spEquipModel = _desc.spEquipModel;
	::memcpy(&m_EquipmentInfo, &_desc.EquipmentInfo, sizeof(EQUIPMENTINFO));
	m_PivotMatrix = _desc.mPivotMatrix;
	m_eEquipType = static_cast<EQUIPTYPE>(_desc.iWeaponOrShieldValue);
}

void UEquipment::TickActive(const _double& _dTimeDelta)
{
//	m_SockMatrixParam.SocketMatrix = _float4x4::Identity;
}

void UEquipment::LateTickActive(const _double& _dTimeDelta)
{
	if (nullptr != m_spEquipBoneNode)
	{
		SHPTR<UPawn> spPawn = m_wpOwner.lock();
		SHPTR<UTransform> spTransform = spPawn->GetTransform();

		// Get CombineMatrix
		m_SockMatrixParam.SocketMatrix = m_spEquipBoneNode->GetCombineMatrix() * m_spCharacterAnimModel->GetPivotMatirx() * spTransform->GetWorldMatrix();
		m_SockMatrixParam.SocketMatrix =  m_SockMatrixParam.SocketMatrix.Transpose();
	}

	if (nullptr != m_spEquipModel)
	{
		AddShadowRenderGroup(RI_SHADOW);
		AddRenderGroup(RI_NONALPHA_MIDDLE);
	}
}

HRESULT UEquipment::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderActive(_spCommand, _spTableDescriptor);

	for (_uint i = 0; i < m_spEquipModel->GetMeshContainerCnt(); ++i)
	{
		// Bind Transform 
		GetTransform()->BindTransformData(GetShader());

		m_spEquipModel->BindTexture(i, SRV_REGISTER::T0, TEXTYPE::TextureType_DIFFUSE, GetShader());
		m_spEquipModel->BindTexture(i, SRV_REGISTER::T1, TEXTYPE::TextureType_SPECULAR, GetShader());
		m_spEquipModel->BindTexture(i, SRV_REGISTER::T2, TEXTYPE::TextureType_NORMALS, GetShader());

		m_HasTexContainer[0] = m_spEquipModel->GetMaterials()[i]->IsEmpty(TextureType_DIFFUSE) == true ? 0 : 1;
		m_HasTexContainer[1] = m_spEquipModel->GetMaterials()[i]->IsEmpty(TextureType_SPECULAR) == true ? 0 : 1;
		m_HasTexContainer[2] = m_spEquipModel->GetMaterials()[i]->IsEmpty(TextureType_NORMALS) == true ? 0 : 1;

		BindShaderBuffer();
		// Render
		m_spEquipModel->Render(i, GetShader(), _spCommand);
	}
	return S_OK;
}

HRESULT UEquipment::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{

	if (nullptr != m_spEquipModel)
	{
		__super::RenderShadowActive(_spCommand, _spTableDescriptor);

		for (_uint i = 0; i < m_spEquipModel->GetMeshContainerCnt(); ++i)
		{
			// Bind Transform 
			GetTransform()->BindTransformData(GetShadowShader());

			//m_spEquipModel->BindTexture(i, SRV_REGISTER::T0, TEXTYPE::TextureType_DIFFUSE, GetShadowShader());
			//m_spEquipModel->BindTexture(i, SRV_REGISTER::T1, TEXTYPE::TextureType_NORMALS, GetShadowShader());
			// Render
			GetShadowShader()->BindCBVBuffer(m_spSocketMatrixBuffer, &m_SockMatrixParam, GetTypeSize<SOCKETMATRIXPARAM>());
	//		GetShadowShader()->BindCBVBuffer(m_spTexCheckBuffer, &m_HasTexContainer, GetTypeSize< HASBUFFERCONTAINER>());
			m_spEquipModel->Render(i, GetShadowShader(), _spCommand);
		}
	}
	return S_OK;
}

HRESULT UEquipment::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return __super::RenderOutlineActive(_spCommand, _spTableDescriptor,_pass);
}

void UEquipment::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}


void UEquipment::BindShaderBuffer()
{
	GetShader()->BindCBVBuffer(m_spSocketMatrixBuffer, &m_SockMatrixParam, GetTypeSize<SOCKETMATRIXPARAM>());
	GetShader()->BindCBVBuffer(m_spTexCheckBuffer, &m_HasTexContainer, GetTypeSize< HASBUFFERCONTAINER>());
}

void UEquipment::UpdateBoneNode(CSHPTRREF<UAnimModel> _spAnimModel, const _wstring& _wstrBoneNode)
{
	RETURN_CHECK(nullptr == _spAnimModel, ;);
	m_spEquipBoneNode = _spAnimModel->FindBoneNode(_wstrBoneNode);
	m_PivotMatrix = _spAnimModel->GetPivotMatirx();

	if (nullptr == m_spSocketMatrixBuffer)
	{
		_uint ModelBufferSize = m_spEquipModel->GetMeshContainerCnt();
		m_spSocketMatrixBuffer = CreateNative< UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::SOCKETMATRIX, GetTypeSize<SOCKETMATRIXPARAM>(), ModelBufferSize);
		m_spTexCheckBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::MODELCHECKBUF, GetTypeSize< HASBUFFERCONTAINER>(), ModelBufferSize);
		m_spCharacterAnimModel = _spAnimModel;
	}
}


void UEquipment::UpdateBoneNode(CSHPTRREF<UCharacter> _spCharacter, const _wstring& _wstrBoneNode)
{
	RETURN_CHECK(nullptr == _spCharacter, ;);
	SHPTR<UAnimModel> spAnimModel = _spCharacter->GetAnimModel();
	RETURN_CHECK(nullptr == spAnimModel, ;);
	m_spEquipBoneNode = spAnimModel->FindBoneNode(_wstrBoneNode);
	m_PivotMatrix = spAnimModel->GetPivotMatirx();
	m_spCharacterAnimModel = spAnimModel;

	if (nullptr == m_spSocketMatrixBuffer)
	{
		_uint ModelBufferSize = m_spEquipModel->GetMeshContainerCnt();
		m_spSocketMatrixBuffer = CreateNative< UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::SOCKETMATRIX, GetTypeSize<SOCKETMATRIXPARAM>(), ModelBufferSize);
		m_spTexCheckBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::MODELCHECKBUF, GetTypeSize< HASBUFFERCONTAINER>(), ModelBufferSize);
	}
}