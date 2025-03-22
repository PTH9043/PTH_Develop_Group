#define SFML_STATIC 1
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <unordered_map>
#include <Windows.h>
#include <chrono>
#include <array>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <fstream>
#include <format>
using namespace std;

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "ws2_32.lib")

#include "..\..\protocol_2023.h"

sf::TcpSocket s_socket;

constexpr auto SCREEN_WIDTH = 16;
constexpr auto SCREEN_HEIGHT = 16;

constexpr auto TILE_WIDTH = 65;
constexpr auto WINDOW_WIDTH = SCREEN_WIDTH * TILE_WIDTH;   // size of window
constexpr auto WINDOW_HEIGHT = SCREEN_WIDTH * TILE_WIDTH;

using _byte = signed char;
using MAPDATA = std::array<std::array<_byte, W_WIDTH>, W_HEIGHT>;

int g_left_x;
int g_top_y;
int g_myid;

sf::RenderWindow* g_window;
sf::Font g_font;
sf::Texture* g_ChattingImage;
sf::Sprite g_Image;
sf::Sprite g_WindowImage;
sf::Text text;
sf::Texture* g_AttackTexture;

char g_attackDirection;

void send_packet(void* packet);


struct CHAT {
	sf::Text text;
	std::string str{""};
};

const int CHAT_NUM = 10;
std::array<CHAT, CHAT_NUM> g_ChatArr{};

MAPDATA	g_MapData{};

bool bJoin{ false };
bool bChattingMode{ false };

class OBJECT {
protected:
	bool m_showing;
	sf::Sprite m_sprite;

	sf::Text m_name;
	sf::Text m_chat;
	chrono::system_clock::time_point m_mess_end_time;
	char m_MoveType = TYPE_MOVE;
public:
	int id;
	int m_x, m_y;
	short m_dirX, m_dirY;
	char name[NAME_SIZE];
	OBJECT(sf::Texture& t, int x, int y, int x2, int y2) {
		m_showing = false;
		m_sprite.setTexture(t);
		m_sprite.setTextureRect(sf::IntRect(x, y, x2, y2));
		set_name("NONAME");
		m_mess_end_time = chrono::system_clock::now();
	}
	OBJECT() {
		m_showing = false;
	}
	void show()
	{
		m_showing = true;
	}
	void hide()
	{
		m_showing = false;
	}

	void a_move(int x, int y) {
		m_sprite.setPosition((float)x, (float)y);
	}

	void a_draw() {
		g_window->draw(m_sprite);
	}

	void setMoveType(char _moveType) { this->m_MoveType = _moveType; }

	void move(int x, int y, short _dirX, short _dirY) {
		m_x = x;
		m_y = y;
		m_dirX = _dirX;
		m_dirY = _dirY;
	}
	virtual void draw() {
		if (false == m_showing) return;
		float rx = (m_x - g_left_x) * 65.0f + 1;
		float ry = (m_y - g_top_y) * 65.0f + 1;
		m_sprite.setPosition(rx, ry);
		g_window->draw(m_sprite);
		auto size = m_name.getGlobalBounds();
		if (m_mess_end_time < chrono::system_clock::now()) {
			m_name.setPosition(rx + 32 - size.width / 2, ry - 10);
			g_window->draw(m_name);
		}
		else {
			m_chat.setPosition(rx + 32 - size.width / 2, ry - 10);
			g_window->draw(m_chat);
		}
	}
	void set_name(const char str[]) {
		m_name.setFont(g_font);
		m_name.setString(str);
		if (id < MAX_USER) m_name.setFillColor(sf::Color(255, 255, 255));
		else m_name.setFillColor(sf::Color(255, 255, 0));
		m_name.setStyle(sf::Text::Bold);
	}
	void set_name(const std::string& str) {
		m_name.setFont(g_font);
		m_name.setString(str);
		if (id < MAX_USER) m_name.setFillColor(sf::Color(255, 255, 255));
		else m_name.setFillColor(sf::Color(255, 255, 0));
		m_name.setStyle(sf::Text::Bold);
	}
	 void set_chat(const char str[]) {
		m_chat.setFont(g_font);
		m_chat.setString(str);
		m_chat.setFillColor(sf::Color(255, 255, 255));
		m_chat.setStyle(sf::Text::Bold);
		m_mess_end_time = chrono::system_clock::now() + chrono::seconds(3);
	}
	 void set_chat(const std::string& str) {
		 m_chat.setFont(g_font);
		 m_chat.setString(str);
		 m_chat.setFillColor(sf::Color(255, 255, 255));
		 m_chat.setStyle(sf::Text::Bold);
		 m_mess_end_time = chrono::system_clock::now() + chrono::seconds(3);
	 }
};

