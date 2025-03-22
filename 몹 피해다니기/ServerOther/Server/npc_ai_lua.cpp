#include "Defines.h"


#include "CLuaContainer.h"
#include "CMyDataBase.h"
#include "CPathFinder.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
#pragma comment(lib, "lua54.lib")
constexpr int VIEW_RANGE = 100;

enum EVENT_TYPE { EV_RANDOM_MOVE, EV_FIND, EV_ATTACK, EV_END };

enum DBEVENT_TYPE { DB_CHNAGE, DB_GET, DB_DELETE, DB_END};

struct TIMER_EVENT {
	int obj_id;
	chrono::system_clock::time_point wakeup_time;
	EVENT_TYPE event_id;
	int target_id;
	constexpr bool operator < (const TIMER_EVENT& L) const
	{
		return (wakeup_time > L.wakeup_time);
	}
};

struct ASTAR_EVENT {
	int obj_id{ 0 }, target_id{ 0 };
	bool bEnd{ false };

public:
	ASTAR_EVENT(){}
	ASTAR_EVENT(const int _objId, const int _targetId) : obj_id(_objId), target_id(_targetId){}

	constexpr bool operator < (const ASTAR_EVENT& L) const
	{
		return obj_id < L.obj_id;
	}
};

struct DATABASE_EVENT {
	int obj_id;
	chrono::system_clock::time_point wakeup_time;
	DBEVENT_TYPE eType;

	DATABASE_EVENT() = default;
	DATABASE_EVENT(const int _id, const chrono::system_clock::time_point& _wakeup_time,
		const DBEVENT_TYPE _eventType)  : obj_id(_id), wakeup_time(_wakeup_time), 
	eType(_eventType) {}

	constexpr bool operator < (const DATABASE_EVENT& L) const
	{
		return (wakeup_time > L.wakeup_time);
	}
};

struct ALIVE_USER {
	int obj_id;
	chrono::system_clock::time_point wakeup_time;

	constexpr bool operator < (const ALIVE_USER& L) const
	{
		return (wakeup_time > L.wakeup_time);
	}
};

concurrency::concurrent_priority_queue<TIMER_EVENT> timer_queue;
concurrency::concurrent_priority_queue<ASTAR_EVENT> astar_queue;
concurrency::concurrent_priority_queue<DATABASE_EVENT> database_queue;
concurrency::concurrent_priority_queue<ALIVE_USER> wait_dead_time_user_queue;

mutex WAIT_DEAD_TIME_LOCK;


enum COMP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND, OP_NPC_MOVE, OP_NPC_FIND, OP_NPC_ATTACK };
class OVER_EXP {
public:
	WSAOVERLAPPED _over;
	WSABUF _wsabuf;
	char _send_buf[BUF_SIZE];
	COMP_TYPE _comp_type;
	int _ai_target_obj;
	OVER_EXP()
	{
		_wsabuf.len = BUF_SIZE;
		_wsabuf.buf = _send_buf;
		_comp_type = OP_RECV;
		ZeroMemory(&_over, sizeof(_over));
	}
	OVER_EXP(char* packet)
	{
		_wsabuf.len = packet[0];
		_wsabuf.buf = _send_buf;
		ZeroMemory(&_over, sizeof(_over));
		_comp_type = OP_SEND;
		memcpy(_send_buf, packet, packet[0]);
	}
	OVER_EXP(void* packet, const int _size)
	{
		_wsabuf.len = _size;
		_wsabuf.buf = _send_buf;
		ZeroMemory(&_over, sizeof(_over));
		_comp_type = OP_SEND;
		memcpy(_send_buf, packet, _size);
	}
};

enum S_STATE { ST_FREE, ST_ALLOC, ST_INGAME };

enum MON_MOVETYPE : char { MON_STAY, MON_MOVE};

class SESSION {
	OVER_EXP _recv_over;
public:
	mutex _s_lock;
	S_STATE _state;
	atomic_bool	_is_active;		// 주위에 플레이어가 있는가?
	int _id;
	SOCKET _socket;
	short	x, y;
	// prev
	short	startX, startY;
	short	endX, endY;
	atomic_int moveState{ 0 };
	char	_name[NAME_SIZE];
	int		_prev_remain;
	concurrency::concurrent_unordered_set<int> _view_list;
	concurrency::concurrent_queue<TILE*>				_BestList;
	mutex	_vl;
	int		last_move_time;
	lua_State*	_L;
	mutex	_ll;
	CHARDATA tCharData;
	MON_MOVETYPE	eMonsterMoveType{ MON_MOVE };
	short _directionX;
	short _directionY;
	atomic_bool  _is_dmg;
	chrono::system_clock::time_point waitAttack;
public:
	SESSION() : 
		 _state{ST_FREE}, _is_active{false}, _id{-1}, _socket{NULL},
		x{ 0 }, y{ 0 }, endX{ 0 }, endY{ 0 }, moveState{ 0 }, _prev_remain{0}, 
		last_move_time{ 0 }, _L{ NULL }, tCharData{}
	{
		_name[0] = 0;
	}

	~SESSION() {}

