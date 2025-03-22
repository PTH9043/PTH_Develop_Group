#pragma once

#include "UCharacter.h"

BEGIN(Engine)
class UAnimationController;
class UCamera;
class UStageManager;
class URegion;
/*
@ Date: 2024-04-30, Writer: ������
@ Explain
-  ����ڰ� ���� �����ϴ� Ŭ�����̴�. 
*/
class UPlayer abstract : public UCharacter  {
public:
	struct PLAYERDESC 
	{
		SHPTR<UCamera> spFollowCamera;

		PLAYERDESC() :	spFollowCamera{ nullptr }
		{}
		PLAYERDESC(CSHPTRREF<UCamera> _spFollowCamera) :
			spFollowCamera{ _spFollowCamera }
		{}
	};

	enum {
		PLAYERDESCORDER = 1
	};
public:
	UPlayer(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	UPlayer(const UPlayer& _rhs);
	DESTRUCTOR(UPlayer)
public:
	virtual SHPTR<UCloneObject> Clone(const VOIDDATAS& _vecDatas) PURE;
	virtual void Free() PURE;
	virtual HRESULT NativeConstruct() PURE;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _Datas) PURE;

	const _bool& GetJumpingState() { return m_bisJumping; }
	void SetJumpingState(const _bool& _jumpstate) { m_bisJumping = _jumpstate; }

	const _bool& GetFallingState() { return m_bisFalling; }
	void SetFallingState(const _bool& _fallstate) { m_bisFalling = _fallstate; }

	const _bool& GetFallOverState() { return m_bisFallOver; }
	void SetFallOverState(const _bool& _fallstate) { m_bisFallOver = _fallstate; }
	CSHPTRREF<UCamera> GetFollowCamera() const { return m_spFollowCamera; }
protected:
	virtual void TickActive(const _double& _dTimeDelta) PURE;
	virtual void LateTickActive(const _double& _dTimeDelta) PURE;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) PURE;
	virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) PURE;
	virtual HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass = true) override;
	virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) PURE;

	void FollowCameraMove(const _float3& _vPlayerToDistancePosition, const _double& _TimeElapsed);
	void JumpState(const _double& _dTimeDelta);
private:
	SHPTR<UCamera>		m_spFollowCamera;
	_bool								m_bisJumping;
	_bool								m_bisFalling;
	_bool								m_bisFallOver;
};

END