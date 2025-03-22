#pragma once

namespace Engine {

	template<class T>
	struct SIZESTRUCTINT {
	public:
		static int VALUE_SIZE;
	};

	template<class T>
	int SIZESTRUCTINT<T>::VALUE_SIZE{ sizeof(T) };

	typedef  struct tagGraphicDesc {

		enum WINMODE { MODE_FULL, MODE_WIN, MODE_END };

		HWND hWnd = { NULL };
		HINSTANCE hInst = { NULL };
		unsigned int iWinCX{ 0 }, iWinCY{ 0 };
		WINMODE eWinMode{ MODE_END };
	}GRAPHICDESC;

	typedef struct tagOutputData {
		WKPTR<class UDevice>					wpDevice{};
		WKPTR<class UCommand>			wpGpuCmd{};
	}OUTPUTDATA;

#pragma region INDICES

	typedef struct tagIndices16 {
		_ushort _1{ 0 }, _2{ 0 }, _3{ 0 };
	}INDICIES16;

	typedef struct tagIndices32 {
		_uint _1{ 0 }, _2{ 0 }, _3{ 0 };
	}INDICIES32;

#pragma endregion INDICES

#pragma region VTXINDEX

	typedef struct tagVertexPoint
	{
		_float3 vPosition{ 0.f, 0.f, 0.f };
		_float3 vNormal{ 0.f, 0.f, 0.f };
		_float3 vTagent{ 0.f, 0.f, 0.f };
		_float2 vTexUV{ 0.f, 0.f };
	}VTXPOINT;

	typedef struct tagVertexPoint_Declaration
	{
		static constexpr unsigned int					iNumElement{ 4 };
		static D3D12_INPUT_ELEMENT_DESC		Element[iNumElement];
	}VTXPOINT_DELCARTION;

	// VTXRect
	typedef struct tagVertexDefault
	{
		_float3			vPosition = _float3(0.f, 0.f, 0.f);
		_float2			vTexUV = _float2(0.f, 0.f);
	}VTXDEFAULT;

	typedef struct tagVertexRect_Declaration
	{
		static const unsigned int							iNumElement = 2;
		static D3D12_INPUT_ELEMENT_DESC		Element[iNumElement];
	}VTXDEFAULT_DECLARATION;

	typedef struct tagVertexNormal {

		_float3		vPosition{ 0.f, 0.f, 0.f };
		_float3		vNormal{ 0.f, 0.f, 0.f };
		_float2		vTexUV{ 0.f, 0.f };
	}VTXNORMAL;

	typedef struct tagVertexNormal_Decalaration {
		static const unsigned int							iNumElement = 3;
		static D3D12_INPUT_ELEMENT_DESC		Element[iNumElement];
	}VTXNORMAL_DECLARATION;

#pragma region CUBE
	typedef struct tagVertexCube
	{
		_float3		vPosition{ 0.f, 0.f, 0.f };
		_float2		vTexUV{ 0.f, 0.f};
	}VTXCUBE;

	typedef struct tagVertexCube_Declaration {
		static const unsigned int							iNumElement = 2;
		static D3D12_INPUT_ELEMENT_DESC		Element[iNumElement];
	}VTXCUBE_DECLARATION;

	typedef struct tagVertexNormalCube
	{
		_float3		vPosition{ 0.f, 0.f, 0.f };
		_float3		vNormal{ 0.f, 0.f, 0.f };
		_float2		vTexUV{ 0.f, 0.f };
	}VTXNORCUBE;

	typedef struct tagVertexNormalCube_Declaration {
		static const unsigned int							iNumElement = 3;
		static D3D12_INPUT_ELEMENT_DESC		Element[iNumElement];
	}VTXNORMALCUBE_DECLARATION;

	typedef struct tagVertexSkyBox
	{
		_float3		vPosition{ 0.f, 0.f, 0.f };
	}VTXSKYBOX;