class CHARACTER : public OBJECT {
private:
	vector<sf::Sprite> m_ShowSpriteVector;
	short m_Size{ 0 };
	bool m_dead{ false };
public:
	CHARDATA tCharData{};
	std::string ObjectInfoStr;
	CHARACTER(sf::Texture& t, int x, int y, int x2, int y2, const CHARDATA& _tData = {}) : 
	tCharData(_tData), OBJECT() {
		m_sprite.setTexture(t);
		m_sprite.setTextureRect(sf::IntRect(x, y, x2, y2));
		set_name("NONAME");
		m_mess_end_time = chrono::system_clock::now();
	}
	CHARACTER() : OBJECT() {
	}
	~CHARACTER() {
	}

	void makeShowSprite(sf::Texture& _pTexture, short _size) {
		if (TYPE_MOVE == m_MoveType) {
			return;
		}
		else
		{
			m_ShowSpriteVector.resize(1);
			m_Size = _size;
			m_ShowSpriteVector[0].setScale(m_Size, m_Size);
		}

		for (auto& iter : m_ShowSpriteVector) {
			iter.setTexture(_pTexture);
			iter.setTextureRect(sf::IntRect(0, 0, 65, 65));
			sf::Color col = iter.getColor();
			col.a = 100;
			iter.setColor(col);
		}
	}

	void makeShowSprite_Player(sf::Texture& _pTexture, short _size) {
		m_ShowSpriteVector.resize(_size);
		m_Size = _size;

		for (auto& iter : m_ShowSpriteVector) {
			iter.setTexture(_pTexture);
			iter.setTextureRect(sf::IntRect(0, 0, 65, 65));
			sf::Color col = iter.getColor();
			col.a = 100;
			iter.setColor(col);
		}
	}

	void makeObjectStr(CHARDATA* _tData) {
		::memcpy(&tCharData, _tData, sizeof(CHARDATA));

		ObjectInfoStr = std::format("LEVEL:{:1} HP:{:1}\nNAME:{:1}", 
			tCharData.level, tCharData.hp, name);
		set_name(ObjectInfoStr);
	}

	void makeObjectStr() {
		ObjectInfoStr = std::format("LEVEL:{:1} HP:{:1}\nNAME:{:1}",
			tCharData.level, tCharData.hp, name);
		set_name(ObjectInfoStr);
	}

	virtual void draw() {
		if (false == m_showing) return;
		float rx = (m_x - g_left_x) * 65.0f + 1;
		float ry = (m_y - g_top_y) * 65.0f + 1;
		
		if (TYPE_MOVE == m_MoveType) {
			int i = 1;
			for (auto& iter : m_ShowSpriteVector) {
				iter.setPosition(rx + 65 * m_dirX * i, ry + 65 * m_dirY * i);
				++i;
				g_window->draw(iter);
			}
		}
		else {
			for (auto& iter : m_ShowSpriteVector) {
				iter.setPosition(rx - 45 * iter.getScale().x * 0.5f, ry - 45 * iter.getScale().y * 0.5f);
				g_window->draw(iter);
			}
		}
		m_sprite.setPosition(rx, ry);
		g_window->draw(m_sprite);
		auto size = m_name.getGlobalBounds();
		if (m_mess_end_time < chrono::system_clock::now()) {
			m_name.setPosition(rx + 32 - size.width / 2, ry - 10);
			g_window->draw(m_name);
		}
		else {
			m_chat.setPosition(rx + 32 - size.width / 2, ry - 10);
			g_window->draw(m_chat);
		}
	}

