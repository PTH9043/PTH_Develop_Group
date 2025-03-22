#pragma once

#include "UItem.h"


BEGIN(Engine)
class UCharacter;
class UCollider;
class UBoneNode;
class UShaderConstantBuffer;
class UModel;
class UAnimModel;
class URootBoneNode;
/*
@ Date: 2024-04-27, Writer: 박태현
@ Explain
-  캐릭터가 들고 있게 하는 장비에 대한 클래스이다. 
*/
class UEquipment : public UItem {
	using HASBUFFERCONTAINER = ARRAY<_int, MAX_HAS_TEX>;
public:
	struct EQDESC {
		// 주인이 되는 Pawn
		SHPTR<UCharacter>	spOwner;
		SHPTR<UModel>			spEquipModel;
		EQUIPMENTINFO			EquipmentInfo;
		_wstring							wstrBoneNodeName;
		_int									iWeaponOrShieldValue;
		_float4x4						mPivotMatrix;

		EQDESC() :spOwner{nullptr}, spEquipModel{nullptr},
			EquipmentInfo{}, wstrBoneNodeName{ L"" }, iWeaponOrShieldValue{ 0 } { }

		EQDESC(CSHPTRREF<UModel> _spEquipModel, const _wstring& _wstrEquipDescPath);

		EQDESC(CSHPTRREF<UModel> _spEquipModel, SHPTR<UCharacter> _spOwner, const _wstring& _wstrEquipDescPath);

		EQDESC(CSHPTRREF<UCharacter> _spOwner, CSHPTRREF<UModel> _spEquipModel,
			const EQUIPMENTINFO& _EquipmentInfo, const _wstring& _wstrBoneNodeName, 
			const _int _iWeaponOrShieldValue, const _float4x4& _PivotMatrix = _float4x4::Identity) :
			spOwner{ _spOwner }, spEquipModel{ _spEquipModel },
			EquipmentInfo{ _EquipmentInfo }, wstrBoneNodeName{ _wstrBoneNodeName }, 
			iWeaponOrShieldValue{ _iWeaponOrShieldValue }, mPivotMatrix{_PivotMatrix}
		{}
		EQDESC(const EQUIPMENTINFO& _EquipmentInfo, const _wstring& _wstrBoneNodeName,
			const _int _iWeaponOrShieldValue, const _float4x4& _PivotMatrix = _float4x4::Identity) :
			spOwner{ nullptr }, spEquipModel{ nullptr },
			EquipmentInfo{ _EquipmentInfo }, wstrBoneNodeName{ _wstrBoneNodeName },
			iWeaponOrShieldValue{ _iWeaponOrShieldValue }, mPivotMatrix{ _PivotMatrix }
		{}
		
		void Save(const _wstring& _wstrPath);
		void Load(const _wstring& _wstrPath);
	};
	enum ORDER{ FIRST = 0 };
public:
	UEquipment(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer,
		const CLONETYPE& _eCloneType, const EQUIPTYPE _eEquipType);
	UEquipment(const UEquipment& _rhs);
	DESTRUCTOR(UEquipment)
public:
	// UPawn을(를) 통해 상속됨
	CLONE_MACRO(UEquipment, "UEquipment::Clone To Failed")
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _Datas) override;

	void SaveEquipDesc(const _wstring& _wstrPath);
	void ChangeEquipDescInfo(const EQDESC& _desc);
public: /* get set */
	const _float4x4& GetPivotMatrix() const { return m_PivotMatrix; }
	void SetPivotMatrix(const _float4x4& _mTargetModePivot) { this->m_PivotMatrix = _mTargetModePivot; }
protected:
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta) override;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass = true) override;
	virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) override;

	void BindShaderBuffer();
	void UpdateBoneNode(CSHPTRREF<UAnimModel> _spAnimModel, const _wstring& _wstrBoneNode);
	void UpdateBoneNode(CSHPTRREF<UCharacter> _spCharacter, const _wstring& _wstrBoneNode);
protected: /* get set */
	const EQUIPMENTINFO& GetEquipmentInfo() const { return m_EquipmentInfo; }
	CSHPTRREF<UModel> GetEquipModel() const { return m_spEquipModel; }
	CSHPTRREF<UBoneNode> GetEquipBoneNode() const { return m_spEquipBoneNode; }
	CSHPTRREF<UAnimModel> GetOwnerCharAnimModel() const { return m_spCharacterAnimModel; }
	EQUIPTYPE GetEquipType() const { return m_eEquipType; }
	SHPTR<UPawn> GetOwner() const { return m_wpOwner.lock(); }

	void SetEquipmentInfo(const EQUIPMENTINFO _EquipmentInfo) { this->m_EquipmentInfo = _EquipmentInfo; }
	void SetEquipModel(CSHPTRREF<UModel> _spModel) { this->m_spEquipModel = _spModel; }
	void SetEquipType(const EQUIPTYPE& _eEquipType) { this->m_eEquipType = _eEquipType; }
	void SetEquipBoneNode(CSHPTRREF<UBoneNode> _spBoneNode) { this->m_spEquipBoneNode = _spBoneNode; }
	void SetOwner(CSHPTRREF<UPawn> _spPawn) { this->m_wpOwner = _spPawn; }
	void SetSocketMatrix(const _float4x4& _matrix) { m_SockMatrixParam.SocketMatrix = _matrix; }
private:
	EQUIPTYPE												m_eEquipType;
	WKPTR<UPawn>										m_wpOwner;
	SHPTR<UModel>										m_spEquipModel;
	EQUIPMENTINFO										m_EquipmentInfo;
	// 장착되는 본 노드
	SHPTR<UBoneNode>								m_spEquipBoneNode;
	SHPTR<UAnimModel>							m_spCharacterAnimModel;


	SOCKETMATRIXPARAM							m_SockMatrixParam;
	SHPTR< UShaderConstantBuffer>		m_spSocketMatrixBuffer;
	SHPTR<UShaderConstantBuffer>		m_spTexCheckBuffer;
	// MaxHasTex
	HASBUFFERCONTAINER							m_HasTexContainer;
	_float4x4													m_PivotMatrix;
};

END