	typedef struct tagVertexSkyBox_Declaration {
		static const unsigned int							iNumElement = 1;
		static D3D12_INPUT_ELEMENT_DESC		Element[iNumElement];
	}VTXSKYBOX_DECLARATION;
#pragma region SHADOW
	typedef struct tagVertexShadow_Declaration {
		static const unsigned int							iNumElement = 1;
		static D3D12_INPUT_ELEMENT_DESC		Element[iNumElement];
	}VTXSHADOWINPUT_DECLARATION;

#pragma endregion SHADOW

#pragma region NORPOS
	typedef struct tagVertexNorPos_Declaration {
		static const unsigned int							iNumElement = 2;
		static D3D12_INPUT_ELEMENT_DESC		Element[iNumElement];
	}VTXNORPOSINPUT_DECLARATION;

#pragma endregion NORPOS

#pragma endregion CUBE

	typedef struct  tagVertexModel
	{
		_float3			vPosition = _float3(0.f, 0.f, 0.f);
		_float3			vNormal = _float3(0.f, 0.f, 0.f);
		_float2			vTexUV[5]{};
		_float3			vTangent = _float3(0.f, 0.f, 0.f);
	}VTXMODEL;

	typedef struct tagVertexModel_Declaration
	{
		static const unsigned int iNumElement = 8;
		static D3D12_INPUT_ELEMENT_DESC  Element[iNumElement];
	}VTXMODEL_DECLARATION;

	typedef struct tagVertexAnimModel
	{
		_float3			vPosition = _float3(0.f, 0.f, 0.f);
		_float3			vNormal = _float3(0.f, 0.f, 0.f);
		_float2			vTexUV[5]{};
		_float3			vTangent = _float3(0.f, 0.f, 0.f);

		_int4				vBlendIndex = _int4(0, 0, 0, 0);
		_float4			vBlendWeight = _float4(0.f, 0.f, 0.f, 0.f);
	}VTXANIMMODEL;

	typedef struct tagVertexAnimModel_Declaration
	{
		static const unsigned int iNumElement = 10;
		static D3D12_INPUT_ELEMENT_DESC  Element[iNumElement];
	}VTXANIMMODEL_DECLARATION;

	typedef struct tagVertexModelColor {

		_float3			vPosition = _float3(0.f, 0.f, 0.f);
		_float3			vNormal = _float3(0.f, 0.f, 0.f);
		_float2			vTexUV[5]{};
		_float3			vTangent = _float3(0.f, 0.f, 0.f);
		_float4			vColor{ 0.f, 0.f, 0.f, 0.f };
	}VTXMODELCOLOR;

	typedef struct tagVertexModelColor_Declaration
	{
		static const unsigned int iNumElement = 9;
		static D3D12_INPUT_ELEMENT_DESC  Element[iNumElement];
	}VTXMODELCOLOR_DECLARATION;

#pragma endregion VTXINDEX

#pragma region VTXINSTANCE

	typedef struct tagVertexIntanceRect {
		_float3			vPosition = _float3(0.f, 0.f, 0.f);
		_float2			vTexUV = _float2(0.f, 0.f);

		_float4			vRight{ 0, 0, 0, 0 };
		_float4			vUp{ 0, 0, 0, 0 };
		_float4			vLook{ 0, 0, 0, 0 };
		_float4			vPos{ 0, 0, 0, 1 };
	}VTXINSTANCERECT;

	typedef struct tagVertexInstanceRect_Declaration
	{
		static const unsigned int							iNumElement = 6;
		static D3D12_INPUT_ELEMENT_DESC		Element[iNumElement];
	}VTXINSTANCERECT_DECLARATION;

#pragma endregion VTXINSTNACE

#pragma region CLONEINFO

	typedef struct tagCloneData {
		_wstring			wstrCloneLayer = L"";
		_ubyte			bCloneType = 0;
	}CLONEDATA;

#pragma endregion CLONEINFO

#pragma region CUSTIMER
	// TODO Shader