	void setDead(const bool _dead) { this->m_dead = _dead; }
	const bool IsDead() const { return m_dead; }
};

CHARACTER avatar;
unordered_map <int, CHARACTER> players;

OBJECT white_tile;
OBJECT black_tile;
OBJECT Wall_Tile;
OBJECT FileAttack[2];

sf::Texture* board;
sf::Texture* pieces;
sf::Texture* Wall;
sf::Texture* Show;
sf::Texture* PlayerAttackRange;

void EnterChat() {
	g_ChatArr[CHAT_NUM - 1].str.clear();
	string value = g_ChatArr[0].str;
	for (int i = 1; i < CHAT_NUM ; ++i) {
		string str = g_ChatArr[i].str;
		g_ChatArr[i].str = value;
		value = str;
	}
	CS_CHAT_PACKET p;
	p.size = sizeof(p);
	p.type = CS_CHAT;
	memcpy(p.mess, g_ChatArr[0].str.c_str(), CHAT_SIZE -1);
	send_packet(&p);
}

void EnterChat(const string& _str) {
	g_ChatArr[CHAT_NUM - 1].str.clear();
	string value = g_ChatArr[0].str;
	for (int i = 1; i < CHAT_NUM; ++i) {
		string str = g_ChatArr[i].str;
		g_ChatArr[i].str = value;
		value = str;
	}
	g_ChatArr[0].str = _str;
}


void client_initialize()
{
	board = new sf::Texture;
	pieces = new sf::Texture;
	Wall = new sf::Texture;
	g_AttackTexture = new sf::Texture;
	g_ChattingImage = new sf::Texture;
	Show = new sf::Texture;
	PlayerAttackRange = new sf::Texture;

	board->loadFromFile("chessmap.bmp");
	pieces->loadFromFile("chess2.png");
	Wall->loadFromFile("Wall.png");
	Show->loadFromFile("Show.png");
	PlayerAttackRange->loadFromFile("PlayerAttackRange.png");

	g_ChattingImage->loadFromFile("Road.png");
	g_AttackTexture->loadFromFile("Fire.jpg");

	g_Image.setTexture(*g_ChattingImage);
	g_Image.setTextureRect(sf::IntRect(0, 0, g_ChattingImage->getSize().x, g_ChattingImage->getSize().y));
	g_Image.setPosition(750, 750);
	g_Image.setScale(0.75f, 0.75f);
	sf::Color col = g_Image.getColor();
	col.a = 100;
	g_Image.setColor(col);
	if (false == g_font.loadFromFile("cour.ttf")) {
		cout << "Font Loading Error!\n";
		exit(-1);
	}
	white_tile = OBJECT{ *board, 5, 5, TILE_WIDTH, TILE_WIDTH };
	black_tile = OBJECT{ *board, 69, 5, TILE_WIDTH, TILE_WIDTH };
	Wall_Tile = OBJECT(*Wall, 0, 0, 5, 5);
	for (int i = 0; i < 2; ++i) {
		FileAttack[i] = OBJECT(*g_AttackTexture, 0, 0, g_AttackTexture->getSize().x,
			g_AttackTexture->getSize().y);
		FileAttack[i].show();
	}
	avatar = CHARACTER{ *pieces, 128, 0, 64, 64 };
	avatar.show();

	for (auto& iter : g_ChatArr) {
		iter.text.setFont(g_font);
		iter.text.setFillColor(sf::Color(0, 0, 0, 255));
		iter.text.setCharacterSize(15);
		iter.text.setLineSpacing(100.f);
	}
}

