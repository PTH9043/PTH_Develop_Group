#pragma once
#include "UBase.h"

BEGIN(Tool)
class TAssimpModel;
class TAssimpBoneNode;
/*
@ Date: 2024-02-04, Writer: 박태현
@ Explain
- Animation Frame 마다 있는 Channel 클래스
*/
class TAssimpChannel : public UBase {
public:
	TAssimpChannel();
	TAssimpChannel(const TAssimpChannel& _rhs);
	DESTRUCTOR(TAssimpChannel)
public: /* get set*/
	const _wstring& GetName() const { return m_wstrName; }
public:
	virtual void Free() override;
	HRESULT NativeConstruct(struct aiNodeAnim* _pChannel, CSHPTRREF< TAssimpModel> _pModel);
	HRESULT NativeConstruct(struct aiNodeAnim* _pChannel, CSHPTRREF< TAssimpModel> _pModel, const _wstring& _wstrRootBoneName);
	HRESULT NativeConstruct(CSHPTRREF<TAssimpModel> _spAnimModel, const CHANNELDESC& _stChannelDesc);
	void GetData(CHANNELDESC& _stChannelDesc);
private:
	using KEYFRAMES = VECTOR<KEYFRAME>;
	KEYFRAMES									m_vecKeyFrames;
	SHPTR<TAssimpBoneNode>		m_spBoneNode;
	_wstring											m_wstrName;
	_uint												m_iNumMaxKeyFrames;
	_uint												m_iCurKeyFrames;
};

END

