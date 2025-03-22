#pragma once

namespace Engine
{
	typedef struct tagGraphicDesc
	{
		enum WINMODE { MODE_FULL, MODE_WIN, MODE_END };

		HWND hWnd;
		unsigned int iWinCX;
		unsigned int iWinCY;
		WINMODE  eWindowed = MODE_END;
	}GRAPHICDESC;

#pragma region INDICES

	typedef struct tagFaceIndices16
	{
		unsigned short		_1, _2, _3;
	}FACEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long		_1, _2, _3;
	}FACEINDICES32;

	typedef struct tagLineIndices16
	{
		unsigned short		_1, _2;
	}LINEINDICES16;

	typedef struct tagLineIndices32
	{
		unsigned long		_1, _2;
	}LINEINDICES32;

#pragma endregion INDICIES

#pragma region VERTEX_DEFINE

	typedef struct tagVertexTexture
	{
		XMFLOAT3			vPosition;
		XMFLOAT2			vTexUV;
	}VTXTEX;

	typedef struct ENGINE_DLL tagVertexTexture_Declaration
	{
		static const unsigned int iNumElement = 2;
		static D3D11_INPUT_ELEMENT_DESC  Element[iNumElement];
	}VTXTEX_DECLARATION;

	typedef struct tagVertexNormalTexture
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexUV;
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertexNormalTexture_Declaration
	{
		static const unsigned int iNumElement = 3;
		static D3D11_INPUT_ELEMENT_DESC  Element[iNumElement];
	}VTXNORTEX_DECLARATION;

	typedef struct tagVertexColor
	{
		XMFLOAT3			vPosition;
		XMFLOAT4			vColor;
	}VTXCOL;

	typedef struct ENGINE_DLL tagVertexColor_Declaration
	{
		static const unsigned int iNumElement = 2;
		static D3D11_INPUT_ELEMENT_DESC  Element[iNumElement];
	}VTXCOL_DECLARATION;

	typedef struct tagVertexCubeTexture
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vUvPosition;
	}VTXCUBETEX;

	typedef struct ENGINE_DLL tagVertexCubeTexture_Declaration
	{
		static const unsigned int iNumElement = 2;
		static D3D11_INPUT_ELEMENT_DESC  Element[iNumElement];
	}VTXCUBETEX_DECLARATION;

	typedef struct  tagVertexModel
	{
		XMFLOAT3			vPosition = XMFLOAT3(0.f, 0.f, 0.f);
		XMFLOAT3			vNormal = XMFLOAT3(0.f, 0.f, 0.f);
		XMFLOAT2			vTexUV0 = XMFLOAT2(0.f, 0.f);
		XMFLOAT3			vTangent = XMFLOAT3(0.f, 0.f, 0.f);
	}VTXMODEL;

	typedef struct ENGINE_DLL tagVertexModel_Declaration
	{
		static const unsigned int iNumElement = 4;
		static D3D11_INPUT_ELEMENT_DESC  Element[iNumElement];
	}VTXMODEL_DECLARATION;

	typedef struct tagVertexAnimModel
	{
		XMFLOAT3			vPosition = XMFLOAT3(0.f, 0.f, 0.f);
		XMFLOAT3			vNormal = XMFLOAT3(0.f, 0.f, 0.f);
		XMFLOAT2			vTexUV0 = XMFLOAT2(0.f, 0.f);
		XMFLOAT2			vTexUV1 = XMFLOAT2(0.f, 0.f);
		XMFLOAT2			vTexUV2 = XMFLOAT2(0.f, 0.f);
		XMFLOAT2			vTexUV3 = XMFLOAT2(0.f, 0.f);
		XMFLOAT2			vTexUV4 = XMFLOAT2(0.f, 0.f);
		XMFLOAT3			vTangent = XMFLOAT3(0.f, 0.f, 0.f);

		XMUINT4				vBlendIndex = XMUINT4(0, 0, 0, 0);
		XMFLOAT4			vBlendWeight = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	}VTXANIMMODEL;

	typedef struct ENGINE_DLL tagVertexAnimModel_Declaration
	{
		static const unsigned int iNumElement = 10;
		static D3D11_INPUT_ELEMENT_DESC  Element[iNumElement];
	}VTXANIMMODEL_DECLARATION;


	typedef struct tagVertexPoint
	{
		XMFLOAT3			vPosition;
		XMFLOAT2			vPSize;
	}VTXPOINT;

	typedef struct tagVertexEffectInstance
	{
		XMFLOAT4	vRight;
		XMFLOAT4	vUp;
		XMFLOAT4	vLook;
		XMFLOAT4	vTranslation;
	}VTXEFFECTINSTANCE;

	typedef struct ENGINE_DLL tagVertexEffectInstance_Delearation
	{
		static const unsigned int iNumElement = 6;
		static D3D11_INPUT_ELEMENT_DESC Element[iNumElement];
	}VTXEFFECTINSTANCE_DECLARATION;

	typedef struct ENGINE_DLL tagVertexRectInstance_Delearation
	{
		static const unsigned int iNumElement = 6;
		static D3D11_INPUT_ELEMENT_DESC Element[iNumElement];
	}VTXRECTINSTANCE_DECLARATION;

	typedef struct  tagVertexModel_Instance
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;
	}VTXMODEL_INSTANCE;

	typedef struct ENGINE_DLL tagVertexModel_Instance_Declaration
	{
		static const unsigned int iNumElement = 8;
		static D3D11_INPUT_ELEMENT_DESC  Element[iNumElement];
	}VTXMODEL_INSTNACE_DECLARATION;

	typedef struct tagVertexMatrix
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vPos;
	}VERTEXMATRIX;

	typedef struct tagVertexAnimModel_Instance
	{
		XMFLOAT4										vRight;
		XMFLOAT4										vUp;
		XMFLOAT4										vLook;
		XMFLOAT4										vTranslation;
		_uint													iGroupID = 0;
	}VTXANIMMODEL_INSTANCE;

	typedef struct ENGINE_DLL tagVertexAnimModel_Instance_Declaration
	{
		static const unsigned int iNumElement = 15;
		static D3D11_INPUT_ELEMENT_DESC  Element[iNumElement];
	}VTXANIMMODEL_INSTANCE_DECLARATION;

