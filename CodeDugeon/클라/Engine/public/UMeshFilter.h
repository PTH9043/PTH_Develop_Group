#pragma once
#include "UController.h"


BEGIN(Engine)
class UMeshContainer;
using MESHCONTAINERS = VECTOR<SHPTR<UMeshContainer>>;
/*
@ Date: 2024-05-21, Writer: 박태현
@ Explain
-   메쉬의 보여줌을 관장하는 컨트롤러
*/
class UMeshFilter : public UController {
	struct MESHFILTERINFO
	{
		_bool isMeshFilterEnable;
		_int	   iCurMeshIndex;
		_int	   iMeshGroupIndex;

		MESHFILTERINFO() : isMeshFilterEnable{ true }, iCurMeshIndex{ 0 }, iMeshGroupIndex { 0 } {}
	};
public:
	using MESHFILTERINFOCONTAINER = VECTOR<MESHFILTERINFO>;
	struct DESC{
		VECTOR<SHPTR<UMeshContainer>> MeshContainer;
	};
public:
	UMeshFilter(CSHPTRREF<UDevice> _spDevice);
	UMeshFilter(const UMeshFilter& _rhs);
	virtual ~UMeshFilter() = default;
public:
	CLONE_MACRO(UMeshFilter, "UMeshShowController::Clone To Failed")
public:
	// Native Construct 
	virtual HRESULT NativeConstruct() override;
	HRESULT NativeConstruct(const _wstring& _wstrPath, const MESHCONTAINERS& _meshContainer);
	// Clone 
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _tDatas) override;
	// Tick
	virtual void Tick(const _double& _dTimeDelta);
	_bool IsShowEnable(const _int _iMeshIndex);

	void GroupEnable(const _int _iGroupIndex);
	void GroupDisable(const _int _iGroupIndex);

	_bool Load(const _wstring& _wstrPath);
	void Save(const _wstring& _wstrPath);
public: /* get set */
	MESHFILTERINFOCONTAINER& GetMeshFilterInfoContainer() { return m_MeshFilterInfoContainer; }
private:
	virtual void Free() override;
private:
	MESHFILTERINFOCONTAINER			m_MeshFilterInfoContainer;
};


END