	void do_recv()
	{
		DWORD recv_flag = 0;
		memset(&_recv_over._over, 0, sizeof(_recv_over._over));
		_recv_over._wsabuf.len = BUF_SIZE - _prev_remain;
		_recv_over._wsabuf.buf = _recv_over._send_buf + _prev_remain;
		WSARecv(_socket, &_recv_over._wsabuf, 1, 0, &recv_flag,
			&_recv_over._over, 0);
	}

	void do_send(void* packet)
	{
		OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char*>(packet) };
		WSASend(_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, 0);
	}

	void do_send(void* packet, const short _size) {
		OVER_EXP* sdata = new OVER_EXP{ packet, _size };
		WSASend(_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, 0);
	}

	void send_login_info_packet()
	{
		SC_LOGIN_INFO_PACKET p;
		p.id = _id;
		::memcpy(p.name, _name, NAME_SIZE);
		p.size = sizeof(SC_LOGIN_INFO_PACKET);
		p.type = SC_LOGIN_INFO;
		p.x = x;
		p.y = y;
		p.tCharData = tCharData;
		do_send(&p);
	}
	void send_move_packet(int c_id);
	void send_add_player_packet(int c_id);
	void send_chat_packet(int c_id, const char* mess);
	void send_remove_player_packet(int c_id)
	{
		if (_view_list.count(c_id)) {
			_vl.lock();
			_view_list.unsafe_erase(c_id);
			_vl.unlock();
		}
		SC_REMOVE_OBJECT_PACKET p;
		p.id = c_id;
		p.size = sizeof(p);
		p.type = SC_REMOVE_OBJECT;
		do_send(&p);
	}
	void send_login_failed_packet();
	void send_login_ok_packet();
	void send_change_charDataPacket(int c_id);

	void SetSqlData(const SQLDATA& _tData) {
		tCharData.id = _tData.ID;
		tCharData.job = (char)_tData.Job;
		tCharData.level = _tData.Level;
		tCharData.atk = (_float)(_tData.atk);
		tCharData.luck = (_float)_tData.luck;
		tCharData.speed = (_float)_tData.speed;
		tCharData.max_hp = tCharData.hp = (_float)_tData.hp;
		tCharData.max_exp = tCharData.exp = (_float)_tData.exp;
		tCharData.max_mp = tCharData.mp = (_float)_tData.mp;
		::memcpy(_name, _tData.Name, NAME_SIZE);
		x = (short)_tData.x;
		y = (short)_tData.y;
	}

	void release() {
		_state = ST_FREE;
	}
};

HANDLE h_iocp{ NULL };
array<SESSION, MAX_USER + MAX_NPC> clients;

CMyDataBase				m_DataBase;
CLuaContainer				m_LuaContainer;
CPathFinder					m_PathFinder;
MAPARR							m_MapData;

 int									g_ADDR_SIZE{ 0 };

 default_random_engine				dre{};
 uniform_int_distribution<int>		RandomMove{0, 3};

 uniform_int_distribution<int>		RandomX{0, W_WIDTH - 1};
 uniform_int_distribution<int>		RandomY{0, W_HEIGHT - 1};


SOCKET g_s_socket, g_c_socket;
OVER_EXP g_a_over;

bool is_pc(int object_id)
{
	return object_id < MAX_USER;
}

bool is_npc(int object_id)
{
	return !is_pc(object_id);
}

bool can_see(int from, int to)
{
	int x = clients[from].x  - clients[to].x;
	x = x * x;
	int y = clients[from].y -clients[to].y;
	y = y * y;
	if (x > VIEW_RANGE) return false;
	return y <= VIEW_RANGE;
}

bool can_range(int from, int to)
{
	int x = clients[from].x - clients[to].x;
	x = x * x;
	int y = clients[from].y - clients[to].y;
	y = y * y;
	short range = clients[from].tCharData.range;
	if (x > range) return false;
	return y  <= range;
}

bool can_attack(int from, int to)
{
	int x = clients[from].x - clients[to].x;
	x = x * x;
	int y = clients[from].y - clients[to].y;
	y = y * y;
	short range = 2;
	if (x > range) return false;
	return y <= range;
}

bool OutRandomPos(short& _x, short& _y) {
	int x = RandomX(dre), y = RandomY(dre);

	if (m_MapData[y][x] == MAP_BLOCK)
		return false;

	_x = (short)x;
	_y = (short)y;
	return true;
}

bool OutRandomMove(short& _x, short& _y) {
	
	int x = _x, y = _y;
	switch (RandomMove(dre)) {
	case 0: if (y > 0) --y; break;
	case 1: if (y < W_HEIGHT - 1) ++y; break;
	case 2: if (x > 0) --x; break;
	case 3: if (x < W_WIDTH - 1) ++x; break;
	}

	if (m_MapData[y][x] == MAP_BLOCK)
		return false;

	_x = x;
	_y = y;
	return true;
}

