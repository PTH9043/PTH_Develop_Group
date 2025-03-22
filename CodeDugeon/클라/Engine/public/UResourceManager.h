#pragma once
#include "UBase.h"

BEGIN(Engine)
class UResource;
class UShader;
class UCommand;
class URenderer;

class UResourceManager : public UBase {
public:
	UResourceManager();
	NO_COPY(UResourceManager)
	DESTRUCTOR(UResourceManager)
public:
	virtual void Free() override;

	HRESULT AddPrototype(const _wstring& _wstrPrototype, const CLONETYPE& _eCloneType, CSHPTRREF<UResource> _spResource);
	SHPTR<UResource> CloneResource(const _wstring& _wstrPrototype, const VOIDDATAS& _stDatas = VOIDDATAS{});
	void		FindResourceProtoTag(CSHPTRREF<UResource> _spResource, _wstring& _wstrResourceTag);
	HRESULT ClearOnceTypeData();

	void AddPipeLineState(const _wstring& _wstrName, const ComPtr<Dx12PipelineState>& _cpPipeLineState);
	void SettingPipeLineState(const _wstring& _wstrName, CSHPTRREF<UCommand> _spCommand);
private:
	// Find Prototype
	SHPTR<UResource> FindPrototype(const _ubyte _bCloneType, const _wstring& _strPrototype);

private:
	using PROTOTYPE = UNORMAP<_wstring, SHPTR<UResource>>;
	using PROTOTYPEARR = ARRAY<PROTOTYPE, CLONE_MAX_SIZE>;
	using PIPELINECONTAINER = UNORMAP<_wstring, ComPtr<Dx12PipelineState>>;

	PROTOTYPEARR				m_arrPrototypes;
	PIPELINECONTAINER		m_PipeLineContainer;
};

END