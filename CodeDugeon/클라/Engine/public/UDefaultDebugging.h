#pragma once
#include "UPawn.h"

BEGIN(Engine)
class UVIBuffer;
class UShaderConstantBuffer;

class UDefaultDebugging : public UPawn{
public:
	enum class DEBUGTYPE
	{
		DEBUG_CUBE, DEBUG_SPHERE, DEBUG_CELL, DEBUG_CUBE_WHITE, DEBUG_END
	};
public:
	UDefaultDebugging(CSHPTRREF<UDevice> _spDevice, 	const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	UDefaultDebugging(const UDefaultDebugging& _rhs);
	DESTRUCTOR(UDefaultDebugging)


public:
	CLONE_MACRO(UDefaultDebugging, "UDefaultDebugging::Clone To Failed")
		virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;

	void AddRenderer(RENDERID _eRenderID);
	void SetColor(const _float3& _vChangeColor);
	void SetColor(const _float4& _vChangeColor);
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
	static constexpr _uint							DEBUGPARAM_SIZE {sizeof(DEBUGGINPARAM)};

	SHPTR< UVIBuffer>							m_spVIBuffer;
	DEBUGGINPARAM								m_stDebuggParam;
	DEBUGTYPE									m_eDebugType;
};

END