bool OutRandomMove(short& _x, short& _y, short& _dirX, short& _dirY) {

	int x = _x, y = _y;
	_dirX = 0; _dirY = 0;

	switch (RandomMove(dre)) {
	case 0: if (y > 0) { --y; _dirY = -1; } break;
	case 1: if (y < W_HEIGHT - 1) { ++y; _dirY = 1; } break;
	case 2: if (x > 0) { --x; _dirX = -1; } break;
	case 3: if (x < W_WIDTH - 1) { ++x;  _dirX = 1; }break;
	}

	if (m_MapData[y][x] == MAP_BLOCK)
		return false;

	_x = x;
	_y = y;
	return true;
}

void SESSION::send_move_packet(int c_id)
{
	SC_MOVE_OBJECT_PACKET p;
	p.id = c_id;
	p.size = sizeof(SC_MOVE_OBJECT_PACKET);
	p.type = SC_MOVE_OBJECT;
	p.x = clients[c_id].x;
	p.y = clients[c_id].y;
	p.move_time = clients[c_id].last_move_time;
	p.dirX = clients[c_id]._directionX;
	p.dirY = clients[c_id]._directionY;
	do_send(&p);
}

void SESSION::send_add_player_packet(int c_id)
{
	SC_ADD_OBJECT_PACKET add_packet;
	add_packet.id = c_id;
	strcpy_s(add_packet.name, clients[c_id]._name);
	add_packet.size = sizeof(add_packet);
	add_packet.type = SC_ADD_OBJECT;
	add_packet.x = clients[c_id].x;
	add_packet.y = clients[c_id].y;
	add_packet.dirX = clients[c_id]._directionX;
	add_packet.dirY = clients[c_id]._directionY;
	add_packet.moveType = clients[c_id].eMonsterMoveType == MON_STAY ? TYPE_NONMOVE : TYPE_MOVE;
	::memcpy(&add_packet.tCharData, &clients[c_id].tCharData, sizeof(CHARDATA));
	{
		lock_guard<mutex> Lock{_vl};
		_view_list.insert(c_id);
	}
	do_send(&add_packet);
}

void SESSION::send_chat_packet(int p_id, const char* mess)
{
	SC_CHAT_PACKET packet;
	packet.id = p_id;
	packet.size = sizeof(packet);
	packet.type = SC_CHAT;
	strcpy_s(packet.mess, mess);
	do_send(&packet, packet.size);
}

void SESSION::send_login_failed_packet()
{
	SC_LOGIN_FAIL_PACKET packet;
	packet.type = SC_LOGIN_FAIL;
	packet.size = sizeof(packet);
	do_send(&packet);
}

void SESSION::send_login_ok_packet()
{
	SC_LOGIN_OK_PACKET packet;
	packet.type = SC_LOGIN_FAIL;
	packet.size = sizeof(packet);
	do_send(&packet);
}

void SESSION::send_change_charDataPacket(int c_id)
{
	SC_STAT_CHANGE_PACKET packet;
	packet.size = sizeof(SC_STAT_CHANGE_PACKET);
	packet.type = SC_STAT_CHANGE;
	packet.id = c_id;
	packet.tCharData.id = clients[c_id].tCharData.id;
	packet.tCharData.job = clients[c_id].tCharData.job;
	packet.tCharData.hp = clients[c_id].tCharData.hp;
	packet.tCharData.mp = clients[c_id].tCharData.mp;
	packet.tCharData.exp = clients[c_id].tCharData.exp;
	packet.tCharData.level = clients[c_id].tCharData.level;
	packet.tCharData.atk = clients[c_id].tCharData.atk;
	packet.tCharData.speed = clients[c_id].tCharData.speed;
	packet.tCharData.luck = clients[c_id].tCharData.luck;
	packet.tCharData.range = clients[c_id].tCharData.range;
	packet.checkDead = packet.tCharData.hp <= 0 ? 0 : 1;
	do_send(&packet, packet.size);
}

int get_new_client_id()
{
	for (int i = 0; i < MAX_USER; ++i) {
		lock_guard <mutex> ll{ clients[i]._s_lock };
		if (clients[i]._state == ST_FREE)
			return i;
	}
	return -1;
}

void WakeUpNPC(int npc_id, int waker)
{
	if (clients[npc_id]._is_active)
	{
		return;
	}

	bool old_state = false;
	if (false == atomic_compare_exchange_strong(&clients[npc_id]._is_active, &old_state, true))
		return;

	if (clients[npc_id].eMonsterMoveType == MON_STAY) {

		TIMER_EVENT ev{ npc_id, chrono::system_clock::now(), EV_ATTACK, 0 };
		timer_queue.push(ev);
	}
	else {
		TIMER_EVENT ev{ npc_id, chrono::system_clock::now(), EV_RANDOM_MOVE, 0 };
		timer_queue.push(ev);
	}
}

