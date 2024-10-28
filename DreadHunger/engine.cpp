#include "engine.h"
#include "utils.h"

std::string FNameEntry::String()
{
	if (bIsWide) { return std::string(); }
	return { AnsiName, Len };
}

FNameEntry* FNamePool::GetEntry(FNameEntryHandle handle) const
{
	return reinterpret_cast<FNameEntry*>(Blocks[handle.Block] + 2 * static_cast<uint64_t>(handle.Offset));
}

std::string FName::GetName()
{
	auto entry = NamePoolData->GetEntry(Index);
	auto name = entry->String();
	if (Number > 0)
	{
		name += '_' + std::to_string(Number);
	}
	auto pos = name.rfind('/');
	if (pos != std::string::npos)
	{
		name = name.substr(pos + 1);
	}
	return name;
}

std::string UObject::GetName()
{
	return NamePrivate.GetName();
}

std::string UObject::GetFullName()
{
	std::string name;
	for (auto outer = OuterPrivate; outer; outer = outer->OuterPrivate) { name = outer->GetName() + "." + name; }
	name = ClassPrivate->GetName() + " " + name + this->GetName();
	return name;
}
/*
bool UObject::IsA(void* cmp)
{
	for (auto super = ClassPrivate; super; super = static_cast<UClass*>(super->SuperStruct)) { if (super == cmp) { return true; } }
	return false;
}*/

FMatrix FTransform::ToMatrixWithScale()
{
	FMatrix m;

	m.M[3][0] = Translation.X;
	m.M[3][1] = Translation.Y;
	m.M[3][2] = Translation.Z;

	float x2 = Rotation.X + Rotation.X;
	float y2 = Rotation.Y + Rotation.Y;
	float z2 = Rotation.Z + Rotation.Z;

	float xx2 = Rotation.X * x2;
	float yy2 = Rotation.Y * y2;
	float zz2 = Rotation.Z * z2;
	m.M[0][0] = (1.0f - (yy2 + zz2)) * Scale3D.X;
	m.M[1][1] = (1.0f - (xx2 + zz2)) * Scale3D.Y;
	m.M[2][2] = (1.0f - (xx2 + yy2)) * Scale3D.Z;


	float yz2 = Rotation.Y * z2;
	float wx2 = Rotation.W * x2;
	m.M[2][1] = (yz2 - wx2) * Scale3D.Z;
	m.M[1][2] = (yz2 + wx2) * Scale3D.Y;


	float xy2 = Rotation.X * y2;
	float wz2 = Rotation.W * z2;
	m.M[1][0] = (xy2 - wz2) * Scale3D.Y;
	m.M[0][1] = (xy2 + wz2) * Scale3D.X;


	float xz2 = Rotation.X * z2;
	float wy2 = Rotation.W * y2;
	m.M[2][0] = (xz2 + wy2) * Scale3D.Z;
	m.M[0][2] = (xz2 - wy2) * Scale3D.X;

	m.M[0][3] = 0.0f;
	m.M[1][3] = 0.0f;
	m.M[2][3] = 0.0f;
	m.M[3][3] = 1.0f;

	return m;
};

void UObject::ProcessEvent(void* fn, void* parms)
{
	auto vtable = *reinterpret_cast<void***>(this);
	reinterpret_cast<void(*)(void*, void*, void*)>(vtable[0x43])(this, fn, parms);
}

UObject* TUObjectArray::GetObjectPtr(uint32_t id) const
{
	if (id >= NumElements) return nullptr;
	uint64_t chunkIndex = id / 65536;
	if (chunkIndex >= NumChunks) return nullptr;
	auto chunk = Objects[chunkIndex];
	if (!chunk) return nullptr;
	uint32_t withinChunkIndex = id % 65536 * 24;
	auto item = *reinterpret_cast<UObject**>(chunk + withinChunkIndex);
	return item;
}

UObject* TUObjectArray::FindObject(const char* name) const
{
	for (auto i = 0u; i < NumElements; i++)
	{
		auto object = GetObjectPtr(i);
		if (object && object->GetFullName() == name) { return object; }
	}
	return nullptr;
}

UEngine** Engine = nullptr;
TUObjectArray* ObjObjects = nullptr;
FNamePool* NamePoolData = nullptr;

