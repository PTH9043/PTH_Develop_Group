#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_ACHANEL_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_ACHANEL_H

#include "ACoreBase.h"

BEGIN(Core)
class ABoneNode;
class AAnimation;
class AAnimator;

using KEYFRAMECONTAINER = VECTOR<KEYFRAME>;

class CORE_DLL AChannel final : public ACoreBase {
public:
	AChannel();
	AChannel(const AChannel& _rhs);
	DESTRUCTOR(AChannel)
public:
	_bool NativeConstruct(AAnimator* _pAnimator, std::ifstream& _read);

	void UpdateTransformMatrix(const _double& _dTimeAcc, AAnimation* _pAnimation);
	void UpdateSupplyToCurAndNextTransformMatrix(const _double& _dTimeAcc, const _float _fRatio,
		AAnimation* _pAnimation, CSHPTRREF<AChannel> _spNextAnimChannel);

	void ComputeCurKeyFrames(const _double& _dTimeAcc);
	void ComputeCurKeyFrames(const _double& _dTimeAcc, REF_OUT _int& _iCurKeyFrame);
	void ResetCurKeyFrames() { this->m_iCurrentKeyFrames = 0; }
public: /* get set */
	const _int GetCurrentKeyFrame() const { return m_iCurrentKeyFrames; }
private:
	virtual void Free() override;
private:
	static  constexpr Vector4			ZERO_VALUE{ 0.f, 0.f, 0.f, 1.f };
	// 사용하는 것들
	Vector3											m_vScale;
	Vector4											m_vRotation;
	Vector4											m_vPosition;
	_double											m_dCurTimeAcc;
	_int													m_iCurrentKeyFrames;
	_int													m_iNumKeyFrameRealSize;
	
	KEYFRAMECONTAINER				m_KeyFrameContainer;
	_string											m_strBoneNodeName;
	SHPTR<ABoneNode>					m_spBoneNode;
};

END

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_ACHANEL_H