void client_finish()
{
	players.clear();
	delete board;
	delete pieces;
	delete Wall;
	delete g_ChattingImage;
	delete g_AttackTexture;
	delete Show;
	delete PlayerAttackRange;
}

void ProcessPacket(char* ptr)
{
	static bool first_time = true;
	switch (ptr[2])
	{
	case SC_LOGIN_INFO: 
	{
		SC_LOGIN_INFO_PACKET* packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(ptr);
		g_myid = packet->id;
		avatar.id = g_myid;
		avatar.move(packet->x, packet->y, 0, 0);
		g_left_x = packet->x - SCREEN_WIDTH / 2;
		g_top_y = packet->y - SCREEN_HEIGHT / 2;
		::memcpy(avatar.name, packet->name, NAME_SIZE);
		bJoin = true;
		avatar.show();
		avatar.makeObjectStr(&packet->tCharData);
		avatar.makeShowSprite_Player(*PlayerAttackRange, (short)packet->tCharData.speed);
	}
	break;

	case SC_ADD_OBJECT:
	{
		SC_ADD_OBJECT_PACKET* my_packet = reinterpret_cast<SC_ADD_OBJECT_PACKET*>(ptr);
		int id = my_packet->id;

		if (id == g_myid) {
			avatar.move(my_packet->x, my_packet->y, my_packet->dirX, my_packet->dirY);
			g_left_x = my_packet->x - SCREEN_WIDTH / 2;
			g_top_y = my_packet->y - SCREEN_HEIGHT / 2;
			avatar.show();
		}
		else if (id < MAX_USER) {
			players[id] = CHARACTER{ *pieces, 0, 0, 64, 64 };
			players[id].id = id;
			players[id].move(my_packet->x, my_packet->y, my_packet->dirX, my_packet->dirY);
			::memcpy(players[id].name, my_packet->name, NAME_SIZE);
			players[id].show();
			players[id].makeObjectStr(&my_packet->tCharData);
			players[id].makeShowSprite_Player(*PlayerAttackRange, (short)my_packet->tCharData.speed);
		}
		else {
			if (my_packet->moveType == TYPE_MOVE) {
				players[id] = CHARACTER{ *pieces, 307, 0, 64, 64 };
			}
			else {
				players[id] = CHARACTER{ *pieces, 256, 0, 64, 64 };
			}
			players[id].id = id;
			players[id].setMoveType(my_packet->moveType);
			players[id].move(my_packet->x, my_packet->y, my_packet->dirX, my_packet->dirY);
			::memcpy(players[id].name, my_packet->name, NAME_SIZE);
			players[id].show();
 			players[id].makeObjectStr(&my_packet->tCharData);
			players[id].makeShowSprite(*Show, (short)my_packet->tCharData.range);
		}
		break;
	}
	case SC_MOVE_OBJECT:
	{
		SC_MOVE_OBJECT_PACKET* my_packet = reinterpret_cast<SC_MOVE_OBJECT_PACKET*>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			avatar.move(my_packet->x, my_packet->y, my_packet->dirX, my_packet->dirY);
			g_left_x = my_packet->x - SCREEN_WIDTH / 2;
			g_top_y = my_packet->y - SCREEN_HEIGHT / 2;
		}
		else {
			players[other_id].move(my_packet->x, my_packet->y, my_packet->dirX, my_packet->dirY);
		}
		break;
	}

	case SC_REMOVE_OBJECT:
	{
		SC_REMOVE_OBJECT_PACKET* my_packet = reinterpret_cast<SC_REMOVE_OBJECT_PACKET*>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			avatar.hide();
		}
		else {
			players.erase(other_id);
		}
		break;
	}
	case SC_CHAT:
	{
		SC_CHAT_PACKET* my_packet = reinterpret_cast<SC_CHAT_PACKET*>(ptr);
		if(my_packet->id != g_myid)
		EnterChat(my_packet->mess);
	}
	break;
	case SC_LOGIN_OK:
	{
		SC_LOGIN_OK_PACKET* packet = reinterpret_cast<SC_LOGIN_OK_PACKET*>(ptr);
		{
			bJoin = true;
		}
	}
	break;
	case SC_LOGIN_FAIL:
	{
		// Login Failed
		cout << "로그인에 실패했습니다." << endl;
	}
	break;
	case SC_STAT_CHANGE:
	{
		SC_STAT_CHANGE_PACKET* packet = reinterpret_cast<SC_STAT_CHANGE_PACKET*>(ptr);
		if (packet->id == g_myid) {
			avatar.tCharData.SetData(packet->tCharData);
			avatar.makeObjectStr();
			avatar.makeShowSprite(*PlayerAttackRange, (short)packet->tCharData.speed);
		}
		else {
			players[packet->id].tCharData.SetData(packet->tCharData);
			players[packet->id].makeObjectStr();

			if (packet->id < MAX_USER)
			{
				players[packet->id].makeShowSprite_Player(*PlayerAttackRange, (short)packet->tCharData.speed);
			}
		}
	}
		break;
	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
	}
}

