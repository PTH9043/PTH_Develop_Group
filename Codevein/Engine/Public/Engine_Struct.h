#pragma once

namespace Engine
{
	typedef struct tagModelMatrial
	{
		class CTexture* pMaterialTexture[TEXTYPE::TextureType_UNKNOWN] = { nullptr };
	}MODELMATRIALDESC;

	typedef struct tagGraphicDesc
	{
		enum WINMODE { MODE_FULL, MODE_WIN, MODE_END };

		HWND hWnd;
		unsigned int iWinCX;
		unsigned int iWinCY;
		WINMODE  eWindowed = MODE_END;
	}GRAPHICDESC;

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

	typedef struct tagVertexTexture
	{
		XMFLOAT3 v3Position;
		XMFLOAT2 v2TexUv;
	}VTXTEX;

	typedef struct ENGINE_DLL tagVertexTexture_Declaration
	{
		static const unsigned int iNumElement = 2;
		static D3D11_INPUT_ELEMENT_DESC  Element[iNumElement];
	}VTXTEX_DECLARATION;

	typedef struct tagVertexNormalTexture
	{
		XMFLOAT3 v3Position;
		XMFLOAT3 v3Normal;
		XMFLOAT2 v2TexUv;
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertexNormalTexture_Declaration
	{
		static const unsigned int iNumElement = 3;
		static D3D11_INPUT_ELEMENT_DESC  Element[iNumElement];
	}VTXNORTEX_DECLARATION;

	typedef struct tagVertexCubeTexture
	{
		XMFLOAT3 v3Position;
		XMFLOAT3 v3UvPosition;
	}VTXCUBETEX;

	typedef struct ENGINE_DLL tagVertexCubeTexture_Declaration
	{
		static const unsigned int iNumElement = 2;
		static D3D11_INPUT_ELEMENT_DESC  Element[iNumElement];
	}VTXCUBETEX_DECLARATION;

	typedef struct tagVertexCubeNorTexture
	{
		XMFLOAT3 v3Position;
		XMFLOAT4 vDirection;
		XMFLOAT3 v3UvPosition;
	}VTXCUBNORETEX;

	typedef struct ENGINE_DLL tagVertexCubeNorTexture_Declaration
	{
		static const unsigned int iNumElement = 3;
		static D3D11_INPUT_ELEMENT_DESC  Element[iNumElement];
	}VTXCUBNORTEX_DECLARATION;

	typedef struct  tagVertexModel
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
		XMFLOAT3			vTangent;
	}VTXMODEL;

	typedef struct ENGINE_DLL tagVertexModel_Declaration
	{
		static const unsigned int iNumElement = 4;
		static D3D11_INPUT_ELEMENT_DESC  Element[iNumElement];
	}VTXMODEL_DECLARATION;

	typedef struct tagVertexAnimModel
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
		XMFLOAT3			vTangent;

