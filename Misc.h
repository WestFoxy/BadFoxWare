#pragma once
#include "MainInclude.hpp"
class FoxMisc
{
public:
	void Run();
	CUserCmd* cmd;
private:
	void edgejump();
	void bhop();
	void revealrank();
	void stealname();
	void fastduck();
	void autostrafe();
	void fakeduck();
	void antiafk();
	void clantag();
	void CircleStrafe();
	void NameSpam();
};

extern FoxMisc g_chMisc;