void process_data(char* net_buf, size_t io_byte)
{
	char* ptr = net_buf;
	static size_t in_packet_size = 0;
	static size_t saved_packet_size = 0;
	static char packet_buffer[2000];


	while (0 != io_byte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (in_packet_size >= 500) { in_packet_size = ptr[0]; saved_packet_size = 0; }
		if (io_byte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			io_byte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
			saved_packet_size += io_byte;
			io_byte = 0;
		}
	}
}

void client_main()
{
	char net_buf[BUF_SIZE];
	size_t	received;

	auto recv_result = s_socket.receive(net_buf, BUF_SIZE, received);
	if (recv_result == sf::Socket::Error)
	{
		wcout << L"Recv 에러!";
		exit(-1);
	}
	if (recv_result == sf::Socket::Disconnected) {
	//	wcout << L"Disconnected";
	//	exit(-1);
	}
	if (recv_result != sf::Socket::NotReady)
		if (received > 0) process_data(net_buf, received);

	for (int i = 0; i < SCREEN_WIDTH; ++i)
		for (int j = 0; j < SCREEN_HEIGHT; ++j)
		{
			int tile_x = i + g_left_x;
			int tile_y = j + g_top_y;
			if ((tile_x < 0) || (tile_y < 0)) continue;
			if (g_MapData[tile_y][tile_x] == MAP_BLOCK) {
				Wall_Tile.a_move(TILE_WIDTH * i, TILE_WIDTH * j);
				Wall_Tile.a_draw();
			}
			else
			{
				if (0 == (tile_x / 3 + tile_y / 3) % 2) {
					white_tile.a_move(TILE_WIDTH * i, TILE_WIDTH * j);
					white_tile.a_draw();
				}
				else
				{
					black_tile.a_move(TILE_WIDTH * i, TILE_WIDTH * j);
					black_tile.a_draw();
				}
			}
		}

	avatar.draw();
	for (auto& pl : players) pl.second.draw();
	text.setFont(g_font);
	char buf[100];
	sprintf_s(buf, "(%d, %d)", avatar.m_x, avatar.m_y);
	text.setString(buf);

	int iIndex{ 1 };
	g_window->draw(g_Image);
	for (auto& iter : g_ChatArr) {
		if (!iter.str.empty()) {
			iter.text.setString(iter.str);
			iter.text.setPosition(780, 1000 - iIndex++ * 40.f);
			g_window->draw(iter.text);
		}
	}
	g_window->draw(text);
}

void send_packet(void *packet)
{
	unsigned char *p = reinterpret_cast<unsigned char *>(packet);
	size_t sent = 0;
	s_socket.send(packet, p[0], sent);
}


