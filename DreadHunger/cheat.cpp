#pragma once
#include "engine.h"
#include "utils.h"
#include "cheat.h"


void** Address;
void* Font;

void(__thiscall* PostRenderOriginal)(UGameViewportClient* Viewport, UCanvas* Canvas);

int Skeleton[][2] =
{
	{ boneWolf::HEAD, boneWolf::SPINE1 },{ boneWolf::SPINE1, boneWolf::FRONTLEG_L },{ boneWolf::FRONTLEG_L, boneWolf::FRONTHEEL_L },{ boneWolf::FRONTHEEL_L, boneWolf::FRONTTOE_L } ,
	{ boneWolf::SPINE1, boneWolf::FRONTLEG_R },{ boneWolf::FRONTLEG_R,boneWolf::FRONTHEEL_R},{boneWolf::FRONTHEEL_R,boneWolf::FRONTTOE_R },{boneWolf::SPINE1, boneWolf::SPINE2},
	{ boneWolf::SPINE2, boneWolf::BACKLEG_L },{ boneWolf::BACKLEG_L,  boneWolf::BACKHEEL_L},{ boneWolf::BACKHEEL_L, boneWolf::BACKTOE_L},{ boneWolf::SPINE2, boneWolf::BACKLEG_R},
	{ boneWolf::BACKLEG_R, boneWolf::BACKHEEL_R},{ boneWolf::BACKHEEL_R, boneWolf::BACKTOE_R},{ boneWolf::SPINE2, boneWolf::TAIL1},{ boneWolf::TAIL1, boneWolf::TAIL2},
};

FTransform ReadFTransform(DWORD* ptr, int index)
{
	ptr = ptr + index * 0xc;
	auto vtable = reinterpret_cast<float*>(ptr);

	float quatX = *(vtable + 0x0);
	float quatY = *(vtable + 0x1);
	float quatZ = *(vtable + 0x2);
	float quatW = *(vtable + 0x3);

	float transX = *(vtable + 0x4);
	float transY = *(vtable + 0x5);
	float transZ = *(vtable + 0x6);

	float scaleX = *(vtable + 0x8);
	float scaleY = *(vtable + 0x9);
	float scaleZ = *(vtable + 0xa);

	char bc2[15];

	FQuat fQuat = FQuat(quatX, quatY, quatZ, quatW);
	FVector trans = FVector(transX, transY, transZ);
	FVector scale = FVector(scaleX, scaleY, scaleZ);
	FTransform fTransform = FTransform(fQuat, trans, scale);
	return fTransform;
}

