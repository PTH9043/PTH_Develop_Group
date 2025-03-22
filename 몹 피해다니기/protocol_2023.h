#pragma once



constexpr char MAP_ROAD = 0;
constexpr char MAP_BLOCK = 1;
constexpr char MAP_ITEM = 2;

constexpr int PORT_NUM = 4000;
constexpr int CHAT_SIZE = 100;

constexpr int MAX_USER = 10000;
constexpr int MAX_NPC = 200000;

constexpr int W_WIDTH = 2000;
constexpr int W_HEIGHT = 2000;
constexpr short BUF_SIZE = 300;
constexpr short NAME_SIZE = 50;
constexpr short SHOW_RANGE = 81;

constexpr short OBJTYPE_PLAYER = 1;
constexpr short OBJTYPE_MON = 2;


constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;
constexpr char CS_CHAT = 2;
constexpr char CS_ATTACK = 3;
constexpr char CS_TELEPORT = 4;
constexpr char CS_LOGOUT = 5;
constexpr char CS_MAKEID = 6;
constexpr char CS_SENDCHARDATA = 7;

constexpr char SC_LOGIN_INFO = 2;
constexpr char SC_ADD_OBJECT = 3;
constexpr char SC_REMOVE_OBJECT = 4;
constexpr char SC_MOVE_OBJECT = 5;
constexpr char SC_CHAT = 6;
constexpr char SC_LOGIN_OK = 7;
constexpr char SC_LOGIN_FAIL = 8;
constexpr char SC_STAT_CHANGE = 9;

constexpr char TYPE_NONMOVE = 0;
constexpr char TYPE_MOVE = 1;

struct SENDCHARDATA {
	// 4
	int		id{ 0 };
	// 직업
	char	job{ 0 };
	// 체력
	float	hp{ 0 };
	// 마나
	float	mp{ 0 };
	// 경험치
	float	exp{ 0 };
	// 레벨
	int		level{ 0 };
	// 공격력
	float	atk{ 0 };
	// 스피드
	float	speed{ 0 };
	// 운
	float	luck{ 0 };
	unsigned short	range{ 0 };
};

struct CHARDATA {
	// 4
	int		id{ 0 };
	// 직업
	char	job{ 0 };
	// 체력
	float	hp{ 0 };
	float	max_hp{ 0 };
	// 마나
	float	mp{ 0 };
	float	max_mp{ 0 };
	// 경험치
	float	exp{ 0 };
	float	max_exp{ 0 };
	// 레벨
	int		level{ 0 };
	// 공격력
	float	atk{ 0 };
	// 스피드
	float	speed{ 0 };
	// 운
	float	luck{ 0 };
	unsigned short	range{ 0 };

	void SetData(const SENDCHARDATA& _tData) {
		id = _tData.id;
		job = _tData.job;
		hp = _tData.hp;
		mp = _tData.mp;
		exp = _tData.exp;
		max_hp = _tData.hp;
		max_mp = _tData.mp;
		max_exp = _tData.exp;
		level = _tData.level;
		atk = _tData.atk;
		speed = _tData.speed;
		luck = _tData.luck;
	}

	void UpdateData(const int level, const float _hpValue = 10, const float _mpValue = 10, const float _exp = 5, 
		const float _atk = 3, const float _speed = 1, const float _luck = 1) {
		max_hp = hp = level * _hpValue;
		max_mp = mp = level * _mpValue;
		max_exp = exp  = level * _exp;
		atk = level * _atk;
		speed = level * _speed;
		luck = level * _luck;
		this->level = level;
		range = (short)(1 + level * 0.5);
		speed = (float)range;
	}

	void LevelUp(const float _hpValue = 10, const float _mpValue = 10, const float _exp = 5,
		const float _atk = 3, const float _speed = 1, const float _luck = 1) {
		if (IsExpOver()) {
			max_hp += _hpValue;
			max_mp += _mpValue;
			max_exp += _exp;
			atk += _atk;
			speed += _speed;
			luck += _luck;
			++level;
			range = (short)(1 + level * 0.5);
			speed = (float)range;
		}
	}

