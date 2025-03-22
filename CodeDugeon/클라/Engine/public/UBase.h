#ifndef _PTH_FRAMEWORK_ENGIEN_PUBLIC_UBASE_H
#define _PTH_FRAMEWORK_ENGIEN_PUBLIC_UBASE_H

BEGIN(Engine)

/*
최상위 클래스 모든 클래스는 반드시 이 클래스의 하위 클래스여야 한다.
예외란 존재하지 않는다.
*/

class UBase abstract : public std::enable_shared_from_this<UBase> {
protected:
	UBase();
	UBase(const UBase& _rhs) : enable_shared_from_this<UBase>(_rhs) {}
	virtual  ~UBase() {  }
public:
	// 삭제
	virtual void Free() PURE;
protected:
	// 자기 자신을 캐스팅 해서 사용한다. 
	template<class T>
	std::shared_ptr<T> ThisShared() { return std::static_pointer_cast<T>(shared_from_this()); }
	template<class T>
	std::weak_ptr<T> ThisWeak() { return std::static_pointer_cast<T>(weak_from_this()); }
};

END

#endif // _PTH_FRAMEWORK_ENGIEN_PUBLIC_UBASE_H