#pragma once
#include <cstdint>
#include <windows.h>
#include <string>
#undef DrawText

struct FVector {
	float X, Y, Z;

	FVector() : X(0.f), Y(0.f), Z(0.f) {}
	FVector(float X, float Y, float Z) : X(X), Y(Y), Z(Z) {}
	FVector(float InF) : X(InF), Y(InF), Z(InF) { }
	float Size() const { return sqrtf(X * X + Y * Y + Z * Z); }
	float DistTo(const FVector& V) const { return (*this - V).Size(); }
	FVector operator-(const FVector& other) const { return FVector(X - other.X, Y - other.Y, Z - other.Z); }
};

struct FPlane : public FVector {
	float W;

	FPlane() {};
	FPlane(float InX, float InY, float InZ, float InW) 
		: FVector(InX, InY, InZ)
		, W(InW) {};
};

struct FQuat {
	float W, X, Y, Z;
	FQuat() : X(0.f), Y(0.f), Z(0.f), W(0.f) {}
	FQuat(float X, float Y, float Z, float W) : X(X), Y(Y), Z(Z), W(W) {}
};

struct FVector2D {
	float X, Y;
};

struct FRotator {
	float Pitch, Yaw, Roll;
};

struct FLinearColor {
	float R, G, B, A;
	FLinearColor() : R(0.f), G(0.f), B(0.f), A(0.f) {};
	FLinearColor(float R, float G, float B, float A) : R(R), G(G), B(B), A(A) {};
};

template<typename T>
struct TArray {
	T* Data;
	int Count;
	int Size;
};

struct FString : TArray<wchar_t> {};

struct FNameEntryHandle {
	uint32_t Block = 0;
	uint32_t Offset = 0;

	FNameEntryHandle(uint32_t block, uint32_t offset) : Block(block), Offset(offset) {};
	FNameEntryHandle(uint32_t id) : Block(id >> 16), Offset(id & 65535) {};
	operator uint32_t() const { return (Block << 16 | Offset); }
};

struct FNameEntry {
	uint16_t bIsWide : 1;
	uint16_t LowercaseProbeHash : 5;
	uint16_t Len : 10;
	union
	{
		char AnsiName[1024];
		wchar_t	WideName[1024];
	};

	std::string String();
};

struct FNamePool 
{
	byte Lock[8];
	uint32_t CurrentBlock;
	uint32_t CurrentByteCursor;
	byte* Blocks[8192];

	FNameEntry* GetEntry(FNameEntryHandle handle) const;
};

struct FName {
	uint32_t Index;
	uint32_t Number;

	std::string GetName();
};

struct FMatrix
{
	float M[4][4];
	FMatrix() {
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				M[i][j] = 0;
	};
	FMatrix(const FPlane& InX, const FPlane& InY, const FPlane& InZ, const FPlane& InW)
	{
		M[0][0] = InX.X; M[0][1] = InX.Y;  M[0][2] = InX.Z;  M[0][3] = InX.W;
		M[1][0] = InY.X; M[1][1] = InY.Y;  M[1][2] = InY.Z;  M[1][3] = InY.W;
		M[2][0] = InZ.X; M[2][1] = InZ.Y;  M[2][2] = InZ.Z;  M[2][3] = InZ.W;
		M[3][0] = InW.X; M[3][1] = InW.Y;  M[3][2] = InW.Z;  M[3][3] = InW.W;
	}

	FMatrix operator*(const FMatrix Other) const
	{
		FMatrix Result;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				for (int k = 0; k < 4; k++) {
					Result.M[i][j] += (*this).M[i][k] * Other.M[k][j];//遍历a矩阵各行与b矩阵各列  }   
				}
			}
		}
		return Result;
	}
};

struct FTransform
{
	FQuat Rotation;
	FVector Translation;
	FVector Scale3D;
	FTransform()
		: Rotation(0.f, 0.f, 0.f, 1.f)
		, Translation(0.f)
		, Scale3D(1.f, 1.f, 1.f)
	{};
	FTransform(FQuat rot, FVector translation, FVector scale) : Rotation(rot), Translation(translation), Scale3D(scale) {};
	struct FMatrix ToMatrixWithScale();
};

struct UObject {
	void** VFTable;
	uint32_t ObjectFlags;
	uint32_t InternalIndex;
	struct UClass* ClassPrivate;
	FName NamePrivate;
	UObject* OuterPrivate;

