#include <string>
#include "ProtocolBuffer.h"
#include <thread>


PROTOFUNC::PROTOCOL_MEMORY_LEAK_REMOVER::~PROTOCOL_MEMORY_LEAK_REMOVER()
{
	google::protobuf::ShutdownProtobufLibrary();
}

void PROTOFUNC::MakeVector3(VECTOR3* _pOut, float _x, float _y, float _z)
{
	assert(nullptr != _pOut);
	_pOut->set_x(_x);
	_pOut->set_y(_y);
	_pOut->set_z(_z);
}

void PROTOFUNC::MakeVector4(VECTOR4* _pOut, float _x, float _y, float _z, float _w)
{
	assert(nullptr != _pOut);
	_pOut->set_x(_x);
	_pOut->set_y(_y);
	_pOut->set_z(_z);
	_pOut->set_w(_w);
}

void PROTOFUNC::MakeEqInfo(EQINFO* _pOut, float _gold, float _increasePower, float _increaseDefensive, float _increaseHp)
{
	assert(nullptr != _pOut);
	_pOut->set_gold(_gold);
	_pOut->set_increasepower(_increasePower);
	_pOut->set_increasedefensive(_increaseDefensive);
	_pOut->set_increasehp(_increaseHp);
}

void PROTOFUNC::MakeCollisionData(COLLISIONDATA* _pOut, LLONG _id, LLONG _enemyID)
{
	assert(nullptr != _pOut);
	_pOut->set_id(_id);
	_pOut->set_enemyid(_enemyID);
}

void PROTOFUNC::MakeCharState(CHARSTATE* _pOut, LLONG _id, const VECTOR3& _vPos,
	const VECTOR3& _vRotate, int _animstate, int _curAnimIndex, int _triggerOn)
{
	assert(nullptr != _pOut);
	_pOut->set_id(_id);
	_pOut->set_posx(_vPos.x());
	_pOut->set_posy(_vPos.y());
	_pOut->set_posz(_vPos.z());
	// rotate
	_pOut->set_rotatex(_vRotate.x());
	_pOut->set_rotatey(_vRotate.y());
	_pOut->set_rotatez(_vRotate.z());
	_pOut->set_state(_animstate);
	_pOut->set_animationindex(_curAnimIndex);
	_pOut->set_triggeron(_triggerOn);
}

void PROTOFUNC::MakeCharState(CHARSTATE* _pOut, LLONG _id, const VECTOR3& _vPos, const VECTOR3& _vRotate)
{
	assert(nullptr != _pOut);
	_pOut->set_id(_id);
	_pOut->set_posx(_vPos.x());
	_pOut->set_posy(_vPos.y());
	_pOut->set_posz(_vPos.z());
	// rotate
	_pOut->set_rotatex(_vRotate.x());
	_pOut->set_rotatey(_vRotate.y());
	_pOut->set_rotatez(_vRotate.z());
}

void PROTOFUNC::MakeMobState(MOBSTATE* _pOut, LLONG _id, const VECTOR3& _vPos, const VECTOR3& _vRotate, int _animstate, int _curAnimIndex, bool _triggerOn, bool _foundOn, double _animtime)
{
	assert(nullptr != _pOut);
	_pOut->set_id(_id);
	_pOut->set_posx(_vPos.x());
	_pOut->set_posy(_vPos.y());
	_pOut->set_posz(_vPos.z());
	// rotate
	_pOut->set_rotatex(_vRotate.x());
	_pOut->set_rotatey(_vRotate.y());
	_pOut->set_rotatez(_vRotate.z());
	_pOut->set_state(_animstate);
	_pOut->set_animationindex(_curAnimIndex);
	_pOut->set_triggeron(_triggerOn);
	_pOut->set_foundon(_foundOn);
	_pOut->set_animtime(_animtime);
}


void PROTOFUNC::MakeMobState(MOBSTATE* _pOut, LLONG _id, const VECTOR3& _vPos, const VECTOR3& _vRotate)
{
	assert(nullptr != _pOut);
	_pOut->set_id(_id);
	_pOut->set_posx(_vPos.x());
	_pOut->set_posy(_vPos.y());
	_pOut->set_posz(_vPos.z());
	// rotate
	_pOut->set_rotatex(_vRotate.x());
	_pOut->set_rotatey(_vRotate.y());
	_pOut->set_rotatez(_vRotate.z());
}

/* =========== SC =============== */
// Server To Client 

void PROTOFUNC::MakeScConnectSuccess(SC_CONNECTSUCCESS* _pOut, LLONG _id, int _cellIndex,  int _type)
{
	assert(nullptr != _pOut);
	_pOut->set_id(_id);
	_pOut->set_cellindex(_cellIndex);
	_pOut->set_type(_type);
}

void PROTOFUNC::MakeScOtherClientLogin(SC_OTHERCLIENTLOGIN* _pOut, LLONG _id, int _cellIndex, int _type)
{
	assert(nullptr != _pOut);
	_pOut->set_id(_id);
	_pOut->set_cellindex(_cellIndex);
	_pOut->set_type(_type);
}

void PROTOFUNC::MakeScMonsterFind(SC_MONSTERFIND* _pOut, LLONG _id, int _findtype, int _targetID)
{
	assert(nullptr != _pOut);
	_pOut->set_id(_id);
	_pOut->set_findtype(_findtype);
	_pOut->set_targetid(_targetID);
}

void PROTOFUNC::MakeScDamaged(SC_DAMAGED* _pOut, LLONG _id, float _damage)
{
	assert(nullptr != _pOut);
	_pOut->set_id(_id);
	_pOut->set_damage(_damage);
}

void PROTOFUNC::MakeScSeePlayerMove(SC_SEEPLAYERMOVE* _pOut, LLONG _id, const VECTOR3& _vPos)
{
	assert(nullptr != _pOut);
	_pOut->set_id(_id);
	_pOut->set_posx(_vPos.x());
	_pOut->set_posy(_vPos.y());
	_pOut->set_posz(_vPos.z());
}

/* =========== CS =============== */
// Client To Server 

void PROTOFUNC::MakeCsLogin(CS_LOGIN* _pOut, LLONG _id)
{
	assert(nullptr != _pOut);
	_pOut->set_id(_id);
}


void PROTOFUNC::MakeCsAttack(CS_ATTACK* _pOut, LLONG _id, float _damage, VECTOR3* _pMovePos)
{
	assert(nullptr != _pOut);
	_pOut->set_id(_id);
	_pOut->set_damage(_damage);
	_pOut->set_allocated_attackpos(_pMovePos);
}

void PROTOFUNC::MakeCsDisconnect(CS_DISCONNECT* _pOut, LLONG _id)
{
	assert(nullptr != _pOut);
	_pOut->set_id(_id);
}

void PROTOFUNC::MakeCsDamaged(SC_DAMAGED* _pOut, LLONG _id, float _damage)
{
	assert(nullptr != _pOut);
	_pOut->set_id(_id);
	_pOut->set_damage(_damage);
}