void process_packet(int c_id, char* packet)
{
	switch (packet[2]) {
	case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);

		SQLDATA tData;
		if (true == m_DataBase.IsFindData(p->id, tData)) {
			clients[c_id].SetSqlData(tData);
			DATABASE_EVENT db = { c_id, chrono::system_clock::now() + 5s, DB_CHNAGE };
			database_queue.push(db);

			strcpy_s(clients[c_id]._name, NAME_SIZE, (char*)tData.Name);
			{
				lock_guard<mutex> ll{ clients[c_id]._s_lock };
				clients[c_id]._state = ST_INGAME;
			}
			clients[c_id].send_login_info_packet();
			for (auto& pl : clients) {
				{
					if (ST_INGAME != pl._state) continue;
				}
				if (pl._id == c_id) continue;
				if (false == can_see(c_id, pl._id))
					continue;
				if (is_pc(pl._id)) pl.send_add_player_packet(c_id);
				else WakeUpNPC(pl._id, c_id);
				clients[c_id].send_add_player_packet(pl._id);
			}
		}
		else {
			clients[c_id].send_login_failed_packet();
		}
		break;
	}
	case CS_MOVE: {

		if (clients[c_id].tCharData.hp <= 0) {
			return;
		}

		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		clients[c_id].last_move_time = p->move_time;
		short x = clients[c_id].x;
		short y = clients[c_id].y;
		clients[c_id].endX = x;
		clients[c_id].endY = y;
		clients[c_id]._directionX = 0;
		clients[c_id]._directionY = 0;

		switch (p->direction) {
		case 0: if (y > 0) { --y; clients[c_id]._directionY = -1; }  break;
		case 1: if (y < W_HEIGHT - 1) { ++y; clients[c_id]._directionY = +1; }  break;
		case 2: if (x > 0) { --x; clients[c_id]._directionX= -1; } break;
		case 3: if (x < W_WIDTH - 1) { ++x; clients[c_id]._directionX = +1; }break;
		}

		if (m_MapData[y][x] == MAP_BLOCK) {
			return;
		}

		clients[c_id].x = x;
		clients[c_id].y = y;

		concurrency::concurrent_unordered_set<int> near_list;
		concurrency::concurrent_unordered_set<int> find_list;
		clients[c_id]._vl.lock();
		concurrency::concurrent_unordered_set<int> old_vlist = clients[c_id]._view_list;
		clients[c_id]._vl.unlock();
		for (auto& cl : clients) {
			{
				if (cl._state != ST_INGAME) continue;
			}
			if (cl._id == c_id) continue;
			if (can_see(c_id, cl._id))
				near_list.insert(cl._id);
		}

		clients[c_id].send_move_packet(c_id);

		for (auto& pl : near_list) {
			auto& cpl = clients[pl];
			if (is_pc(pl)) {
				if (clients[pl]._view_list.count(c_id)) {
					clients[pl].send_move_packet(c_id);
				}
				else {
					clients[pl].send_add_player_packet(c_id);
				}
			}
			else WakeUpNPC(pl, c_id);

			if (old_vlist.count(pl) == 0)
				clients[c_id].send_add_player_packet(pl);
		}

		for (auto& pl : old_vlist)
			if (0 == near_list.count(pl)) {
				clients[c_id].send_remove_player_packet(pl);
				if (is_pc(pl))
					clients[pl].send_remove_player_packet(c_id);
			}
	}
			break;
	case CS_CHAT:
		{
			CS_CHAT_PACKET* p = reinterpret_cast<CS_CHAT_PACKET*>(packet);
			for (int i = 0; i < MAX_USER; ++i) {
				if (i == c_id)continue;
				{
					if (clients[i]._state != ST_INGAME) continue;
				}
				clients[i].send_chat_packet(c_id, (char*)p->mess);
			}
		}
		break;
	case CS_ATTACK:
	{
		if (clients[c_id].waitAttack >= chrono::system_clock::now())
			return;

		CS_ATTACK_PACKET* p = reinterpret_cast<CS_ATTACK_PACKET*>(packet);
		short x = clients[c_id].x;
		short y = clients[c_id].y;

		short moveX{ 0 }, moveY{ 0 };
		switch (p->direction) {
		case 0: --moveY; break;
		case 1: ++moveY; break;
		case 2: --moveX; break;
		case 3: ++moveX; break;
		}
		clients[c_id]._directionX = (char)(moveX);
		clients[c_id]._directionY = (char)(moveY);
		concurrency::concurrent_unordered_set<int>& old_vlist = clients[c_id]._view_list;
		concurrency::concurrent_unordered_set<int> dmgList;
		concurrency::concurrent_unordered_set<int> deadList;

		vector<short> xArr;
		vector<short> yArr;
		int size = (int)clients[c_id].tCharData.speed;
		xArr.resize(size);
		yArr.resize(size);

		for (int i = 1; i <= xArr.size(); ++i) {
			xArr[i] = x + moveX * i;
		}
		for (int i = 1; i <= yArr.size(); ++i) {
			yArr[i] = y + moveY * i;
		}

		for (auto& iter : old_vlist){
			if (is_pc(iter)) continue;

			for (int i = 0; i < size; ++i) {
				if (clients[iter].x == xArr[i] && clients[iter].y == yArr[i]) {
					if (!clients[iter].tCharData.IsDead(clients[c_id].tCharData.atk)) {
						dmgList.insert(iter);
						break;
					}
					else {
						float exp = clients[iter].tCharData.exp * (clients[iter].eMonsterMoveType == TYPE_MOVE ? 2 : 1);
						deadList.insert(iter);
						clients[c_id].tCharData.exp += exp;
						clients[c_id].tCharData.PlayerLevelUp();
					}
				}
				clients[iter]._is_dmg = true;
			}
		}

		for (auto& id : dmgList) {
			for (int i = 0; i < MAX_USER; ++i) {
				if (clients[i]._state != ST_INGAME) continue;
				clients[i].send_change_charDataPacket(id);
			}
		}

		for (auto& id : deadList) {
			for (int i = 0; i < MAX_USER; ++i) {
				if (clients[i]._state != ST_INGAME) continue;
				clients[i].send_remove_player_packet(id);
			}
			{
				clients[id].release();
				lock_guard<mutex> LOCK{clients[id]._s_lock};
				old_vlist.unsafe_erase(id);
			}
		}

		clients[c_id].waitAttack = chrono::system_clock::now() + 1s;
	}
		break;
	default:
		break;
	}
}