void UCanvas::K2_DrawText(void* RenderFont, FString& RenderText, FVector2D& ScreenPosition, FVector2D& Scale, FLinearColor& RenderColor, float Kerning, FLinearColor& ShadowColor, FVector2D& ShadowOffset, bool bCentreX, bool bCentreY, bool bOutlined, FLinearColor& OutlineColor)
{
	static auto fn = ObjObjects->FindObject("Function Engine.Canvas.K2_DrawText");
	struct {
		void* RenderFont;
		FString RenderText;
		FVector2D ScreenPosition;
		FVector2D Scale;
		FLinearColor RenderColor;
		float Kerning;
		FLinearColor ShadowColor;
		FVector2D ShadowOffset;
		bool bCentreX;
		bool bCentreY;
		bool bOutlined;
		FLinearColor OutlineColor;
	} parms;
	parms = { RenderFont , RenderText, ScreenPosition, Scale, RenderColor, Kerning, ShadowColor, ShadowOffset, bCentreX, bCentreY, bOutlined, OutlineColor};
	ProcessEvent(fn, &parms);
}

void UCanvas::K2_DrawBox(FVector2D& ScreenPosition, FVector2D& ScreenSize, float Thickness, FLinearColor& RenderColor)
{
	static auto fn = ObjObjects->FindObject("Function Engine.Canvas.K2_DrawBox");
	struct {
		FVector2D ScreenPosition;
		FVector2D ScreenSize;
		float Thickness;
		FLinearColor RenderColor;
	} parms;
	parms = { ScreenPosition, ScreenSize, Thickness, RenderColor };
	ProcessEvent(fn, &parms);
}

void UCanvas::K2_DrawLine(FVector2D& ScreenPositionA, FVector2D& ScreenPositionB, float Thickness, FLinearColor& RenderColor) {
	static auto fn = ObjObjects->FindObject("Function Engine.Canvas.K2_DrawLine");
	struct {
		FVector2D ScreenPosition;
		FVector2D ScreenSize;
		float Thickness;
		FLinearColor RenderColor;
	} parms;
	parms = { ScreenPositionA,ScreenPositionB,Thickness,RenderColor };
	ProcessEvent(fn, &parms);
}

FVector AActor::K2_GetActorLocation()
{
	static auto fn = ObjObjects->FindObject("Function Engine.Actor.K2_GetActorLocation");
	struct {
		FVector ReturnValue;
	} parms;
	ProcessEvent(fn, &parms);
	return parms.ReturnValue;
}

APawn* AActor::GetInstigator() {
	static auto fn = ObjObjects->FindObject("Function Engine.Actor.GetInstigator");
	struct {
		APawn* ReturnValue;
	} parms;
	ProcessEvent(fn, &parms);
	return parms.ReturnValue;
	
}


/*
float AActor::GetDistanceTo(void* OtherActor) {
	
}*/

APawn* AController::K2_GetPawn()
{
	static auto fn = ObjObjects->FindObject("Function Engine.Controller.K2_GetPawn");
	struct {
		APawn* ReturnValue;
	} parms;
	ProcessEvent(fn, &parms);
	return parms.ReturnValue;
}

bool APawn::IsControlled() {
	static auto fn = ObjObjects->FindObject("Function Engine.Pawn.IsControlled");
	struct {
		bool* ReturnValue;
	} parms;
	ProcessEvent(fn, &parms);
	return parms.ReturnValue;
}

bool APlayerController::ProjectWorldLocationToScreen(FVector& WorldLocation, FVector2D& ScreenLocation, bool bPlayerViewportRelative)
{
	static auto fn = ObjObjects->FindObject("Function Engine.PlayerController.ProjectWorldLocationToScreen");
	struct {
		FVector WorldLocation;
		FVector2D ScreenLocation;
		bool bPlayerViewportRelative;
		bool ReturnValue;
	} parms;
	parms = { WorldLocation, ScreenLocation, bPlayerViewportRelative };
	ProcessEvent(fn, &parms);
	ScreenLocation = parms.ScreenLocation;
	return parms.ReturnValue;
}

bool EngineInit()
{
	auto main = GetModuleHandleA(nullptr);

	static byte engineSig[] = { 0x48, 0x89, 0x05, 0xB3, 0x9B, 0x63, 0x04, 0x48, 0x85, 0xC9, 0x74, 0x05, 0xE8, 0x71, 0xEB, 0xA3, 0x00 };
	Engine = reinterpret_cast<decltype(Engine)>(FindPointer(main, engineSig, sizeof(engineSig), 0));
	if (!Engine) return false;
	
	static byte objSig[] = { 0x48, 0x8B, 0x05, 0x55, 0xB0, 0x4F, 0x04, 0x48, 0x8B, 0x0C, 0xC8 };
	ObjObjects = reinterpret_cast<decltype(ObjObjects)>(FindPointer(main, objSig, sizeof(objSig), 0));
	if (!ObjObjects) return false;

	static byte poolSig[] = { 0x48, 0x8D, 0x0D, 0xD1, 0x3B, 0x9B, 0x03, 0xE8, 0x7C };
	NamePoolData = reinterpret_cast<decltype(NamePoolData)>(FindPointer(main, poolSig, sizeof(poolSig), 0));
	if (!NamePoolData) return false;
	//static byte	

	return true;
}