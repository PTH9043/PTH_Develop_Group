#pragma once

#include "UPawn.h"

BEGIN(Engine)
class UTransform;
class UAnimModel;
class UAnimationController;
class UNavigation;
class UShaderConstantBuffer;
class UTexGroup;

class UCharacter abstract : public UPawn {
public:
	enum {
		CHARACTERDESCORDER = 0
	};
	struct CHARACTERDESC {
		_wstring		wstrAnimModelProtoData;
		_wstring		wstrAnimControllerProtoData;
		_wstring		wstrDissolveTextureName;
		_bool				isNetworkConnected;

		CHARACTERDESC() :wstrAnimModelProtoData{ L" " }, wstrAnimControllerProtoData{ L" " }, isNetworkConnected{ false }, wstrDissolveTextureName{L""}
		{ }
		CHARACTERDESC(const _wstring& _wstrAnimModelPathData, const _wstring& _wstrAnimControllerProtoData,
			const _bool _isNetworkConnected = false) :
			wstrAnimModelProtoData{ _wstrAnimModelPathData }, wstrAnimControllerProtoData{ _wstrAnimControllerProtoData },
			isNetworkConnected{ _isNetworkConnected }, wstrDissolveTextureName{ L"T_FX_Enemy_Executioner_WarterNoiz" } {}
		CHARACTERDESC(const _wstring& _wstrAnimModelPathData, const _wstring& _wstrAnimControllerProtoData,
			const _wstring& _wstrDissoveTextureName, const _bool _isNetworkConnected = false) :
			wstrAnimModelProtoData{ _wstrAnimModelPathData }, wstrAnimControllerProtoData{ _wstrAnimControllerProtoData },
			isNetworkConnected{ _isNetworkConnected }, wstrDissolveTextureName{_wstrDissoveTextureName} {}
	};

public:
	UCharacter(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	UCharacter(const UCharacter& _rhs);
	DESTRUCTOR(UCharacter)
public:
	virtual SHPTR<UCloneObject> Clone(const VOIDDATAS& _vecDatas) PURE;
	virtual void Free() PURE;
	virtual HRESULT NativeConstruct() PURE;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _Datas) PURE;
public:
	_float OtherCharacterToDistance(CSHPTRREF<UActor> _spOtherActor);
	_float OtherCharacterDirToLook(CSHPTRREF<UActor> _spOtherActor);
	_float OhterCharacterDirToRight(CSHPTRREF<UActor> _spOtherActor);
	_float OtherCharacterDirToLookConverter(CSHPTRREF<UActor>  _spOtherActor);
	_float3 OtherCharacterDirToLookVectorF3(CSHPTRREF<UActor>  _spOtherActor);
	_float OtherCharacterToDistance(CSHPTRREF<UTransform> _spOtherTransform);
	_float OtherCharacterDirToLook(CSHPTRREF<UTransform> _spOtherTransform);
	_float OhterCharacterDirToRight(CSHPTRREF<UTransform> _spOtherTransform);
	_float OtherCharacterDirToLookConverter(CSHPTRREF<UTransform> _spOtherTransform);
	_float3 OtherCharacterDirToLookVectorF3(CSHPTRREF<UTransform> _spOtherTransform);
public: /* get set */
	CSHPTRREF<UAnimModel> GetAnimModel() const { return m_spAnimModel; }
	SHPTR<UNavigation> GetCurrentNavi() const { return m_spCurNavi; }
	CSHPTRREF<UCollider> GetHitCollider() const { return m_spHitCollider; }
	const _bool IsNetworkConnected() const { return m_isNetworkConnected; }

	const _bool& GetHitState() const { return m_bisHit; }
	void SetHitstate(_bool _newState) { m_bisHit = _newState; }

	const _bool& GetCollisionState() const { return m_bisCollision; }
	void SetCollisionState(_bool _newState) { m_bisCollision = _newState; }

	void SetOutline(_bool _Draw) { m_DrawOutline = _Draw; }
	void SetIfOutlineScale(_bool _Draw) { m_OutlineifScale = _Draw; }
	void SetOutlineColor(_float3 _color) { m_OutlineColor = _color; }
	void SetifPlayer(_bool _isPlayer) { m_isPlayer = _isPlayer; }

	const _bool& GetOutlineState()const { return m_DrawOutline; }
	const _bool& GetOutlineScaleState()const { return m_OutlineifScale; }

	void SetOutlineByAbility(_bool Set) { ifDrawOutlineByAbility = Set; }