void disconnect(int c_id)
{
	clients[c_id]._vl.lock();
	concurrency::concurrent_unordered_set <int> vl = clients[c_id]._view_list;
	clients[c_id]._vl.unlock();
	for (auto& p_id : vl) {
		if (is_npc(p_id)) continue;
		auto& pl = clients[p_id];
		{
			lock_guard<mutex> ll(pl._s_lock);
			if (ST_INGAME != pl._state) continue;
		}
		if (pl._id == c_id) continue;
		pl.send_remove_player_packet(c_id);
	}
	m_DataBase.EraseLoginData(clients[c_id].tCharData.id);
	closesocket(clients[c_id]._socket);

	lock_guard<mutex> ll(clients[c_id]._s_lock);
	clients[c_id]._state = ST_FREE;
}

void SendToPlayerMessage(SESSION& npc, int player_id, const char* _pMessage) {

	lua_State* L = npc._L;
	lua_getglobal(L, "API_SendMessage");
	lua_pushnumber(L, npc._id);
	lua_pushnumber(L, player_id);
	lua_pushstring(L, _pMessage);
	lua_pcall(L, 3, 1, 0);
	lua_pop(L, 1);
}

void MoveToEnemy(SESSION& npc, int player_id) {
	auto L = npc._L;
	lua_getglobal(L, "MoveToEnemy");
	lua_pushnumber(L, player_id);
	lua_pcall(L, 1, 0, 0);
}

void do_npc_random_move(int npc_id)
{
	SESSION& npc = clients[npc_id];

	if (!npc._BestList.empty()) {
		TILE* pTile = nullptr;
		npc._BestList.try_pop(pTile);

		{
			npc.x = (short)pTile->vPosition.y;
			npc.y = (short)pTile->vPosition.x;
		}
	}
	else
	{
		short px = npc.x, py = npc.y;

		if (clients[npc_id].eMonsterMoveType == TYPE_MOVE)
			OutRandomMove(npc.x, npc.y, npc._directionX, npc._directionY);

		if (!(npc.startX <= npc.x && npc.endX >= npc.x)) {
			if (!(npc.startY <= npc.y && npc.endY >= npc.y)) {
				npc.x = px;
				npc.y = py;
			}
		}
	}

	unordered_set<int> old_vl;
	unordered_set<int> new_vl;
	for (auto& obj : clients) {
		if (ST_INGAME != obj._state) continue;
		if (true == is_npc(obj._id)) continue;
		if (true == can_see(npc._id, obj._id))
		{
			old_vl.insert(obj._id);
			new_vl.insert(obj._id);
		}
	}

	for (auto pl : new_vl) {
		if (0 == old_vl.count(pl)) {
			// 플레이어의 시야에 등장
			clients[pl].send_add_player_packet(npc._id);
		}
		else {
			// 플레이어가 계속 보고 있음.
			clients[pl].send_move_packet(npc._id);
		}

		if (clients[pl].tCharData.hp > 0) {
			if (npc._BestList.empty())
			{
				if (can_see(npc_id, pl)) {
					TIMER_EVENT ev{ npc_id, chrono::system_clock::now(), EV_FIND, pl };
					timer_queue.push(ev);
				}
			}
			else  if (true == can_attack(npc_id, pl))
			{
				TIMER_EVENT ev{ npc_id, chrono::system_clock::now(), EV_ATTACK, pl };
				timer_queue.push(ev);
			}
		}
		else {

			if (!npc._BestList.empty())
				npc._BestList.clear();
		}
	}
	///vvcxxccxvvdsvdvds
	for (auto pl : old_vl) {
		if (0 == new_vl.count(pl)) {
			if (0 != clients[pl]._view_list.count(npc._id)) {
				clients[pl].send_remove_player_packet(npc._id);
			}
		}
	}

}