	std::string GetName();
	std::string GetFullName();
	//bool IsA(void* cmp);
	void ProcessEvent(void* fn, void* parms);
};

// Class CoreUObject.Field
// Size: 0x30 (Inherited: 0x28)
struct UField : UObject {
	char UnknownData_28[0x8]; // 0x28(0x08)
};

// Class CoreUObject.Struct
// Size: 0xb0 (Inherited: 0x30)
struct UStruct : UField {
	char UnknownData_30[0x80]; // 0x30(0x80)
	/*UStruct* SuperStruct; // 0x40(0x8)
	char UnknownData_48[0x68]; // 0x48(0x80)*/
};

// Class CoreUObject.Class
// Size: 0x230 (Inherited: 0xb0)
struct UClass : UStruct {
	char UnknownData_B0[0x180]; // 0xb0(0x180)
};

struct TUObjectArray {
	byte** Objects;
	byte* PreAllocatedObjects;
	uint32_t MaxElements;
	uint32_t NumElements;
	uint32_t MaxChunks;
	uint32_t NumChunks;

	UObject* GetObjectPtr(uint32_t id) const;
	UObject* FindObject(const char* name) const;
};

// Class Engine.Canvas
// Size: 0x2c0 (Inherited: 0x28)
struct UCanvas : UObject {
	char pad_28[0xAA8];// 0x28(0xAA8)

	void K2_DrawText(void* RenderFont, FString& RenderText, FVector2D& ScreenPosition, FVector2D& Scale, FLinearColor& RenderColor, float Kerning, FLinearColor& ShadowColor, FVector2D& ShadowOffset, bool bCentreX, bool bCentreY, bool bOutlined, FLinearColor& OutlineColor); // Function Engine.Canvas.K2_DrawText
	void K2_DrawBox(FVector2D& ScreenPosition, FVector2D& ScreenSize, float Thickness, FLinearColor& RenderColor);//Function Engine.Canvas.K2_DrawBox
	void K2_DrawLine(FVector2D& ScreenPositionA, FVector2D& ScreenPositionB, float Thickness, FLinearColor& RenderColor); // Function Engine.Canvas.K2_DrawLine
};

// Class Engine.Actor
// Size: 0x220 (Inherited: 0x28)
struct AActor : UObject {
	char pad_28[0xF0]; // 0x28(0x1F8)
	struct APawn* Instigator;//0x118(0x08)
	char pad_120[0x100]; // 0x120(0xF8)

	FVector K2_GetActorLocation();
	APawn* GetInstigator();
	//float GetDistanceTo(void* OtherActor); // Function Engine.Actor.GetDistanceTo
};

// Class Engine.Pawn
// Size: 0x280 (Inherited: 0x220)
struct APawn : AActor {
	char pad_220[0x38];
	struct AController* Controller; // 0x258(0x08)
	char pad_260[0x20]; // 0x260(0x20)

	bool IsControlled();
};

// Class Engine.PlayerCameraManager
// Size: 0x2740 (Inherited: 0x220)
struct APlayerCameraManager : AActor {
	char pad_220[0x2590]; // 0x220
};

// Class Engine.Controller
// Size: 0x298 (Inherited: 0x220)
struct AController : AActor {
	char pad_220[0x40];
	struct ACharacter* Character; // 0x260(0x08)
	char pad_268[0x30];

	struct APawn* K2_GetPawn();
};

struct ACharacter : APawn {
	struct USkeletalMeshComponent* Mesh; // 0x280(0x08)
	struct UCharacterMovementComponent* CharacterMovement; // 0x288(0x028)
	char pad_290[0x230];
};

struct UCharacterMovementComponent : UObject {
	char pad_28[0x128]; // 0x28(0x10)
	float GravityScale; // 0x150(0x04)
	char pad_154[0x38];
	float MaxWalkSpeed; // 0x18c(0x04)
	float MaxWalkSpeedCrouched; // 0x190(0x04)
	float MaxSwimSpeed; // 0x194(0x04)
	float MaxFlySpeed; // 0x198(0x04)
	float MaxCustomMovementSpeed; // 0x19c(0x04)
	float MaxAcceleration; // 0x1a0(0x04)
	float MinAnalogWalkSpeed; // 0x1a4(0x04)
	char pad_1A8[0x1E4]; // 0x1A8(0x1E4)
	char bCheatFlying : 1; // 0x38c(0x01)
};

