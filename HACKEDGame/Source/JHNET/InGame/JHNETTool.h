// Copyrightⓒ2019 by 블랙말랑카우. All Data cannot be copied without permission. 

#pragma once

#include "JHNET.h"

/*	InGame Manager를 찾습니다.
	POINTER_OF_INGAMEMANAGER : InGameManager를 받을 포인터.
*/

#define BindInGameManger(POINTER_OF_INGAMEMANAGER) {\
	auto GAMEINSTANCE = Cast<UJHNETGameInstance>(GetGameInstance());\
	JHNET_CHECK(GAMEINSTANCE);\
	POINTER_OF_INGAMEMANAGER = GAMEINSTANCE->GetInGameManager();\
	JHNET_CHECK(POINTER_OF_INGAMEMANAGER);\
}

/*	Object Asset을 받습니다.
*	생성자 타임에만 작동합니다.
*	RETURN_POINTER : 받을 포인터
*	CLASS_TYPE : 받을 오브젝트의 클래스
*	PATH : 경로 (TEXT를 붙이지 않습니다.)*/
#define GetObjectAsset(RETURN_POINTER, CLASS_TYPE, PATH)\
static ConstructorHelpers::FObjectFinder<CLASS_TYPE> __##RETURN_POINTER(TEXT(PATH));\
if (__##RETURN_POINTER.Succeeded()) {\
	RETURN_POINTER = __##RETURN_POINTER.Object;\
}\
else RETURN_POINTER = nullptr;\
JHNET_CHECK(RETURN_POINTER)


/*	Class Asset을 받습니다.
*	생성자 타임에만 작동합니다.
*	RETURN_POINTER : 받을 포인터
*	CLASS_TYPE : 받을 클래스 타입
*	PATH : 경로 (TEXT를 붙이지 않습니다.)*/
#define GetClassAsset(RETURN_POINTER, CLASS_TYPE, PATH)\
static ConstructorHelpers::FClassFinder<CLASS_TYPE> __##RETURN_POINTER(TEXT(PATH));\
if (__##RETURN_POINTER.Succeeded()) {\
	RETURN_POINTER = __##RETURN_POINTER.Class;\
}\
else RETURN_POINTER = nullptr;\
JHNET_CHECK(RETURN_POINTER)

namespace AIAllStopTool
{
	void AIDisable(APawn* ai, bool bWithoutAuthority = false);
	void AIRestart(APawn* ai, bool bWithoutAuthority = false);
	bool OnAIPlaying(APawn* ai);
}

namespace ActorFinderTool
{
	// 오브젝트가 존재한다면 그대로 찾아오고 없다면 만들어서 리턴하는 함수.
	template<typename T>
	T* CreateOrGetObject(UWorld* worldContext)
	{
		if (!worldContext) return nullptr;
		return CreateOrGetObject<T>(worldContext, T::StaticClass());
	}

	// 오브젝트가 존재한다면 그대로 찾아오고 없다면 만들어서 리턴하는 함수.
	// 직접 스폰할 클래스를 넣을수있다. (블루프린트 클래스를 넣는 것을 권장)
	template<typename T>
	T* CreateOrGetObject(UWorld* worldContext, UClass* spawnClass)
	{
		if (!worldContext) return nullptr;
		TArray<AActor*> outActors;

		UGameplayStatics::GetAllActorsOfClass(worldContext, spawnClass, outActors);

		if (outActors.Num() > 1) {
			// 여러개가 존재한다면 오류 출력.
			FString newLog = TEXT("Only one of ") + spawnClass->GetName() +
				TEXT(" must be placed in level!!!!");
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, newLog);
			JHNET_LOG_STRING(Error, newLog);
			return Cast<T>(outActors[0]);
		}
		else if (outActors.Num() == 1) {
			// 한개만 존재한다면 성공!
			return Cast<T>(outActors[0]);
		}
		else {
			// 한개도 없다면 강제로 스폰해주고 로그 출력.
			FString newLog = TEXT("One of ") + spawnClass->GetName() +
				TEXT(" must be placed in level!!!!");
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, newLog);
			JHNET_LOG_STRING(Error, newLog);

			FActorSpawnParameters spawnParms;
			FTransform spawnTransform = FTransform::Identity;
			spawnTransform.SetLocation(FVector(1000000.0f, 1000000.0f, 1000000.0f));
			spawnParms.bNoFail = true;
			spawnParms.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AActor* retval = worldContext->SpawnActor(spawnClass, &spawnTransform, spawnParms);
			return Cast<T>(retval);
		}
	}
}

namespace ObjectFinderTool
{
	template <typename ObjClass>
	FORCEINLINE ObjClass* LoadObjFromPath(const FName& Path)
	{
		if (Path == NAME_None) return NULL;

		return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), NULL, *Path.ToString()));
	}

	FORCEINLINE USkeletalMesh* GetSKFromPath(FString path)
	{
		return Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, *path));
	}

	FORCEINLINE UStaticMesh* GetSMFromPath(FString path)
	{
		return Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *path));
	}

	template<typename TEnum>
	FORCEINLINE FString EnumDeserializeValueAsString(const FString& Name, TEnum Value)
	{
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
		if (!enumPtr)
		{
			return FString("Invalid");
		}
		return enumPtr->GetNameByValue((int64)Value).ToString();
	}
}

namespace VectorHelper
{
	FORCEINLINE FVector VectorClamp(FVector &v, const FVector &min, const FVector &max)
	{
		v.X = FMath::Clamp<float>(v.X, min.X, max.X);
		v.Y = FMath::Clamp<float>(v.Y, min.Y, max.Y);
		v.Z = FMath::Clamp<float>(v.Z, min.Z, max.Z);

		return v;
	}
	FORCEINLINE FVector VectorLerp(const FVector &start, const FVector &to, const int &alpha)
	{
		FVector newVector;
		newVector.X = FMath::Lerp(start.X, to.X, alpha);
		newVector.Y = FMath::Lerp(start.Y, to.Y, alpha);
		newVector.Z = FMath::Lerp(start.Z, to.Z, alpha);

		return newVector;
	}
}