void do_npc_attack(int npc_id) {

	if (clients[npc_id].waitAttack >= chrono::system_clock::now())
		return;

	SESSION& npc = clients[npc_id];

	unordered_set<int> atk_list;
	concurrency::concurrent_unordered_set<int> dmgList;
	if (clients[npc_id].eMonsterMoveType == MON_STAY) {
		if (npc._is_dmg == true)
		{
			for (auto& obj : clients) {
				if (ST_INGAME != obj._state) continue;
				if (true == is_npc(obj._id)) continue;
				if (true == can_range(npc._id, obj._id))
				{
					if (clients[obj._id].tCharData.hp > 0)
						atk_list.insert(obj._id);
				}
				else {
					npc._is_dmg = false;
				}
			}
		}
	}
	else
	{
		for (auto& obj : clients) {
			if (ST_INGAME != obj._state) continue;
			if (true == is_npc(obj._id)) continue;
			if (true == can_attack(npc._id, obj._id))
			{
				if (clients[obj._id].tCharData.hp > 0)
					atk_list.insert(obj._id);
			}
		}
	}
	string s;
	for (auto& iter : atk_list) {
		if (!clients[iter].tCharData.IsDead(npc.tCharData.atk)) {
			dmgList.insert(iter);
		}
		else {
			wait_dead_time_user_queue.push(ALIVE_USER{ iter,chrono::system_clock::now() + 5s });
			dmgList.insert(iter);
		}
	}

	for (auto& id : dmgList) {
		for (int i = 0; i < MAX_USER; ++i) {
			if (clients[i]._state != ST_INGAME) continue;
			clients[i].send_change_charDataPacket(id);
		}
	}

	npc.waitAttack = chrono::system_clock::now() + 1s;
}

void worker_thread(HANDLE h_iocp)
{
	while (true) {
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
		OVER_EXP* ex_over = reinterpret_cast<OVER_EXP*>(over);
		int KEY = (int)key;

		if (FALSE == ret) {
			if (ex_over->_comp_type == OP_ACCEPT) cout << "Accept Error";
			else {
				cout << "GQCS Error on client[" << key << "]\n";
				disconnect(static_cast<int>(key));
				if (ex_over->_comp_type == OP_SEND) delete ex_over;
				continue;
			}
		}

		if ((0 == num_bytes) && ((ex_over->_comp_type == OP_RECV) || (ex_over->_comp_type == OP_SEND))) {
			disconnect(KEY);
			if (ex_over->_comp_type == OP_SEND) delete ex_over;
			continue;
		}

		switch (ex_over->_comp_type) {
		case OP_ACCEPT: {
			int client_id = get_new_client_id();
			if (client_id != -1) {
				{
					lock_guard<mutex> ll(clients[client_id]._s_lock);
					clients[client_id]._state = ST_ALLOC;
				}
				clients[client_id].x = 0;
				clients[client_id].y = 0;
				clients[client_id]._id = client_id;
				clients[client_id]._name[0] = 0;
				clients[client_id]._prev_remain = 0;
				clients[client_id]._socket = g_c_socket;
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_c_socket),
					h_iocp, client_id, 0);
				clients[client_id].do_recv();
				g_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			}
			else {
				cout << "Max user exceeded.\n";
			}
			ZeroMemory(&g_a_over._over, sizeof(g_a_over._over));

			BOOL bAccept = AcceptEx(g_s_socket, g_c_socket, g_a_over._send_buf, 0, 
				g_ADDR_SIZE, g_ADDR_SIZE, 0, &g_a_over._over);
			break;
		}
		case OP_RECV: {
			int remain_data = num_bytes + clients[key]._prev_remain;
			char* p = ex_over->_send_buf;
			int packet_size{  };
			while (remain_data > packet_size) {
				process_packet(KEY, p);
				packet_size = p[0];

				if (packet_size <= 0)
					break;

				p = p + packet_size;
				remain_data -= packet_size;
			}
			clients[key]._prev_remain = remain_data;
			if (remain_data > 0) {
				memcpy(ex_over->_send_buf, p, remain_data);
			}
			clients[key].do_recv();
			break;
		}
		case OP_SEND:
			delete ex_over;
			break;
		case OP_NPC_MOVE: {
			bool keep_alive = false;
			for (int j = 0; j < MAX_USER; ++j) {
				if (clients[j]._state != ST_INGAME) continue;
				if (can_see(KEY, j)) {
					keep_alive = true;
					break;
				}
			}
			if (true == keep_alive) {
				do_npc_random_move(KEY);
				TIMER_EVENT ev{ KEY, chrono::system_clock::now() + 1s, EV_RANDOM_MOVE, 0 };
				timer_queue.push(ev);
			}
			else {
				clients[KEY]._is_active = false;
			}
			delete ex_over;
		}
			break;
		case OP_NPC_ATTACK:
		{
			if (clients[KEY]._state == ST_FREE)
				return;

			bool keep_alive = false;
			for (int j = 0; j < MAX_USER; ++j) {
				if (clients[j]._state != ST_INGAME) continue;
				if (can_see(KEY, j)) {
					keep_alive = true;
					break;
				}
			}
			if (true == keep_alive) {
				if(clients[KEY].waitAttack <= chrono::system_clock::now())
					do_npc_attack(KEY);

				TIMER_EVENT ev{ KEY, chrono::system_clock::now() + 1s, EV_ATTACK, 0 };
				timer_queue.push(ev);
			}
			else {
				clients[KEY]._is_active = false;
			}
		}
		break;
		case OP_NPC_FIND: {
		}
			break;

		}
	}
}