	void PlayerUpdateData(const int level, const float _hpValue = 50, const float _mpValue = 15, const float _exp = 10,
		const float _atk = 5, const float _speed = 2, const float _luck = 2) {
		max_hp = hp = level * _hpValue;
		max_mp = mp = level * _mpValue;
		max_exp = level * _exp;
		atk = level * _atk;
		speed = level * _speed;
		luck = level * _luck;
		this->level = level;
		range = (short)(2 + level * 0.5);
		speed = (float)range;
	}

	void PlayerLevelUp(const float _hpValue = 10, const float _mpValue = 10, const float _exp = 5,
		const float _atk = 3, const float _speed = 1, const float _luck = 1) {
		if (IsExpOver()) {
			max_hp += _hpValue;
			hp = max_hp;
			max_mp += _mpValue;
			mp = max_mp;
			max_exp += _exp;
			exp = 0;
			atk += _atk;
			speed += _speed;
			luck += _luck;
			++level;
			range = (short)(2 + level * 0.5);
			speed = (float)range;
		}
	}
	bool IsDead(const float atk) {
		Damaged(atk);
		return hp <= 0;
	}

	void Alive() { hp = max_hp; }
private:
	bool IsExpOver() {
		if (exp > max_exp) {
			exp = 0;
			return true;
		}
		return false;
	}

	void Damaged(const float atk) {
		this->hp -= atk;
	}
};


#pragma pack (push, 1)
struct CS_LOGIN_PACKET {
	unsigned short		size;
	char						type;
	char						name[NAME_SIZE];
	int							id{ 0 };
};

struct CS_MOVE_PACKET {
	unsigned short size;
	char	type;
	char	direction; 
	unsigned	move_time;
};

struct CS_CHAT_PACKET {
	unsigned short size;	
	char	type;
	char	mess[CHAT_SIZE];
};

struct CS_TELEPORT_PACKET {
	unsigned short size;
	char	type;
};

struct CS_LOGOUT_PACKET {
	unsigned short size;
	char	type;
};

struct CS_MAKEID_PACKET {
	unsigned short size;
	char	type;
	int		id;
	char	name[NAME_SIZE];
};

struct CS_ATTACK_PACKET {
	unsigned short size;
	char type;
	char direction;
};

struct CS_SENDSENDCHARDATA_PACKET {
	unsigned short size;
	char	type;
};

struct SC_LOGIN_INFO_PACKET {
	unsigned short size;
	char	type;
	char						name[NAME_SIZE];
	CHARDATA tCharData{};
	short	x, y;
	int id;
};

struct SC_ADD_OBJECT_PACKET {
	unsigned short size;
	char	type;
	int		id;
	short	x, y;
	char	name[NAME_SIZE];
	char	objType{};
	char	moveType;
	short dirX, dirY;
	CHARDATA tCharData{};
};

struct SC_REMOVE_OBJECT_PACKET {
	unsigned short size;
	char	type;
	int		id;
	char	objType{};
};

struct SC_MOVE_OBJECT_PACKET {
	unsigned short size;
	char	type;
	int		id;
	short	x, y;
	unsigned int move_time;
	short dirX, dirY;
};

struct SC_CHAT_PACKET {
	unsigned short size;
	char	type;
	int		id;
	char	mess[CHAT_SIZE];
};

struct SC_LOGIN_OK_PACKET {
	unsigned short size;
	char	type;
};

struct SC_LOGIN_FAIL_PACKET {
	unsigned short size;
	char	type;

};

struct SC_STAT_CHANGE_PACKET {
	unsigned short size;
	char	type;
	int id;
	char checkDead;
	SENDCHARDATA tCharData{};
};

#pragma pack (pop)