		XMUINT4				vBlendIndex; /* 현재 정점의 상태표현에 사용되는 뼈의 갯수.(최대 4개) */
		XMFLOAT4			vBlendWeight; /* 정점에 영향을 주는 각 뼈 상태의 비율.(최대 4개) */
	}VTXANIMMODEL;

	typedef struct ENGINE_DLL tagVertexAnimModel_Declaration
	{
		static const unsigned int iNumElement = 6;
		static D3D11_INPUT_ELEMENT_DESC  Element[iNumElement];
	}VTXANIMMODEL_DECLARATION;

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

	typedef struct tagVertexCubeInstance
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;
		XMFLOAT4			vColor;
		XMFLOAT4			vBlendColor;
	}VTXCUBEINSTANCE;

	typedef struct ENGINE_DLL tagVertexCubeInstance_Declaration
	{
		static const unsigned int iNumElement = 8;
		static D3D11_INPUT_ELEMENT_DESC  Element[iNumElement];
	}VTXCUBENSTANCE_DECLARATION;

	typedef struct tagVertexRectInstance
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vTranslation;
		XMFLOAT4			vColor;
		XMFLOAT4			vBlendColor;
	}VTXRECTINSTANCE;

	typedef struct ENGINE_DLL tagVertexRectInstance_Declaration
	{
		static const unsigned int iNumElement = 8;
		static D3D11_INPUT_ELEMENT_DESC  Element[iNumElement];
	}VTXRECTINSTANCE_DECLARATION;

	typedef struct tagVertexPoint
	{
		XMFLOAT3			vPosition;
		XMFLOAT2			vPSize;
	}VTXPOINT;

	typedef struct ENGINE_DLL tagVertexPointInstance_Declaration
	{
		static const unsigned int iNumElement = 6;
		static D3D11_INPUT_ELEMENT_DESC  Element[iNumElement];
	}VTXPOINTINSTANCE_DECLARATION;

	typedef struct tagLightDesc
	{
		enum TYPE{ TYPE_DIRECTIONAL, TYPE_POINT, TYPE_SPOT, TYPE_END};

		TYPE			eLightType;
		_float4		v4Direction;
		_float3		v3Position;
		_float			fRange;
		// 빛의 색
		_float4		v4Diffuse;		// 난반사 광원에 의한 색상  
		_float4		v4Ambient;	// 빛은 1, 1, 1고 머터리얼의 Ambient는 0.4라고 치면 쉐이더에 그냥 더한다.  
		_float4		v4Specular;
	}LIGHTDESC;
	// Matrial을 Pixel마다 정의하는게 맞다 -> 디자이너가 텍스쳐로부터 지정

	// File Data
	typedef struct tagFileData
	{
		_wstring	strFileName;
		_wstring	strFilePath;
		_uint			iIndex;
	}FILEDATA;

	typedef struct tagFileGroup
	{
		_wstring								strUppderName;
		_wstring								strPath;
		list<tagFileGroup*>			lsUnderList;
		vector<FILEDATA>				vecFileDataList;
		tagFileGroup*						pParentsUpper = nullptr;

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
		_uint			iNumVertices = 0;
		_uint			iNumIndices = 0;
		_float3*		pVerticesPos;
		_wstring	strName;
	}BUFFDESC;
	// Channel Desc 
	typedef struct tagChannelDesc
	{
		_wstring						strName = L"";
		_uint								iNumMaxKeyFrames = 0;
		KEYFRAME*					pKeyFrames;
		// Save Data
		void Save_Data(const HANDLE& _hFile, DWORD& _dwByte)
		{
			WriteFile(_hFile, &iNumMaxKeyFrames, sizeof(_uint), &_dwByte, 0);
			_uint Length = (_uint)lstrlen(strName.c_str()) + 1;
			WriteFile(_hFile, &Length, sizeof(_uint), &_dwByte, 0);
			WriteFile(_hFile, strName.c_str(), sizeof(_tchar) * Length, &_dwByte, 0);

			for (_uint i = 0; i < iNumMaxKeyFrames; ++i)
				WriteFile(_hFile, &pKeyFrames[i], sizeof(KEYFRAME), &_dwByte, 0);
		}
		// Load Data
		void Load_Data(const HANDLE& _hFile, DWORD& _dwByte)
		{
			ReadFile(_hFile, &iNumMaxKeyFrames, sizeof(_uint), &_dwByte, 0);
			{
				_uint iLength = 0;
				ReadFile(_hFile, &iLength, sizeof(_uint), &_dwByte, nullptr);
				_tchar* pName = new _tchar[iLength];
				ReadFile(_hFile, pName, sizeof(_tchar) * iLength, &_dwByte, nullptr);
				strName = pName;
				Safe_Delete_Array(pName);
			}
			pKeyFrames = new KEYFRAME[iNumMaxKeyFrames];
			ReadFile(_hFile, pKeyFrames, sizeof(KEYFRAME) * iNumMaxKeyFrames, &_dwByte, 0);
		}
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
		void Save_Data(const HANDLE& _hFile, DWORD& _dwByte)
		{
			WriteFile(_hFile, &iNumChannels, sizeof(_uint), &_dwByte, 0);
			WriteFile(_hFile, &dTickPerSeconds, sizeof(_double), &_dwByte, 0);
			WriteFile(_hFile, &dDuration, sizeof(_double), &_dwByte, 0);
			_uint Length = (_uint)lstrlen(strName.c_str()) + 1;
			WriteFile(_hFile, &Length, sizeof(_uint), &_dwByte, 0);
			WriteFile(_hFile, strName.c_str(), sizeof(_tchar) * Length, &_dwByte, 0);
			for(auto& iter : vecChannels){
				iter.Save_Data(_hFile, _dwByte);
			}
		}
		// Load Data
		void Load_Data(const HANDLE& _hFile, DWORD& _dwByte)
		{
			ReadFile(_hFile, &iNumChannels, sizeof(_uint), &_dwByte, 0);
			ReadFile(_hFile, &dTickPerSeconds, sizeof(_double), &_dwByte, 0);
			ReadFile(_hFile, &dDuration, sizeof(_double), &_dwByte, 0);
			{
				_uint iLength = 0;
				ReadFile(_hFile, &iLength, sizeof(_uint), &_dwByte, nullptr);
				_char* pName = new _char[iLength];
				ReadFile(_hFile, pName, sizeof(_char) * iLength, &_dwByte, nullptr);
				string Name = pName;
				strName.assign(Name.begin(), Name.end());
				Safe_Delete_Array(pName);
			}

			for (_uint j = 0; j < iNumChannels; ++j)
			{
				CHANNELDESC tChannel;
				tChannel.Load_Data(_hFile, _dwByte);
				vecChannels.push_back(tChannel);
			}
		}
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
		VTXMODEL*					pVtxModel = nullptr;
		FACEINDICES32*			pIndices = nullptr;
		_uint								iMatrialIndex = 0;
		vector<_wstring>		HierachyNameList;
		_uint								iHierachyCount = 0;
	}MESHDESC;
	// Animation Mesh 
	typedef struct tagAnimMeshDesc : public BUFFDESC
	{
		VTXANIMMODEL*		pVtxModel = nullptr;
		FACEINDICES32*			pIndices = nullptr;
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
		FILEGROUP*												pFileGroup = nullptr;
		FILEDATA*													pFileData = nullptr;

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

	typedef struct tagParticleInstanceInfo
	{
		tagParticleInstanceInfo() = default;
		// Effect Particle Desc
		// 
		_float fLifeTime = 0.f;
		// 힘 
		_float fForce = 0.f;
		// 가속도
		_float fAcceleration = 0.f;
		// 점프 
		_float fJumpValue = 0.f;
		// 점프 타임 Value
		_float fJumpTimeFast = 1.f;
		// 질량
		_float fMass = 1.f;
		// 점프 타임
		_float fJumpTime = 0.f;
		// Particle Color
		_float4 vParticleColor = _float4(0.f, 0.f, 0.f, 0.f);
		// Blend Color
		_float4 vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
		// Blend Speed 
		_float  fBlendSpeed = 1.f;
		// 초기 Angle
		_float3 vRotAnlge = _float3(0.f, 0.f, 0.f);
		// 각속도
		_float3 vRotSpeed = _float3(0.f, 0.f, 0.f);
		// Rotate Transform
		_float3 vRotTranslate = _float3(0.f, 0.f, 0.f);
		// Rot Length
		_float  fRotLength = 0.f;
		// Direction
		_float3 vDirection = _float3(0.f, 0.f, 0.f);
		// 크기
		_float3 vSize = _float3(1.f, 1.f, 1.f);
		// Plus Scale
		_float3 vScaleSizeSpeed = _float3(0.f, 0.f, 0.f);
		// Result Pm Scale Size
		_float3 vResultPmScaleSize = _float3(0.f, 0.f, 0.f);
		// Update UVTranslate
		_float3 vUpdateUVTranslate = _float3(0.f, 0.f, 0.f);
		// 초기 위치
		_float3 vInitPos = _float3(0.f, 0.f, 0.f);
		// 결과 힘
		_float fResultForce = 0.f;
		// 생존 시간
		_float fTime = 0.f;
		// Result Blend Color
		_float4 vResultBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
		// Move Jump Time
		_float fMoveJumpTime = 0.f;
		// Result Rot Angle
		_float3 vResultRotAnlge = _float3(0.f, 0.f, 0.f);

		_float fSaveTime = 0.f;

		_bool bGravity = false;
		_bool bControlScaleSize = false;
		_bool bLoop = false;
	public:
		void Update_Data(const _double& _dTimeDelta)
		{
			vResultRotAnlge.x += (_float)(vRotSpeed.x * _dTimeDelta);
			vResultRotAnlge.y += (_float)(vRotSpeed.y * _dTimeDelta);
			vResultRotAnlge.z += (_float)(vRotSpeed.z * _dTimeDelta);
		
			_vector BlendColor = XMLoadFloat4(&vBlendColor) * (_float)(_dTimeDelta) *fBlendSpeed;
			XMStoreFloat4(&vResultBlendColor, BlendColor + XMLoadFloat4(&vResultBlendColor));

			if (vResultBlendColor.x > 1.f)
				vResultBlendColor.x = 1.f;

			if (vResultBlendColor.y > 1.f)
				vResultBlendColor.y = 1.f;

			if (vResultBlendColor.z > 1.f)
				vResultBlendColor.z = 1.f;

			if (vResultBlendColor.w > 1.f)
				vResultBlendColor.w = 1.f;

			if (true == bControlScaleSize)
			{
				vResultPmScaleSize.x += (_float)(vScaleSizeSpeed.x * _dTimeDelta);
				vResultPmScaleSize.y += (_float)(vScaleSizeSpeed.y * _dTimeDelta);
				vResultPmScaleSize.z += (_float)(vScaleSizeSpeed.z * _dTimeDelta);
			}

			if (true == bLoop)
			{
				fSaveTime += (_float)(_dTimeDelta);
			}
		}

		void Reset()
		{
			fMoveJumpTime = fTime;
			fResultForce = fForce;
			vResultRotAnlge = vRotAnlge;
			vResultBlendColor = vBlendColor;
			vResultPmScaleSize = _float3(0.f, 0.f, 0.f);
			fSaveTime = 0.f;
			XMStoreFloat3(&vDirection, XMVector3Normalize(XMLoadFloat3(&vDirection)));
		}
		_vector Translate_Pos(_vector _vPos, const _double& _dTimeDelta)
		{
			_vector vDir = XMLoadFloat3(&vDirection) * (_float)(fResultForce * _dTimeDelta);
			_vector pos = _vPos + vDir;
			fResultForce += (_float)(fAcceleration * _dTimeDelta);

			if (true == bGravity)
			{
				_float fY = (-9.8f * fJumpValue * fMoveJumpTime * fMass) * (_float)(_dTimeDelta);
				fMoveJumpTime -= (_float)(_dTimeDelta) *fJumpTimeFast;
				pos = XMVectorSetY(pos, fY);
			}

			return pos;
		}

	}EFFECTPARTICLEDESC;


	typedef struct tagShaderVertexMovement
	{
		_float3		v3Direction;
		_float			vSpeed;
	}SHADERVTXMV;

	typedef struct tagShaderUVMovement
	{
		_float3 v3Direction;
		_float	  vSpeed;
	}SHADERUVMV;

	typedef struct tagShaderMovementGroup
	{
		tagShaderMovementGroup() = default;

		void Insert(const _uint& _iID, const SHADERVTXMV& _tShaderVtxMv)
		{
			mShaderVTXMVG.insert(pair<_uint, SHADERVTXMV>(_iID, _tShaderVtxMv));
		}

		map<_uint, SHADERVTXMV>  mShaderVTXMVG;
	}SHADERMVGROUP;

	typedef struct tagShaderUVGroup
	{
		tagShaderUVGroup() = default;

		void Insert(const _uint& _iID, const SHADERUVMV& _tShaderVtxMv)
		{
			mShaderUVG.insert(pair<_uint, SHADERUVMV>(_iID, _tShaderVtxMv));
		}

		map<_uint, SHADERUVMV>  mShaderUVG;
	}SHADERUVGROUP;
}