int API_get_x(lua_State* L)
{
	int user_id =
		(int)lua_tointeger(L, -1);
	lua_pop(L, 2);
	int x = clients[user_id].x;
	lua_pushnumber(L, x);
	return 1;
}

int API_get_y(lua_State* L)
{
	int user_id =
		(int)lua_tointeger(L, -1);
	lua_pop(L, 2);
	int y = clients[user_id].y;
	lua_pushnumber(L, y);
	return 1;
}

int API_SendMessage(lua_State* L)
{
	int my_id = (int)lua_tointeger(L, -3);
	int user_id = (int)lua_tointeger(L, -2);
	char* mess = (char*)lua_tostring(L, -1);

	lua_pop(L, 4);

	clients[user_id].send_chat_packet(my_id, mess);
	std::cout << mess << "\n";
	return 0;
}

int API_CallMove(lua_State* L) {
	int npc_id = (int)lua_tointeger(L, -2);
	int waker = (int)lua_tointeger(L, -1);
	lua_pop(L, 3);

	return 1;
}

int API_MoveEnd(lua_State* L) {
	std::cout << "MoveEnd" << "\n";
	int npc_id = (int)lua_tointeger(L, -2);
	int waker = (int)lua_tointeger(L, -1);
	lua_pop(L, 3);

	//clients[npc_id]._ll.lock();
	clients[npc_id].moveState = 0;
	//clients[npc_id]._ll.unlock();
	return 1;
}