	typedef struct  tagCusTimer
	{
		tagCusTimer() = default;
		tagCusTimer(const _float& _fStandardTime)
			: fStandardTime(_fStandardTime)
		{
			fTimer = 0.f;
			isPass = false;
		}
		_bool IsOver(const _double& _dTimeDelta);
		void ResetTimer();
		_bool IsPass() { return isPass; }
		void SetStandardTime(_float _StandardTime) { fStandardTime = _StandardTime; }
		// Timer
		_float		fTimer = 0.f;
		_float		fStandardTime = 0.f;
	private:
		_bool		isPass{ false };
	}CUSTIMER;

#pragma endregion CUSTIMER

#pragma region FONT


#pragma endregion FONT 

#pragma region INSTANCEBUFFER

	struct INSTANCEBUFFERDESC {
		_uint	iInstanceBufferCnt{ 0 };
	};


#pragma endregion INSTANCEBUFFER

#pragma region FILEDATA

	// File Data
	typedef struct  tagFileData
	{
		_wstring											wstrfileName{ L"" };
		_wstring											wstrfilePath{ L"" };
		_ushort											sindex{ 0 };

		WKPTR<struct tagFileGroup>	wpFolder;
	}FILEDATA;

	typedef struct  tagFileGroup
	{
		_wstring																		wstrPath{ L"" };
		_wstring																		wstrFolderName{ L"" };
		UNORMAP<_wstring, SHPTR<tagFileGroup>>	UnderFileGroupList;
		UNORMAP<_wstring, SHPTR<FILEDATA>>			FileDataList;
		SHPTR<tagFileGroup>											spParentsUpper = nullptr;

		void AddFileGroup(const _wstring& _wstrFileName, CSHPTRREF<tagFileGroup> _spFileGroup);
		void AddFileData(const _wstring& _wstrFileName, CSHPTRREF<FILEDATA> _spData);
		void FileAllData(const _wstring& _wstrEct, VECTOR<SHPTR<FILEDATA>>& _lsFileData);

		SHPTR<FILEDATA> FindData(const _wstring& _wstrFileName);
		SHPTR<tagFileGroup> FindGroup(const _wstring& _wstrFileName);
		SHPTR<tagFileGroup> FindUnder(const _wstring& _wstrFileName);
		SHPTR<tagFileGroup> FindUpper(const _wstring& _wstrFileName);

	}FILEGROUP;

#pragma endregion FILEDATA

#pragma region SHADERDESC