// Class Engine.PlayerController
// Size: 0x570 (Inherited: 0x298)
struct APlayerController : AController {
	char pad_298[0x20]; // 0x298(0x08)
	struct APlayerCameraManager* PlayerCameraManager; // 0x2b8(0x08)

	bool ProjectWorldLocationToScreen(struct FVector& WorldLocation, struct FVector2D& ScreenLocation, bool bPlayerViewportRelative); // Function Engine.PlayerController.ProjectWorldLocationToScreen
};

// Class Engine.Player
// Size: 0x48 (Inherited: 0x28)
struct UPlayer : UObject {
	char UnknownData_28[0x8]; // 0x28(0x08)
	struct APlayerController* PlayerController; // 0x30(0x08)
};

// Class Engine.PlayerState
// Size: 0x320 (Inherited: 0x220)
struct APlayerState : AActor {
	char pad_220[0x60]; // 0x220(0x04)
	struct APawn* PawnPrivate; // 0x280(0x08)
	char pad_288[0x78]; // 0x288
	struct FString PlayerNamePrivate; // 0x300(0x10)
};

// Class Engine.GameStateBase
// Size: 0x270 (Inherited: 0x220)
struct AGameStateBase : AActor {
	char pad_220[0x18]; // 0x220
	struct TArray<struct APlayerState*> PlayerArray; // 0x238(0x10)
};

// Class Engine.GameInstance
// Size: 0x198 (Inherited: 0x28)
struct UGameInstance : UObject {
	char UnknownData_28[0x10]; // 0x28(0x10)
	struct TArray<struct UPlayer*> LocalPlayers; // 0x38(0x10)
};

// Class Engine.Level
// Size: 0x298 (Inherited: 0x28)
struct ULevel : UObject {
	char pad_28[0x70];
	struct TArray<struct AActor*> Actors;
};

// Class Engine.World
// Size: 0x710 (Inherited: 0x28)
struct UWorld : UObject {
	char pad_28[0x8]; // 0x28(0x08)
	struct ULevel* PersistentLevel;
	char pad_38[0xE8];
	struct AGameStateBase* GameState; // 0x120(0x08)
	char pad_0x138[0x58]; // 0x128(0x50)
	struct UGameInstance* OwningGameInstance; // 0x180(0x08)
};

// Class Engine.ScriptViewportClient
// Size: 0x38 (Inherited: 0x28)
struct UScriptViewportClient : UObject {
	char UnknownData_28[0x10]; // 0x28(0x10)
};

// Class Engine.GameViewportClient
// Size: 0x330 (Inherited: 0x38)
struct UGameViewportClient : UScriptViewportClient {
	char UnknownData_38[0x40]; // 0x38(0x08)
	struct UWorld* World; // 0x78(0x08)
	struct UGameInstance* GameInstance; // 0x80(0x08)
	char UnknownData_88[0x2d8]; // 0x88(0x2a8)
};

// Class Engine.Engine
// Size: 0xde0 (Inherited: 0x28)
struct UEngine : UObject {
	char UnknownData_28[0x8]; // 0x28(0x08)
	struct UFont* TinyFont; // 0x30(0x08)
	char pad_38[0x18]; // 0x38(0x18)
	struct UFont* SmallFont; // 0x50(0x08)
	char pad_58[0x18]; // 0x58(0x18)
	struct UFont* MediumFont; // 0x70(0x08)
	char pad_78[0x18]; // 0x78(0x18)
	struct UFont* LargeFont; // 0x90(0x08)
	char pad_98[0x18]; // 0x98(0x18)
	struct UFont* SubtitleFont; // 0xb0(0x08)
	char pad_b8[0x6C8]; // 0xb8
	struct UGameViewportClient* GameViewport; // 0x780(0x08)
};

struct UMeshComponent : UObject {
	char pad_28[0x458];
};

struct USkinnedMeshComponent : UMeshComponent {
	struct USkeletalMesh* SkeletalMesh; // 0x480(0x08)
	char pad_488[0x218];
	
};

struct USkeletalMeshComponent : USkinnedMeshComponent {
	char pad_6A0[0x830];
};

extern UEngine** Engine;
extern FNamePool* NamePoolData;
extern TUObjectArray* ObjObjects;

bool EngineInit();