void InitializeNPC()
{
	uniform_int_distribution<int> MonsterMoveState{1, 10};
	uniform_int_distribution<int> MonsterLevel{1, 200'000};

	m_LuaContainer.LoadLuaScript("NPC");
	lua_State* pMonster = m_LuaContainer.GetLuaState("NPC");
	{
		lua_register(pMonster, "API_SendMessage", API_SendMessage);
		lua_register(pMonster, "API_get_x", API_get_x);
		lua_register(pMonster, "API_get_y", API_get_y);
		lua_register(pMonster, "API_CallMove", API_CallMove);
		lua_register(pMonster, "API_MoveEnd", API_MoveEnd);
	}
	
	for (int i = MAX_USER; i < MAX_USER + MAX_NPC;) {
		if (false == OutRandomPos(clients[i].x, clients[i].y)) {
			continue;
		}
		clients[i].startX = clients[i].x - 20 <= 0 ? 0 : clients[i].x - 20;
		clients[i].startY = clients[i].y - 20 <= 0 ? 0 : clients[i].y - 20;
		clients[i].endX = clients[i].x + 20 > W_WIDTH ? 0 : clients[i].x + 20;
		clients[i].endY = clients[i].y + 20 > W_HEIGHT ? 0 : clients[i].y + 20;
		clients[i]._id = i;
		sprintf_s(clients[i]._name, "NPC%d", i);
		clients[i]._state = ST_INGAME;
		clients[i].eMonsterMoveType = MonsterMoveState(dre) <= 4 ? MON_MOVE : MON_STAY;

		int Level = MonsterLevel(dre);
		if (Level <= 100'000) {
			clients[i].tCharData.UpdateData(1);
		}
		else if (Level <= 120'000) {
			clients[i].tCharData.UpdateData(2);
		}
		else if (Level <= 140'000) {
			clients[i].tCharData.UpdateData(3);
		}
		else if (Level <= 160'000) {
			clients[i].tCharData.UpdateData(4);
		}
		else if (Level <= 170'000) {
			clients[i].tCharData.UpdateData(5);
		}
		else if (Level <= 180'000) {
			clients[i].tCharData.UpdateData(6);
		}
		else if (Level <= 190'000) {
			clients[i].tCharData.UpdateData(7);
		}
		else if (Level <= 195'000) {
			clients[i].tCharData.UpdateData(8);
		}
		else if (Level <= 197'500) {
			clients[i].tCharData.UpdateData(9);
		}
		else if (Level <= 200'000) {
			clients[i].tCharData.UpdateData(10);
		}

		if (clients[i].eMonsterMoveType == MON_STAY) {
			clients[i].tCharData.range = (short)(2 + clients[i].tCharData.level * 0.5);
		}
		else {
			clients[i].tCharData.range = 3;
		}

		clients[i]._L = pMonster;
		++i;
	}
}

void do_timer()
{
	while (true) {
		TIMER_EVENT ev;
		auto current_time = chrono::system_clock::now();
		if (true == timer_queue.try_pop(ev)) {
			if (ev.wakeup_time > current_time) {
				auto MTime = ev.wakeup_time - current_time;
				this_thread::sleep_for(MTime);
			}
			switch (ev.event_id) {
			case EV_RANDOM_MOVE:
			{
				OVER_EXP* ov = new OVER_EXP;
				ov->_comp_type = OP_NPC_MOVE;
				PostQueuedCompletionStatus(h_iocp, 1, ev.obj_id, &ov->_over);
			}
			break;
			case EV_FIND:
			{
				ASTAR_EVENT astar = { ev.obj_id, ev.target_id };
				astar_queue.push(astar);
			}
			break;
			case EV_ATTACK:
			{
				OVER_EXP* ov = new OVER_EXP;
				ov->_comp_type = OP_NPC_ATTACK;
				PostQueuedCompletionStatus(h_iocp, 1, ev.obj_id, &ov->_over);
			}
				break;
			}
			continue;		// 즉시 다음 작업 꺼내기
		}
		this_thread::sleep_for(1ms);   // timer_queue가 비어 있으니 잠시 기다렸다가 다시 시작
	}
}

void do_AStar() {

	while (true) {

		ASTAR_EVENT ev;
		if (astar_queue.try_pop(ev)) {
			m_PathFinder.StartAStar(clients[ev.obj_id].x, clients[ev.obj_id].y,
				clients[ev.target_id].x, clients[ev.target_id].y);
			m_PathFinder.AddBestList(clients[ev.obj_id]._BestList);
			continue;
		}
		this_thread::sleep_for(1ms);   // timer_queue가 비어 있으니 잠시 기다렸다가 다시 시작
	}
}

void do_DataBase() {
	while (true) {
		DATABASE_EVENT ev;
		auto current_time = chrono::system_clock::now();
		if (true == database_queue.try_pop(ev)) {
			if (ev.wakeup_time > current_time) {
				auto MTime = ev.wakeup_time - current_time;
				this_thread::sleep_for(MTime);
			}
			if (clients[ev.obj_id]._state == ST_INGAME)
			{
				switch (ev.eType) {
				case DB_CHNAGE:
				{
					m_DataBase.UpdateProcedureData(clients[ev.obj_id].tCharData.id, clients[ev.obj_id]._name, clients[ev.obj_id].x, clients[ev.obj_id].y, clients[ev.obj_id].tCharData);
				}
				break;
				}

				ev.wakeup_time = chrono::system_clock::now() + 30s;
				database_queue.push(ev);
			}
			continue;
		}
		this_thread::sleep_for(1ms);   // timer_queue가 비어 있으니 잠시 기다렸다가 다시 시작
	}
}

void do_alive_user() {

	while (true)
	{
		{
			ALIVE_USER ev;
			auto current_time = chrono::system_clock::now();
			if (true == wait_dead_time_user_queue.try_pop(ev)) {
				if (ev.wakeup_time <= current_time)
				{
					{
				//		lock_guard<mutex> LOCK{WAIT_DEAD_TIME_LOCK};
						clients[ev.obj_id].tCharData.Alive();
						clients[ev.obj_id]._state = ST_INGAME;
					}

					for (int i = 0; i < MAX_USER; ++i) {
						if (clients[i]._state != ST_INGAME) continue;
						clients[i].send_change_charDataPacket(ev.obj_id);
					}
				}
				else {
					wait_dead_time_user_queue.push(ev);
					this_thread::sleep_for(1ms);
				}
				continue;
			}
		}
		this_thread::sleep_for(1ms);
	}
}

int main()
{
	WSADATA WSAData;
	int iError = WSAStartup(MAKEWORD(2, 0), &WSAData);
	if (0 != iError)
		return -1;
	g_s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	if (::bind(g_s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)))
		return -1;
	if (::listen(g_s_socket, SOMAXCONN))
		return -1;

	SOCKADDR_IN cl_addr;
	g_ADDR_SIZE = sizeof(cl_addr) + 16;

	{
		// Ready Data Base
		RETURN_CHECK_FAILED(m_DataBase.ReadyDataBase(), E_FAIL);
		/*m_DataBase.DeleteAllData();

		for (int i = 1; i <= 10000;) {
			CHARDATA tCharData;
			tCharData.PlayerUpdateData(1);

			string PlayerData = "Player";
			PlayerData += to_string(i);

			short x = 0, y = 0;
			if (false == OutRandomPos(x, y)) {
				continue;
			} 

			m_DataBase.InsertProcedureData(i, PlayerData.c_str(), x, y, tCharData);
			++i;
		}*/
	}

	{
		// Load MapData
		std::ifstream read{"MapData.dat"};
		if (!read)
			return E_FAIL;

		read.read((char*)&m_MapData, sizeof(m_MapData));
	}
	{
		// Ready PathFinder
		m_PathFinder.ReadyPathFinder(m_MapData, W_WIDTH, W_HEIGHT);
	}

	InitializeNPC();

	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_s_socket), h_iocp, 9999, 0);
	g_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_a_over._comp_type = OP_ACCEPT;
	BOOL bTrue = AcceptEx(g_s_socket, g_c_socket, 
		g_a_over._send_buf, 0, g_ADDR_SIZE, g_ADDR_SIZE, 0, &g_a_over._over);

	vector <thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency();

	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back(worker_thread, h_iocp);
	
	vector <thread> threads;
	threads.emplace_back(do_timer);
	threads.emplace_back(do_AStar);
	threads.emplace_back(do_DataBase);
	threads.emplace_back(do_alive_user);
	for (auto& iter : threads)
		iter.join();

	for (auto& th : worker_threads)
		th.join();
	closesocket(g_s_socket);
	WSACleanup();
}