#pragma endregion VERTEX_DEFINE

	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_SPOT, TYPE_END };

		TYPE			eLightType;
		_float3		vDirection;
		_float3		vPosition;
		_float			fRange;

		_float4		vDiffuse;
		_float4		vAmbient;
		_float4		vSpecular;
	}LIGHTDESC;

	// File Data
	typedef struct tagFileData
	{
		_wstring	strFileName;
		_wstring	strFilePath;
		_uint			iIndex;
		void* pFolderAddress = nullptr;
	}FILEDATA;

	typedef struct tagFileGroup
	{
		_wstring								strUppderName;
		_wstring								strPath;
		list<tagFileGroup*>			lsUnderList;
		vector<FILEDATA>				vecFileDataList;
		tagFileGroup* pParentsUpper = nullptr;

		FILEDATA* FindData(const _wstring& _strFileName)
		{
			for (auto& iter : vecFileDataList)
				if (!lstrcmp(iter.strFileName.c_str(), _strFileName.c_str()))
					return &iter;

			return nullptr;
		}

		tagFileGroup* Find_Group(const _tchar* _szFileName)
		{
			tagFileGroup* pFileGroup = nullptr;

			for (auto& iter : lsUnderList)
			{
				if (!lstrcmp(iter->strUppderName.c_str(), _szFileName))
				{
					pFileGroup = iter;
					break;
				}
			}
			return pFileGroup;
		}

		tagFileGroup* Find_Group_Under(const _tchar* _szFileName)
		{
			tagFileGroup* pFileGroup = nullptr;

			for (auto& iter : lsUnderList)
			{
				if (!lstrcmp(iter->strUppderName.c_str(), _szFileName))
				{
					pFileGroup = iter;
					break;
				}
				else
				{
					iter->Find_Group_Under(_szFileName);
				}
			}
			return pFileGroup;
		}

		tagFileGroup* Find_Group_Upper(const _tchar* _szFileName)
		{
			tagFileGroup* pFileGroup = nullptr;
			if (nullptr != pParentsUpper)
			{
				if (!lstrcmp(pParentsUpper->strUppderName.c_str(), _szFileName))
				{
					pFileGroup = pParentsUpper;
				}
				else
				{
					if (nullptr != pParentsUpper)
						pParentsUpper->Find_Group_Upper(_szFileName);
				}
				return pFileGroup;
			}
		}

	}FILEGROUP;

	// Key Frame
	typedef struct tagKeyFrame
	{
		double		Time;

		XMFLOAT3	vScale;
		XMFLOAT4	vRotation;
		XMFLOAT4	vPosition;
	}KEYFRAME;

	// BuffDesc
	typedef struct tagBufferDesc
	{
		_uint						iNumVertices = 0;
		_uint						iNumIndices = 0;
		_float3* pVerticesPos;
		_wstring				strName;
		vector<_float2>	UVTexCoords;
	}BUFFDESC;
	// Channel Desc 
	typedef struct tagChannelDesc
	{
		_wstring						strName = L"";
		_uint								iNumMaxKeyFrames = 0;
		KEYFRAME* pKeyFrames;
		// Save Data
		void Save_Data(const HANDLE& _hFile, DWORD& _dwByte);
		// Load Data
		void Load_Data(const HANDLE& _hFile, DWORD& _dwByte);
	}CHANNELDESC;
	// Animation
	typedef struct tagAnimDesc
	{
		_wstring									strName = L"";
		_uint											iNumChannels = 0;
		vector<CHANNELDESC>		vecChannels;
		_double										dTickPerSeconds = 0.0;
		_double										dDuration = 0.0;
		// Save Data
		void Save_Data(const HANDLE& _hFile, DWORD& _dwByte);
		// Load Data
		void Load_Data(const HANDLE& _hFile, DWORD& _dwByte);
	}ANIMDESC;
	// HiearchyNode
	typedef struct tagHierachyNodeDesc
	{
		_float4x4		TransformMatrix;
		_float4x4		OffsetMatrix;
		_float				fDepth;
		_wstring		strName = L"";
		_wstring		strParentsName = L"";
	}HNODEDESC;
	// Mesh
	typedef struct tagMeshDesc : public BUFFDESC
	{
		VTXMODEL* pVtxModel = nullptr;
		FACEINDICES32* pIndices = nullptr;
		_uint								iMatrialIndex = 0;
		vector<_wstring>		HierachyNameList;
		_uint								iHierachyCount = 0;
	}MESHDESC;
	// Animation Mesh 
	typedef struct tagAnimMeshDesc : public BUFFDESC
	{
		VTXANIMMODEL* pVtxModel = nullptr;
		FACEINDICES32* pIndices = nullptr;
		_uint								iMatrialIndex = 0;
		_uint								iNumBones = 0;
		vector<_wstring>		HierachyNameList;
		_uint								iHierachyCount = 0;
	}ANIMMESHDESC;
	// Common Model Data
	typedef struct tagCommonModelData
	{
		map<_uint, vector<_wstring>>			MatrialData;
		_uint															iMeshContainersCount = 0;
		_uint															iMatrialCount = 0;
		_uint															iHierachyCount = 0;
		FILEGROUP* pFileGroup = nullptr;
		FILEDATA* pFileData = nullptr;

		vector<HNODEDESC>							HNodeDataList;
	}COMMODELDATA;
	// Model Desc
	typedef struct tagModelDesc : public COMMODELDATA
	{
		vector<MESHDESC>							MeshList;
	}MODELDESC;
	// Model Desc
	typedef struct tagAnimModelDesc : public COMMODELDATA
	{
		vector<ANIMMESHDESC>				MeshList;
		vector<ANIMDESC>							AnimList;
		_uint														iAnimCount;
	}ANIMMODELDESC;
	// ModelMaterial
	typedef struct tagModelMatrial
	{
		class CTexture* pMaterialTexture[TEXTYPE::TextureType_UNKNOWN] = { nullptr };
	}MODELMATRIALDESC;
	// Coll Data
	typedef struct tagCollData
	{
		_float3						vCollScale = _float3(0.f, 0.f, 0.f);
		_float3						vTranslation = _float3(0.f, 0.f, 0.f);
	}COLLDATA;

	typedef struct tagCUSRGB
	{
		_double r = 0.f;
		_double g = 0.f;
		_double b = 0.f;
		_double a = 0.f;
	}CUSRGB;

	typedef struct tagEffectDesc
	{
		enum AxisType { Axis_X, Axis_Y, Axis_Z, Axis_ALL };

		AxisType eScaleAxisType = Axis_X;
		AxisType eRotAxisType = Axis_X;

		_bool bBillBoardAll = false;

		_bool bGravity = false;
		_bool bLoop = false;
		_bool bJump = false;
		_bool bSliceLoop = false;
		_bool bCutBlack = false;
		_bool bNoise = false;
		_bool bPlay = false;
		_bool bUVFlow = false;
		_bool bUVLoop = false;


		_float4x4 matParent; // 현재 사용안함

		_float3 t1;
		_float3 t2;
		_float t3;
		_int iMaskIndex;
		_bool b1;
		_bool b2;
		_bool b3;
		_bool bEnableMask;
		_float3 vScrollSpeedsX = _float3(0.f, 0.f, 0.f);

		_bool bGoLook = false;
		_bool bGoUp = false;
		_bool bGoRight = false;
		_bool bTemp3 = false;

		_int iSamplerMode = 0;
		_float2 vUV_Mul = _float2(1.f, 1.f);

		_float3 vScrollSpeeds = _float3(1.3f, 10.f, 2.3f);
		_float3 vNoiseScale = _float3(1.f, 2.f, 3.f);
		_float2 vDistortion1 = _float2(0.1f, 0.2f);
		_float2 vDistortion2 = _float2(0.1f, 0.3f);
		_float2 vDistortion3 = _float2(0.1f, 0.1f);
		_float fDistortionScale = 0.8f;
		_float fDistortionBias = 0.5f;
		_float fGlowValue = 0.f;

		_bool bBillBoardExceptY = false;
		_bool bGlow = false;
		_bool bPause = false;
		_bool bOrthogonal = false;

		_float3 vPos = _float3(0.f, 0.f, 0.f);
		_float3 vRotDelta = _float3(0.f, 0.f, 0.f);
		_float3 vRot = _float3(0.f, 0.f, 0.f);
		_float3 vScl = _float3(1.f, 1.f, 1.f);
		_float3 vScaleDelta = _float3(0.f, 0.f, 0.f); // 현재 사용안함
		_float3 vDir = _float3(0.f, 0.f, 0.f);
		_float4 vColor = _float4(0.f, 0.f, 0.f, 0.f);
		_float2 vSliceUV = _float2(0.f, 0.f);
		_float2 vUVSpeed = _float2(0.f, 0.f);

		_float fForce = 0.f;
		_float fMaxFrame = 0.f;
		_float fCurFrame = 0.f;
		_float fDelayTime = 0.f;
		_float fGravity = 0.f;
		_float fAcceleration = 0.f;
		_float fAccMaxTime = 0.f;
		_float fAccSpeed = 0.f;
		_float fRotSpeed = 0.f;
		_float fJumpPower = 0.f;
		_float fJumpTime = 0.f;
		_float fAngle = 0.f;
		_float fDeltaScaleRatio = 0.f;
		_float fDeltaAngleRatio = 0.f;
		_float fDeltaPosRatio = 0.f;
		_float fDestScale = 0.f;
		_float fDestAngle = 0.f;
		_float fDestPos = 0.f;
		_float fSliceSpeed = 0.f;
		_float fEnableTime = 0.f;
		_float fLifeTime = 0.f;
		_float fAlphaTime = 0.f;
		_float fAlphaSpeed = 1.f;

		_int iSliceX = 0;
		_int iSliceY = 0;
		_int iInstanceCount = 0;
		_int iPassIndex = 0;
		_int iRenderID = 0;
		_int iMeshIndex = 0; // 현재 사용안함


	}EFFECTDESC;

	typedef struct tagDEBUGRemainDesc
	{
		class CBase* pInstance = nullptr;
		_ulong* pRefCount = nullptr;
		_string				strExplain = "";
	}DEBUGREMAINDESC;

	typedef struct tagInstancetDesc2
	{
		enum INSTANCETYPE { INSTANCE_CURVELINE, INSTANCE_ANGLEPIECE, INSTANCE_RANDOM };
		enum AxisType { Axis_X, Axis_Y, Axis_Z };

		INSTANCETYPE eInstanceType = INSTANCE_CURVELINE;
		AxisType eAxisType = Axis_X;
		_float3 vScl = _float3(1.f, 1.f, 1.f);
		_float3 vDir = _float3(0.f, 0.f, 0.f);
		_float4 vColor = _float4(0.f, 0.f, 0.f, 0.f);
		_float fForce = 0.f;
		_float fAcceleration = 0.f;
		_float fAccMaxTime = 0.f;
		_float fAngle = 0.f;
		_bool bRandomSpeed = false;
		_bool bRandomPos = false;
		_bool bTemp1 = false;
		_bool bTemp2 = false;
	}INSTANCEDESC2;

	typedef struct tagInstancetDesc
	{
		enum INSTANCETYPE { INSTANCE_DEFAULT, INSTANCE_CURVELINE, INSTANCE_ANGLEPIECE, INSTANCE_RANDOM };
		enum AxisType { Axis_X, Axis_Y, Axis_Z };

		INSTANCETYPE eInstanceType = INSTANCE_DEFAULT;
		AxisType	eAxisType = Axis_X;

		_float		fMaxTime = 10.f;
		_float		fPassTime = 0.f;
		_float		fLoop = 0.f;
		_float		fLastTime = 0.f;

		// Position
		_float3		vPosition = _float3(0.f, 0.f, 0.f);
		_float3		vPositionRand = _float3(0.f, 0.f, 0.f);

		// Scale
		_float3		vScale = _float3(0.1f, 0.f, 0.f);
		_float3		vScale_Speed = _float3(5.f, 0.f, 0.f);

		// Movement
		_float3		vMovement = _float3(-0.5f, -0.5f, -0.5f);
		_float3		vMovementRand = _float3(1.f, 1.f, 1.f);

		// Speed
		_float		fSpeed = 1.f;
		_float		fSpeedRand = 1.f;

		// Color
		_float4		vColor1 = _float4(0.f, 0.f, 0.f, 0.f);
		_float4		vColor2 = _float4(0.f, 0.f, 0.f, 0.f);

		_bool		bBillboard = false;

		// KeyFrame1
		_float		KeyFrame_1_TimeEnd = 2.5f;
		_float		KeyFrame_1_RelaxSpeed = 1.f;
		_float		KeyFrame_1_Speed = 0.f;
		_float		KeyFrame_1_fRelaxRotation = 0.f;
		_float4		KeyFrame_1_Movement = _float4(0.f, 0.f, 0.f, 0.f);

		_float		KeyFrame_2_TimeEnd = 5.f;
		_float		KeyFrame_2_RelaxSpeed = 1.f;
		_float		KeyFrame_2_Speed = 0.f;
		_float		KeyFrame_2_fRelaxRotation = 0.f;
		_float4		KeyFrame_2_Movement = _float4(0.f, 0.f, 0.f, 0.f);

		_float		KeyFrame_3_TimeEnd = 7.5f;
		_float		KeyFrame_3_RelaxSpeed = 1.f;
		_float		KeyFrame_3_Speed = 0.f;
		_float		KeyFrame_3_fRelaxRotation = 0.f;
		_float4		KeyFrame_3_Movement = _float4(0.f, 0.f, 0.f, 0.f);

		_float		KeyFrame_4_TimeEnd = 10.f;
		_float		KeyFrame_4_RelaxSpeed = 1.f;
		_float		KeyFrame_4_Speed = 0.f;
		_float		KeyFrame_4_fRelaxRotation = 0.f;
		_float4		KeyFrame_4_Movement = _float4(0.f, 0.f, 0.f, 0.f);

		_float4x4	tmpMat1;
	}INSTANCEDESC;

	typedef struct tagAnimFastSection
	{
		_float fStartSpot = -1.f;
		_float fEndSpot = -1.f;
		_float fFastValue = 1.f;

		_double Convert(const _double& _fValue, const _double& _dTimeValue)
		{
			_double fValue = _dTimeValue;

			if (fStartSpot <= _fValue && _fValue <= fEndSpot)
				fValue *= fFastValue;

			return fValue;
		}

		_bool Pass(const _double& _dTimeValue)
		{
			if (fStartSpot <= _dTimeValue && _dTimeValue <= fEndSpot)
				return true;

			return false;
		}

	}ANIMFASTSECTION;

	typedef struct tagAnimClipSection
	{
		_float fChange = 0.f;
		_float fEndValue = 0.f;
		_string strName;
		_bool Pass(const _char* _strName, const _double& _dTimeValue, const _double& _dSupTime)
		{
			if (!strcmp(_strName, strName.c_str()))
			{
				if (fEndValue - _dSupTime <= _dTimeValue)
					return true;
			}
			return false;
		}

		_bool Is_Between(const char* _strName, const _double& _dTimeValue)
		{
			if (!strcmp(_strName, strName.c_str()))
			{
				if (fChange <= _dTimeValue && _dTimeValue <= fEndValue)
					return true;
			}
			return false;
		}
	}ANIMCLIPSECTION;

	typedef struct tagRHANDLER_VALUE
	{
		tagRHANDLER_VALUE() = default;
		template<typename T>
		tagRHANDLER_VALUE(T& Data, const _string& ShaderName)
			: strShaderName(ShaderName)
		{
			pData = &Data;
			dwValue = sizeof(Data);
		}
		void* pData = nullptr;
		DWORD		dwValue = 0;
		_string		strShaderName;
	}RH_VALUE;
	typedef vector< RH_VALUE> RH_VALUE_VEC;
	typedef struct tagRHandler_Texture
	{
		tagRHandler_Texture() = default;
		tagRHandler_Texture(const _wstring& _strTargetTag,
			const _string& _strShaderName)
			: strTargetTag(_strTargetTag), strShaderName(_strShaderName)
		{}
		_wstring										strTargetTag = L"";
		_string											strShaderName = "";
	}RH_TEX;
	typedef vector< RH_TEX> RH_TEX_VEC;

	typedef struct tagCusTimer
	{
		tagCusTimer() = default;
		tagCusTimer(const _float& _fStandardTime)
			: fStandardTime(_fStandardTime)
		{
			fTimer = 0.f;
			bPass = false;
		}

		_bool Is_Over(const _double& _dTimeDelta)
		{
			if (fTimer >= fStandardTime)
			{
				bPass = true;
				return true;
			}
			fTimer += (_float)_dTimeDelta;
			return false;
		}

		void Reset_Timer()
		{
			fTimer = 0.f;
			bPass = false;
		}

		_float fTimer = 0.f;
		_float fStandardTime = 0.f;
		_bool bPass = false;
	}CUSTIMER;

	typedef struct tagCinematicCamData
	{
		tagCinematicCamData() = default;
		tagCinematicCamData(const _float3& _vCamPos, const _float4& _vRotValue = _float4(0.f, 0.f, 0.f, 0.f),
			const _float& _fSpeed = 0.f)
			: vCamPos(_vCamPos), vRotValue(_vRotValue), fSpeed(_fSpeed)
		{}
		_float3		vCamPos = _float3(0.f, 0.f, 0.f);
		_float4		vRotValue = _float4(0.f, 0.f, 0.f, 0.f);
		_float			fSpeed = 0.f;
	}CINEMATIC_CAMDATA;

	typedef struct tagOutWorldMatrix
	{
		_float3 vPos = _float3(0.f, 0.f, 0.f);
		_float3 vScale = _float3(0.f, 0.f, 0.f);
		_float3 vRot = _float3(0.f, 0.f, 0.f);
		_float4 vQuaternionRot = _float4(0.f, 0.f, 0.f, 0.f);
	}OUTMATRIX;
}