void hookPostRender(UGameViewportClient* Viewport, UCanvas* Canvas) {
	do{
		auto world = Viewport->World;
		if (!world) break;

		auto level = world->PersistentLevel;
		if (!level) break;

		auto instance = world->OwningGameInstance;
		if (!instance) break;

		auto player = instance->LocalPlayers.Data[0];
		if (!player) break;

		auto playerController = player->PlayerController;
		if (!playerController) break;

		auto playerCamera = playerController->PlayerCameraManager;
		if (!playerCamera) break;

		auto cameraLocation = playerCamera->K2_GetActorLocation();

		auto playerPawn = playerController->K2_GetPawn();
		if (!playerPawn) break;

		auto actors = level->Actors;

		FVector2D scale{ 1,1 };
		FLinearColor color{ .23f, .45f, 1.f, 1.f };
		
		FString txt;
		wchar_t text[] = L"hello";
		txt.Data = text;
		txt.Count = 6;
		txt.Size = 9;

		FVector2D screen1{ 600,20 };

		Canvas->K2_DrawText(Font, txt, screen1, scale, color, 1, color, screen1, true, true, true, color);

		for (int i = 0; i < actors.Count; i++) {
			auto actor = actors.Data[i];
			if (!actor) continue;;

			if (actor->GetInstigator()) {
				auto pawn = actor->GetInstigator();
				if (!pawn) continue;;

				auto controller = pawn->Controller;
				if (!controller) continue;;

				auto character = controller->Character;
				if (!character) continue;;

				if (pawn == playerPawn) {
					auto movecomponent = character->CharacterMovement;
					if (!movecomponent) continue;;
					//set player speed
					movecomponent->MaxWalkSpeed = 1500;
					movecomponent->MaxSwimSpeed = 1500;
				}
				else {
					auto mesh = character->Mesh;
					DWORD64* offset_mesh = reinterpret_cast<DWORD64*>(mesh);
					auto c2W = reinterpret_cast<float*>(offset_mesh + 0x38);
					auto bonesArray = *reinterpret_cast<DWORD**>(offset_mesh + 0x96);
					FQuat q(*c2W, *(c2W + 0x1), *(c2W + 0x2), *(c2W + 0x3));
					FVector v1(*(c2W + 0x4), *(c2W + 0x5), *(c2W + 0x6));
					FVector v2(*(c2W + 0x8), *(c2W + 0x9), *(c2W + 0xa));
					FTransform component2World(q, v1, v2);
					FMatrix component2WorldMatrix = component2World.ToMatrixWithScale();

					wchar_t buf[6];
					swprintf(buf, sizeof(buf) / sizeof(*buf), L"%d", i);
					txt.Data = buf;
					txt.Count = 7;
					txt.Size = 10;

					FVector location = actor->K2_GetActorLocation();
					if (i == 215){
						for (int j = 0; j < 16; j++) {//get BoneNode
							FMatrix boneMatrix = ReadFTransform(bonesArray, Skeleton[j][0]).ToMatrixWithScale();// +0xc
							FMatrix newMatrix = boneMatrix * component2WorldMatrix;
							FVector bonePos(
								newMatrix.M[3][0],
								newMatrix.M[3][1],
								newMatrix.M[3][2]);
							FVector2D start, end;
							if (playerController->ProjectWorldLocationToScreen(bonePos, start, false)) {
								boneMatrix = ReadFTransform(bonesArray, Skeleton[j][1]).ToMatrixWithScale();// +0xc
								newMatrix = boneMatrix * component2WorldMatrix;
								FVector bonePos(
									newMatrix.M[3][0],
									newMatrix.M[3][1],
									newMatrix.M[3][2]);
								playerController->ProjectWorldLocationToScreen(bonePos, end, false);
								Canvas->K2_DrawLine(start, end, 1, color);
							}
							//215 wolf 212 bear
						}
					}
					else {
						FVector2D screen;
						playerController->ProjectWorldLocationToScreen(location, screen, false);
						Canvas->K2_DrawText(Font, txt, screen, scale, color, 1, color, screen, true, true, false, color);
					}
				}
			}
		}

	} while (false);
	
	PostRenderOriginal(Viewport, Canvas);
};

/*
Fatal error!

Unhandled Exception: EXCEPTION_ACCESS_VIOLATION 0x00007ff99cec1235

0x00007ff99cec1235 UnknownFunction []
0x00007ff623a5ed11 DreadHunger-Win64-Shipping.exe!UnknownFunction []
0x00007ff621f825ae DreadHunger-Win64-Shipping.exe!UnknownFunction []
0x00007ff623e65011 DreadHunger-Win64-Shipping.exe!UnknownFunction []
0x00007ff623a456bd DreadHunger-Win64-Shipping.exe!UnknownFunction []
0x00007ff623a496e7 DreadHunger-Win64-Shipping.exe!UnknownFunction []
0x00007ff6217dc3d4 DreadHunger-Win64-Shipping.exe!UnknownFunction []
0x00007ff6217e29ed DreadHunger-Win64-Shipping.exe!UnknownFunction []
0x00007ff6217e2a5a DreadHunger-Win64-Shipping.exe!UnknownFunction []
0x00007ff6217ecfdf DreadHunger-Win64-Shipping.exe!UnknownFunction []
0x00007ff62476276e DreadHunger-Win64-Shipping.exe!UnknownFunction []
0x00007ff9b32f7c24 KERNEL32.DLL!UnknownFunction []
0x00007ff9b344d4d1 ntdll.dll!UnknownFunction []

*/

bool CheatInit() {
	auto engine = *Engine;
	if (!engine) return false;

	Font = engine->TinyFont;
	if (!Font) return false;

	auto viewport = engine->GameViewport;
	if (!viewport) return false;

	auto vftable = viewport->VFTable;
	if (!vftable) return false;

	auto Address = vftable + 0x62;

	PostRenderOriginal = reinterpret_cast<decltype(PostRenderOriginal)>(Address[0]);

	auto hook = &hookPostRender;
	return PatchMem(Address, &hook, 8);
}

bool removeCheat() {
	return PatchMem(Address, &PostRenderOriginal, 8);
}