int main()
{
	WSADATA					m_WsaData{};
	wcout.imbue(locale("korean"));
	//WSAStartup(MAKEWORD(2, 2), &m_WsaData);
	sf::Socket::Status status = s_socket.connect("127.0.0.1", PORT_NUM);
	s_socket.setBlocking(false);

	if (status != sf::Socket::Done) {
		wcout << L"서버와 연결할 수 없습니다.\n";
		exit(-1);
	}

	std::ifstream read{"MapData.dat", std::ios::binary};
	if (!read)
		return 0;

	read.read((char*)&g_MapData, sizeof(_byte) * W_WIDTH * W_HEIGHT);

	int id;
	client_initialize();
	// 로그인 하는 것
	while (!bJoin) {
		char net_buf[BUF_SIZE];
		size_t	received;

		cout << "아이디 : ";
		cin >> id;

		CS_LOGIN_PACKET p;
		p.size = sizeof(p);
		p.type = CS_LOGIN;
		p.id = id;
		send_packet(&p);

		while (true) {
			auto recv_result = s_socket.receive(net_buf, BUF_SIZE, received);
			if (received > 0) {
				process_data(net_buf, received);
				break;
			}
		}
	}

	//CS_LOGIN_PACKET p;
	//p.size = sizeof(p);
	//p.type = CS_SENDCHARDATA;
	//p.id = id;
	//::memcpy(p.name, value.c_str(), sizeof(p.name));
	//send_packet(&p);

	// SendCharData 
	// Ready 2D Client
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "2D CLIENT");
	g_window = &window;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed) {
				int direction = -1;
				switch (event.key.code) {
				case sf::Keyboard::Left:
					direction = 2;
					g_attackDirection = 2;
					break;
				case sf::Keyboard::Right:
					direction = 3;
					g_attackDirection = 3;
					break;
				case sf::Keyboard::Up:
					direction = 0;
					g_attackDirection = 0;
					break;
				case sf::Keyboard::Down:
					direction = 1;
					g_attackDirection = 1;
					break;
				case sf::Keyboard::Escape:
					window.close();
					break;
				case sf::Keyboard::LControl:
				{
					if (!g_ChatArr[0].str.empty()) {
						EnterChat("");
					}
					bChattingMode = !bChattingMode;
					continue;
				}
					break;
				case sf::Keyboard::A:
					g_attackDirection = 2;
					break;
				case sf::Keyboard::S:
					g_attackDirection = 1;
					break;
				case sf::Keyboard::D:
					g_attackDirection = 3;
					break;
				case sf::Keyboard::W:
					g_attackDirection = 0;
					break;
				case sf::Keyboard::Space:
				{
					CS_ATTACK_PACKET tPacket;
					tPacket.size = sizeof(CS_ATTACK_PACKET);
					tPacket.type = CS_ATTACK;
					tPacket.direction = g_attackDirection;
					send_packet(&tPacket);
				}
					break;
				}
				if (false == bChattingMode) {
					if (-1 != direction) {
						CS_MOVE_PACKET p;
						p.size = sizeof(p);
						p.type = CS_MOVE;
						p.direction = direction;
						send_packet(&p);
					}
				}
				else {
					if (event.type == sf::Event::KeyPressed) {

						switch (event.key.code) {
						case sf::Keyboard::Enter:
						{
							EnterChat();
							g_ChatArr[0].str.clear();
							continue;
						}
							break;
						case sf::Keyboard::LControl:
						{
							bChattingMode = !bChattingMode;
							continue;
						}
						break;
						case sf::Keyboard::BackSpace:
							g_ChatArr[0].str.pop_back();
							break;
						}

						if (g_ChatArr[0].str.size() <= 200) {
							if (sf::Keyboard::Space != event.key.code) {
								g_ChatArr[0].str += event.key.code + 97;
							}
							else {
								g_ChatArr[0].str += " ";
							}
						}
					}
				}
			}
		}

		window.clear();
		client_main();
		window.display();
	}
	client_finish();

	return 0;
}