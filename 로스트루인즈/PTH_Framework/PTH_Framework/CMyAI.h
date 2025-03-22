#pragma once
#include "Include.h"

class CMyAI abstract
{
protected:
	explicit CMyAI();
	virtual ~CMyAI();
protected:
public:
	virtual int Update() PURE;
	virtual void Late_Update() PURE;
	virtual void Relase() PURE;
protected:
public:
};

