#pragma once

enum boneWolf : int {
	HEAD = 44,
	SPINE1 = 23,
	SPINE2 = 20,
	TAIL1 = 14,
	TAIL2 = 18,

	FRONTLEG_L = 27,
	FRONTHEEL_L = 28,
	FRONTTOE_L = 29,

	FRONTLEG_R = 35,
	FRONTHEEL_R = 36,
	FRONTTOE_R = 37,

	BACKLEG_L = 3,
	BACKHEEL_L = 4,
	BACKTOE_L = 5,

	BACKLEG_R = 9,
	BACKHEEL_R = 10,
	BACKTOE_R = 11,
};

bool CheatInit();
bool removeCheat();