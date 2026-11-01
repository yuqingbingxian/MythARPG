#pragma once
#include "CoreMinimal.h"

/**
 * PC filesystem save store. No UObjects touch worker threads.
 * Alternates two checksummed generations; a damaged newest generation
 * can be recovered from the older one without overwriting it.
 * Old Campaign_00.sav saves are accepted only for migration.
 */
enum class EMythSaveReadStatus : uint8
{
    Missing, Valid, RecoveredOlderGeneration, Corrupt
};

struct MYTHFOUNDATIONRUNTIME_API FMythSaveReadResult
{
    EMythSaveReadStatus Status = EMythSaveReadStatus::Missing;
    TArray<uint8> Bytes;
    uint64 Generation = 0;
    int32 ActiveIndex = INDEX_NONE; // -1 denotes legacy raw UE slot.
};

class MYTHFOUNDATIONRUNTIME_API FMythSaveStorage
{
public:
    /** BasePath is an absolute path WITHOUT extension; never pass an arbitrary user string. */
    static FMythSaveReadResult ReadBest(const FString& BasePath);
    /** Returns false without modifying the last valid generation on an I/O error. */
    static bool WriteNext(const FString& BasePath, const TArray<uint8>& RawSaveBytes);

    // Pure data transforms: unit-testable without creating a game instance.
    static void Wrap(const TArray<uint8>& Payload, uint64 Generation, TArray<uint8>& OutBytes);
    static bool Unwrap(const TArray<uint8>& Bytes, TArray<uint8>& OutPayload, uint64& OutGeneration);
    static bool LooksLikeLegacySave(const TArray<uint8>& Bytes);
    static FString DefaultBasePath();
};
