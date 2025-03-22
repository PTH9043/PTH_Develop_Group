#pragma once
#include "CModelObjects.h"

BEGIN(Engine)
class UModel;
class UShaderConstantBuffer;
class UTrail;
class UNavigation;
END

BEGIN(Client)
class CHarlequinn;
/*
@ Date: 2024-05-12, Writer: ÀÌ¼ºÇö
@ Explain
- ½´¸®ÄË
*/
class CShurikenThrowing final : public CModelObjects{
public:
	struct SHURIKENDESC
	{
		_float4x4		_Worldm{};
	};

	CShurikenThrowing(CSHPTRREF<UDevice> _spDevice,
		const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	CShurikenThrowing(const CShurikenThrowing& _rhs);
	DESTRUCTOR(CShurikenThrowing)
	CLONE_MACRO(CShurikenThrowing, "CShurikenThrowing::Clone To Failed")
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;

	void ThrowShurikens(const _double& _dTimeDelta, _float3 _dir);
	void SetThrow(const _bool& _isThrow) { m_bisThrown = _isThrow; }
	const _bool& GetThrowingState() { return m_bisThrown; }

	const _float& GetTraveledDistance() { return m_ftraveledDistance; }
	void SetTraveledDistance(const _float& _new) { m_ftraveledDistance = _new; }

	SHPTR<UNavigation> GetCurrentNavi() const { return m_spCurNavi; }

protected:
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta) override;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass = true) override;
	virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) override;
private:
	
	SHPTR<UTrail>				m_spTrail;

	_float						m_ftraveledDistance;
	_bool						m_bisThrown;

	SHPTR<UNavigation>			m_spCurNavi;
	
};

END