#ifndef _PTH_FRAMEWORK_ENGIEN_PUBLIC_URENDERCOMMAND_H
#define _PTH_FRAMEWORK_ENGIEN_PUBLIC_URENDERCOMMAND_H

#include "UCommand.h"

BEGIN(Engine)
/*
@ Date: 2024-04-28, Writer: 박태현
@ Explain
-  main Command와는 다른 Sub Command이다.
*/
class  URenderCommand : public UCommand{
public:
	using ALLOCATORARR = ARRAY<ComPtr<Dx12CommandAllocator>, 3>;
public:
	URenderCommand();
	NO_COPY(URenderCommand)
	DESTRUCTOR(URenderCommand)
public:
	virtual void Free() override;
	virtual HRESULT NativeConstruct(CSHPTRREF<UDevice> _spDevice, const ComPtr<Dx12CommandQueue>& _cpCommandQueue) override;
	virtual void Clear() override;
	virtual void WaitForSynchronization() override;
	/* Variable */
private:
	ALLOCATORARR				m_arrRenderAllocators;
	_uint									m_iAllocatorIndex;
};

END

#endif // _PTH_FRAMEWORK_ENGIEN_PUBLIC_URENDERCOMMAND_H