	const _int& GetHealth() const { return m_iHealth; }
	const _int& GetMaxHealth() const { return m_iMaxHealth; }
	const _int& GetPrevHealth() const { return m_iPrevHealth; }

	void SetHealth(const _int& _inewHealth) { m_iHealth = _inewHealth; }
	void SetMaxHealth(const _int& _inewHealth) { m_iMaxHealth = _inewHealth; }
	void SetPrevHealth(const _int& _inewHealth) { m_iPrevHealth = _inewHealth; }

	void DecreaseHealth(_int amount);
	void IncreaseHealth(_int amount);
	const _bool& GetDeathState() { return m_bisDeath; }
	void SetDeathState(_bool _isDead) { m_bisDeath = _isDead; }

	const _int& GetAttack() const { return m_iAttack; }
	void SetAttack(const _int& _iAttack) { m_iAttack = _iAttack; }

	const _bool& GetIsHItAlreadyState() const { return m_bisHitAlready; }
	void SetHitAlreadyState(_bool _newState) { m_bisHitAlready = _newState; }
	void SetAnimModelRim(_int _SetRim);
	void SetAnimModelRimColor(_float3 _SetRimColor);

	_double GetElapsedTime() const { return m_delapsedTime; }
	void SetElapsedTime(_double _dvalue) { m_delapsedTime = _dvalue; }

	const _bool IsDamaged() const { return m_isDamaged; }
	void SetDamaged(const _bool _isDamaged) { this->m_isDamaged = _isDamaged; }
protected:
	virtual void TickActive(const _double& _dTimeDelta) PURE;
	virtual void LateTickActive(const _double& _dTimeDelta) PURE;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) PURE;
	virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) PURE;
	virtual HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass = true) override;
	virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) PURE;

	virtual HRESULT MakeCollider(const _float3& _vTranslate, const _float3& _vScale, const _int _ColliderType);
	void ApplySlidingMovement(const _float3& _collidedNormal, _float _speed, _float _deltaTime);
protected: /* get set */
	CSHPTRREF<UAnimationController> GetAnimationController() const { return m_spAnimationController; }
	const _float3& GetPrevPos() const { return m_vPrevPos; }
	const _float GetMovingSpeed() const { return m_fMoveSpeed; }
	const _float GetRunningSpeed() const { return m_fRunSpeed; }

	void SetMovingSpeed(const float& _newSpeed) { m_fMoveSpeed = _newSpeed; }
	void SetRunningSpeed(const float& _newSpeed) { m_fRunSpeed = _newSpeed; }

	void SetRunState(_bool _runState) { m_bIsRunning = _runState; }

	const _float3& GetMovingDirection() const { return m_f3MovedDirection; }
	const _float3& GetLastMovingDirection() const { return m_f3LastMovedDirection; }

	const _float3& GetCollidedNormal() const { return m_f3CollidedNormal; }
	void SetCollidedNormal(const _float3& _f3Normal) { m_f3CollidedNormal = _f3Normal; }
private:
	SHPTR< UShaderConstantBuffer>						m_spScaleOutlineBuffer;
	SHPTR< UShaderConstantBuffer>						m_spColorOutlineBuffer;
	// AnimationModel
	SHPTR< UAnimModel>					m_spAnimModel;

	SHPTR<UAnimationController>	m_spAnimationController;
	_float3												m_vPrevPos;
	SHPTR<UNavigation>					m_spCurNavi;
	SHPTR<UCollider>							m_spHitCollider;
	_float													m_fMoveSpeed;
	_float													m_fRunSpeed;
	_bool													m_bIsRunning;
	_bool													m_bisHit;
	_bool													m_bisCollision;
	_float3												m_f3MovedDirection;
	_float3												m_f3LastMovedDirection;
	_bool													m_isNetworkConnected;
	_double												m_delapsedTime;
	_bool													m_DrawOutline = false;
	_bool													m_OutlineifScale;
	_float3												m_OutlineColor;
	_bool													m_isPlayer = false;
	_float3												m_f3CollidedNormal;
	_int														m_iMaxHealth;
	_int														m_iHealth;
	_int														m_iPrevHealth;
	_int														m_iAttack;
	_bool													m_bisDeath;
	_bool													m_bisHitAlready;
	_bool													m_isDamaged;
	_bool													 ifDrawOutlineByAbility = false;
	// Dissolve Texture Group
	SHPTR<UTexGroup>						m_spDissovleTexTGroup;
	_wstring											m_wstrDissolveTextureName;
};

END
