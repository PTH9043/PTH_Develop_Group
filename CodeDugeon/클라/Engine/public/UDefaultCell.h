#pragma once
#include "UPawn.h"

BEGIN(Engine)
class UVIBufferCell;
class UShaderConstantBuffer;

struct DEBUGCOLOR {
	_float4 g_DebugColor;
};

class UDefaultCell : public UPawn {
public:
	UDefaultCell(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	UDefaultCell(const UDefaultCell& _rhs);
	DESTRUCTOR(UDefaultCell)
	SHPTR<UVIBufferCell> GetVIBuffer() { return m_spVIBuffer; }
	void SetVIBuffer(ARRAY<_float3, 3>& _Points);
public:
	CLONE_MACRO(UDefaultCell, "UDefaultCell::Clone To Failed")
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;

	void AddRenderer(RENDERID _eRenderID);
protected:
	// Tick, LateTick, Render
	virtual void TickActive(const _double& _dTimeDelta) override;
	virtual void LateTickActive(const _double& _dTimeDelta) override;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass = true) override;
	// Damaged
	virtual void Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta) override;
private:
	static constexpr _uint						DEBUGPARAM_SIZE{ sizeof(DEBUGCOLOR) };
	SHPTR<UVIBufferCell>						m_spVIBuffer;
	DEBUGCOLOR								m_stDebuggParam;
};

END