	typedef VECTOR<SHADERMAININFO> SHADERLIST;
	typedef struct tagShaderDesc
	{
		// 이름만 있으면 Compute Shader를 빌드한다.
		tagShaderDesc(const _wstring& _wstrShaderName, const D3D_PRIMITIVE_TOPOLOGY
			_eTopology = D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) :
			eType(GRAPHICISCONTROLTYPE::COMPUTE),
			eTopology(_eTopology),
			wstrShaderName(_wstrShaderName), pElement(nullptr), iNumElement(0), ShaderList({ SHADERMAININFO::CS_MAIN }),
			iRenderTargets(1),
			RenderFormats(DXGI_FORMAT_UNKNOWN),
			eRasterRizerType(RASTERIZER_TYPE::CULL_BACK),
			eDepthStencilType(DEPTH_STENCIL_TYPE::LESS),
			eBlendType(BLEND_TYPE::DEFAULT)
		{}

		// 렌더 타겟에 그릴 수 있게 해주는 쉐이더이다. 
		tagShaderDesc(const _wstring& _wstrShaderName,
			D3D12_INPUT_ELEMENT_DESC* _pDesc,
			const _uint& _iNumElement,
			const SHADERLIST& _ShaderList,
			const RENDERFORMATS& _RenderForamts,
			const RASTERIZER_TYPE& _eRasterRizerType = RASTERIZER_TYPE::CULL_BACK,
			const DEPTH_STENCIL_TYPE& _eDepthStencilType = DEPTH_STENCIL_TYPE::LESS,
			const BLEND_TYPE& _eBlendType = BLEND_TYPE::DEFAULT,
			const D3D_PRIMITIVE_TOPOLOGY _eTopology =
			D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
			:
			eType(GRAPHICISCONTROLTYPE::GRAPHICS),
			eTopology(_eTopology),
			wstrShaderName(_wstrShaderName), pElement(_pDesc), iNumElement(_iNumElement), ShaderList(_ShaderList),
			iRenderTargets((_uint)_RenderForamts.size()),
			RenderFormats(_RenderForamts),
			eRasterRizerType(_eRasterRizerType),
			eDepthStencilType(_eDepthStencilType),
			eBlendType(_eBlendType)
		{}
		// 일반 화면에 그리는 쉐이더 1개의 렌더 타겟에 Format만 맞으면 그릴 수 있따. 
		tagShaderDesc(const _wstring& _wstrShaderName,
			D3D12_INPUT_ELEMENT_DESC* _pDesc,
			const _uint& _iNumElement,
			const SHADERLIST& _ShaderList,
			const RASTERIZER_TYPE& _eRasterRizerType = RASTERIZER_TYPE::CULL_BACK,
			const DEPTH_STENCIL_TYPE& _eDepthStencilType = DEPTH_STENCIL_TYPE::LESS,
			const BLEND_TYPE& _eBlendType = BLEND_TYPE::DEFAULT,
			const D3D_PRIMITIVE_TOPOLOGY _eTopology =
			D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
			:
			eType(GRAPHICISCONTROLTYPE::GRAPHICS),
			eTopology(_eTopology),
			wstrShaderName(_wstrShaderName), pElement(_pDesc), iNumElement(_iNumElement), ShaderList(_ShaderList),
			RenderFormats(RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM }),
			iRenderTargets(1),
			eRasterRizerType(_eRasterRizerType),
			eDepthStencilType(_eDepthStencilType),
			eBlendType(_eBlendType)
		{}

		GRAPHICISCONTROLTYPE						eType;
		_wstring														wstrShaderName;
		D3D12_INPUT_ELEMENT_DESC*			pElement;
		_uint															iNumElement;
		SHADERLIST												ShaderList;
		_uint															iRenderTargets;
		RENDERFORMATS									RenderFormats;
		RASTERIZER_TYPE									eRasterRizerType;
		DEPTH_STENCIL_TYPE							eDepthStencilType;
		BLEND_TYPE												eBlendType;
		D3D_PRIMITIVE_TOPOLOGY					eTopology;
	}SHADERDESC;

	typedef struct tagShaderConstantBufferDesc
	{
		tagShaderConstantBufferDesc() = default;
		tagShaderConstantBufferDesc(const CBV_REGISTER& _eReg, const _uint& _iSize, const _uint& _iCnt)
			: eCbvRegister(_eReg), iSize(_iSize), iCount(_iCnt) {}

		CBV_REGISTER		eCbvRegister = CBV_REGISTER::CBV_END;
		_uint						iSize = 0;
		_uint						iCount = 0;
	}SHADERCBDESC;

#pragma endregion SHADERDESC

#pragma region RENDERTARGET

	typedef struct tagTextureTypeDesc
	{
		tagTextureTypeDesc(const DXGI_FORMAT& _eFormat, const _uint& _iWidth, const _uint& _iHeight,
			const _float4& _vClear, const D3D12_RESOURCE_FLAGS& _eResourceFlag)
			: eDxFormat(_eFormat), iWidth(_iWidth), iHeight(_iHeight), vClear(_vClear),
			eHeapFlags(D3D12_HEAP_FLAG_NONE), eResourceFlags(_eResourceFlag)
		{
			HeapProPerty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		}
		tagTextureTypeDesc(const DXGI_FORMAT& _eFormat, const _uint& _iWidth, const _uint& _iHeight, const _float4& _vClear,
			const D3D12_HEAP_TYPE& _eHeapType = D3D12_HEAP_TYPE_DEFAULT,
			const D3D12_HEAP_FLAGS& _eHeapFlags = D3D12_HEAP_FLAG_NONE,
			const D3D12_RESOURCE_FLAGS& _ResourceFlags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
			: eDxFormat(_eFormat), iWidth(_iWidth), iHeight(_iHeight),
			eHeapFlags(_eHeapFlags), eResourceFlags(_ResourceFlags), vClear(_vClear)
		{
			HeapProPerty = CD3DX12_HEAP_PROPERTIES(_eHeapType);
		}
		DXGI_FORMAT									eDxFormat;
		_uint													iWidth;
		_uint													iHeight;
		CD3DX12_HEAP_PROPERTIES		HeapProPerty;
		D3D12_HEAP_FLAGS						eHeapFlags;
		D3D12_RESOURCE_FLAGS			eResourceFlags;
		_float4												vClear;
	}TEXTURETYPEDESC;

	typedef struct tagRenderTargetDesc : public tagTextureTypeDesc
	{
	public:
		tagRenderTargetDesc() :
			eRTObjID(RTOBJID::RTOBJ_END),
			tagTextureTypeDesc(DXGI_FORMAT_UNKNOWN, 0, 0, _float4{}, D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_NONE) {}
		tagRenderTargetDesc(const RTOBJID& _eRTObjID, const DXGI_FORMAT& _eFormat, const _uint& _iWidth, const _uint& _iHeight, const _float4& _vClear,
			const D3D12_HEAP_TYPE& _eHeapType = D3D12_HEAP_TYPE_DEFAULT,
			const D3D12_HEAP_FLAGS& _eHeapFlags = D3D12_HEAP_FLAG_NONE,
			const D3D12_RESOURCE_FLAGS& _ResourceFlags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
			: eRTObjID(_eRTObjID), tagTextureTypeDesc(_eFormat, _iWidth, _iHeight, _vClear, _eHeapType, _eHeapFlags, _ResourceFlags)
		{}
		RTOBJID	 eRTObjID;
	}RTDESC;

#pragma endregion RENDERTARGET

#pragma region SHADOW
	
#pragma endregion SHADOW


#pragma region MODEL

	/*
	@ Date: 2024-04-10, Writer: 박태현
	@ Explain
	-  머터리얼 값들을 추출하기 위한 구조체이다. 
	*/
	struct MODELMATERIALINFO {
		_float4			vDiffuse;
		MODELMATERIALINFO() : vDiffuse{}
		{}
	};

	// Key Frame
	typedef struct tagKeyFrame
	{
		_double		dTime{ 0.0 };

		_float3	vScale;
		_float4	vRotation;
		_float4	vPosition;
	}KEYFRAME;

	// BuffDesc
	typedef struct tagBufferDesc
	{
		_uint										iNumVertices = 0;
		_uint										iNumIndices = 0;
		_wstring									wstrName;
		VECTOR<_float2>				UVTexCoords;
		const _float3*						pPosition{ nullptr };
		const INDICIES32*				pIndices = nullptr;
		_uint										iMatrialIndex = 0;
		_uint										iNumBones = 0;
		VECTOR<_wstring>				BoneNodeNameList;
		_uint										iBoneNodeCnt = 0;

		_float3									vMinVertex{};
		_float3									vMaxVertex{};
	}BUFFDESC;
	// Channel Desc 
	typedef struct tagChannelDesc
	{
		_wstring				wstrBoneName = L"";
		_uint					iNumMaxKeyFrames = 0;
		KEYFRAME*		pKeyFrames{ nullptr };
	}CHANNELDESC;

	typedef struct tagAnimExtraData {

		_uint												iNumChannels = 0;
		_double											dTickPerSeconds = 0.0;
		_double											dDuration = 0.0;
	}ANIMEXTRADATA;

	// Animation
	typedef struct tagAnimDesc
	{
		_wstring												wstrName = L"";
		VECTOR<CHANNELDESC>				Channels;

		ANIMEXTRADATA								stExtraData;
	}ANIMDESC;
	// HiearchyNode
	typedef struct tagBoneNodeDesc
	{
		_float4x4	mTransformMatrix{ _float4x4::Identity };
		_float4x4	mOffsetMatrix{ _float4x4::Identity };
		_float			fDepth{ 0 };
		_wstring		wstrName = L"";
		_wstring		wstrParentsName = L"";
	}BONENODEDESC;
	// Mesh
	typedef struct tagMeshDesc : public BUFFDESC
	{
		VTXMODEL* pVtxModel = nullptr;
	}MESHDESC;
	// Animation Mesh 
	typedef struct tagAnimMeshDesc : public BUFFDESC
	{
		VTXANIMMODEL* pVtxModel = nullptr;
	}ANIMMESHDESC;
	// Common Model Data
	typedef struct tagCommonModelData
	{
		MODELMATERIALINFO												 ModelMaterialInfo;
		UNORMAP<_uint, VECTOR<_wstring>>	 MatrialData;
		_uint																 iMeshContainersCount = 0;
		_uint																 iMatrialCount = 0;
		_uint																 iNodeCnt = 0;
																				 
		VECTOR<BONENODEDESC>						 BNodeDatas;
	}COMMODELDATA;
	// Model Desc
	typedef struct tagModelDesc : public COMMODELDATA
	{
		VECTOR<MESHDESC>			Meshes;
	}MODELDESC;
	// Model Desc
	typedef struct tagAnimModelDesc : public COMMODELDATA
	{
		VECTOR<ANIMMESHDESC>	Meshes;
		VECTOR<ANIMDESC>				Animes;
		_uint											iAnimCount{ 0 };
	}ANIMMODELDESC;

	// ModelMaterial
	typedef struct tagModelMatrial
	{
		// Texture
		ARRAY<SHPTR<class UTexGroup>, MAX_ASSIMP_MATERIAL_TEXTURE_CNT>  arrMaterialTexture{ nullptr };
	}MODELMATRIALDESC;

#pragma endregion MODEL


#pragma region ANIMMATIONFASTSECTION 

	typedef struct tagAnimFastSection
	{
		_float fStartSpot = -1.f;
		_float fEndSpot = -1.f;
		_float fFastValue = 1.f;

		void Convert(_double& _dValue, const _double& _dTimeAcc);
		_bool IsPass(const _double& _dTimeValue);
	}ANIMFASTSECTION;

#pragma endregion ANIMATIONFASTSECTION

#pragma region SOUND 

	struct SOUNDDESC {
		SOUNDTYPE			SoundType;
		_float						fVolume;

		SOUNDDESC() : SoundType{ SOUNDTYPE::SOUND_END }, fVolume{ 0.f } { }
		SOUNDDESC(SOUNDTYPE _SoundType, _float _fVolume) : SoundType{ _SoundType },
			fVolume{ _fVolume } {}
	};

#pragma endregion SOUND

#pragma region ANIMEVENTTYPE


	/*
	@ Date: 2024-02-10, Writer: 박태현
	@ Explain
	- 애니메이션 이벤트 구조체의 상위 구조체
	*/
	struct ANIMEVENTDESC abstract {
	public:
		_bool							isActiveEvent;
		_wstring						wstrEventTrigger;
		_bool							isAnimChangeActive;

		ANIMEVENTDESC() : wstrEventTrigger{ L"" }, isActiveEvent{ false }, isAnimChangeActive { false }
		{ wstrEventTrigger.resize(MAX_BUFFER_LENGTH); }
	};
	/*
	@ Date: 2024-02-10, Writer: 박태현
	@ Explain
	- 애니메이션 Event를 위한 구간을 정의하는 구조체
	- 애니메이션 Event와 ChangeDesc를 상속 받아서 그 값들을 사용한다.
	*/
	struct ANIMEVENTSECTIONDESC : public ANIMEVENTDESC {
		_double dStartTime;
		_double dEndTime;
		_double dStopTime;

		ANIMEVENTSECTIONDESC() : dStartTime{ 0.0 }, dEndTime{ 0.0 }, dStopTime{ 0.0 }  {}

		_bool IsAnimEventActive(const _double& _dTimeAcc) const;
	};
	/*
	@ Date: 2024-02-10, Writer: 박태현
	@ Explain
	- 애니메이션 Event가 발생된 지점을 정의하는 구조체
	- 애니메이션 Event와 ChangeDesc를 상속 받아서 그 값들을 사용한다.
	*/
	struct ANIMOCURRESDESC : public ANIMEVENTDESC {
		_double		dAnimOccursTime;

		ANIMOCURRESDESC() : dAnimOccursTime{ 0.0 } {}

		bool IsAnimOcurrs(const _double& _dTimeAcc) const;
	};
	/*
	@ Date: 2024-02-10, Writer: 박태현
	@ Explain
	-  애니메이션의 Event Desc 말고 각자의 Other Event를 담고 있는 구조체로 
		해당 구조체를 정의함으로써, 다른 POINTER들이 개입할 여지를 없애기 위해서 정의함
	*/
	struct ANIMOTHEREVENTDESC abstract{ };

	/*
	@ Date: 2024-02-10, Writer: 박태현
	@ Explain
	-  애니메이션이 다음으로 넘어가기 위한 구조체
	*/
	struct ANIMCHANGEDESC : public ANIMOTHEREVENTDESC {
		_int				iNextAnimIndex;
		_float			fSupplyAnimValue;
		_double		dNextAnimTimeAcc;
		_float			fAnimChangeTime;
		_float			fLastProgressValue;
		_bool			isEnableLastSettingAnim;

		ANIMCHANGEDESC() : iNextAnimIndex{ 0 }, fSupplyAnimValue{ 1.f }, dNextAnimTimeAcc{ 0.0 },
			fAnimChangeTime{ 0 }, fLastProgressValue{ 1.f }, isEnableLastSettingAnim{ false }
		{}
		ANIMCHANGEDESC(const _int _NextAnimIndex, const _float& _SupplyAnimValue, const _double& _dNextAnimTimeAcc) :
			iNextAnimIndex{ _NextAnimIndex }, fSupplyAnimValue{ _SupplyAnimValue }, dNextAnimTimeAcc{ _dNextAnimTimeAcc } ,
			fAnimChangeTime{ 0 }, fLastProgressValue{ 1.f }, isEnableLastSettingAnim{ false }
		{}
	};
	/*
	@ Date: 2024-02-21, Writer: 박태현
	@ Explain
	-  애니메이션 특정 구역에 Collider를 붙이기 위한 구조체이다. 
	*/
	struct ANIMCOLLIDERDESC : public ANIMOTHEREVENTDESC {
		_int												iColliderType;

		SHPTR<class UCollider>			spCollider;
		SHPTR<class UBoneNode>	spBoneNode;

		_float3										vColliderScale;
		_float3										vColliderTranslation;

		ANIMCOLLIDERDESC() :  iColliderType{ 0 },
			spCollider{ nullptr }, spBoneNode{ nullptr }, vColliderScale{ 1, 1, 1}, vColliderTranslation{ 1, 1, 1 } {}
		ANIMCOLLIDERDESC(const _int _iColliderType, const _float3& _vColliderScale, const _float3& _vColliderTranslation) : 
			iColliderType{_iColliderType}, vColliderScale{_vColliderScale}, vColliderTranslation{_vColliderTranslation}, spBoneNode{nullptr}, spCollider{nullptr}
		{}
	};
	/*
	@ Date: 2024-02-21, Writer: 박태현
	@ Explain
	-  애니메이션 특정 구역에 Collider를 붙이기 위한 구조체이다.
	*/
	struct ANIMSOUNDDESC : public ANIMOTHEREVENTDESC {
		_wstring			wstrSoundName;
		_float3				vSoundVelocity;
		_float					fMinSoundDistance;
		_float					fMaxSoundDistance;
		_float					fSoundVolume;

		ANIMSOUNDDESC() : wstrSoundName{ L"" }, vSoundVelocity{}, fMinSoundDistance{ 0.f }, fMaxSoundDistance{ 1.f }, 
			fSoundVolume{ 0.5f } {}

	};
#pragma endregion ANIMEVENTTYPE


#pragma region EQUIPMENTINFO

	struct ITEMINFO
	{
		// 지불해야할 돈
		_float		fPayGold;

		ITEMINFO() : fPayGold{0} {}
		ITEMINFO(const _float _fPayGold) : fPayGold{ _fPayGold }{}
	};

	struct EQUIPMENTINFO : public ITEMINFO
	{
		_float fIncreasePower;
		_float fIncreaseDefensive;
		_float fIncreaseHp ;

		EQUIPMENTINFO() : fIncreasePower{ 0 }, fIncreaseDefensive{ 0 }, fIncreaseHp{ 0 } { }
		EQUIPMENTINFO(const _float _fPayGold, const _float _fIncreasePower, const _float _fIncreaseDefensive, const _float _fIncreaseHp)
			: ITEMINFO(_fPayGold), fIncreasePower{_fIncreasePower}, fIncreaseDefensive{_fIncreaseDefensive}, fIncreaseHp{_fIncreaseHp}
		{}
	};
#pragma endregion EQUIPMENTINFO


#pragma region FONT 

	typedef struct tagFontDesc {

		tagFontDesc() = default;
		tagFontDesc(const _wstring& _wstrText, const _float2& _vPos, const _float4& _vColor = { 255.f, 255.f, 255.f, 255.f },
			const _float _fRotation = 0.f, const _float2& _vScale = {}, const _float _fDepths = 0.f, const _float2& _vOrigin = {},
			const DirectX::DX12::SpriteEffects& _eSpriteEffects = DirectX::DX12::SpriteEffects_None) :
			wstrText{ _wstrText },
			vPos{ _vPos },
			vColor{ _vColor },
			fRotation{ _fRotation },
			vOrigin{ _vOrigin },
			vScale{ _vScale },
			eSpriteEffects{ _eSpriteEffects },
			fDepths{ _fDepths }
		{}

		_wstring										wstrText{};
		_float2											vPos{};
		_float4											vColor{ 1.f, 1.f, 1.f, 1.f };
		_float												fRotation{ 0.f };
		_float2											vOrigin{};
		_float2											vScale{ 1.f, 1.f };
		DirectX::DX12::SpriteEffects	eSpriteEffects;
		_float												fDepths{ 1.f };
	}FONTDESC;

#pragma endregion FONT 


#pragma region NETWORKDATA 
	/*
	NETWORKDATA
	*/
	struct NETWORKRECEIVEINITDATA
	{
		_int								iNetworkID;
		_int								iCellIndex;
		_int								iType;
		_int								iAnimIndex;
		_float3						vPos;
		_float3						vRotate;
		_float3						vScale;

		NETWORKRECEIVEINITDATA() : iNetworkID{ 0 }, iCellIndex{ 0 }, iType{ 0 }, iAnimIndex{ 0 }, vPos{}, vRotate {}, vScale{} {}
		NETWORKRECEIVEINITDATA(const _int _iNetworkID, const _int _iCellIndex, const _int _iType, const _int _iAnimIndex = 0,
			const _float3& _vPos = {}, const _float3& _vRotate = {}, const _float3& _vScale = {}) :
			iNetworkID{ _iNetworkID }, iCellIndex{ _iCellIndex }, iType{ _iType }, iAnimIndex{ _iAnimIndex }, vPos{ _vPos },
			vRotate {	_vRotate}, vScale{ _vScale } {}
	};

	struct MOBSERVERDATA
	{
		_int					iStartAnimIndex;
		_int					iMobType;
		_float4x4		mWorldMatrix;
		_int					iMobID;
		_int					iMobParentsID;
		MOBSERVERDATA() : iStartAnimIndex{ 0 }, iMobType{ 0 }, mWorldMatrix{ _float4x4::Identity }, iMobID{ 0 }, iMobParentsID{ 0 } {}
	};

#pragma endregion NETWORKDATA
}
