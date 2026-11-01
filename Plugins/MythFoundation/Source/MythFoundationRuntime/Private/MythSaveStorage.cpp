#include "MythSaveStorage.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/Crc.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"

namespace
{
    constexpr uint32 SaveMagic = 0x4D595448;  // "MYTH" in FArchive representation
    constexpr uint32 FormatVersion = 1;
    constexpr int64 HeaderSize = sizeof(uint32) * 3 + sizeof(uint64) * 2;
    FString PathFor(const FString& Base, int32 Index)
    {
        return Base + (Index == 0 ? TEXT("_A.mythsav") : TEXT("_B.mythsav"));
    }
    bool ReadCandidate(const FString& File, FMythSaveReadResult& Out)
    {
        TArray<uint8> Bytes;
        if (!FFileHelper::LoadFileToArray(Bytes, *File)) return false;
        return FMythSaveStorage::Unwrap(Bytes, Out.Bytes, Out.Generation);
    }
}

FString FMythSaveStorage::DefaultBasePath()
{
    return FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("SaveGames"), TEXT("Campaign_00"));
}

void FMythSaveStorage::Wrap(const TArray<uint8>& Payload, uint64 Generation, TArray<uint8>& OutBytes)
{
    OutBytes.Reset();
    FMemoryWriter Writer(OutBytes, true);
    uint32 Magic = SaveMagic;
    uint32 Version = FormatVersion;
    uint64 Count = static_cast<uint64>(Payload.Num());
    uint32 CRC = FCrc::MemCrc32(Payload.GetData(), Payload.Num());
    Writer << Magic << Version << Generation << Count << CRC;
    if (Payload.Num() > 0) Writer.Serialize(const_cast<uint8*>(Payload.GetData()), Payload.Num());
}

bool FMythSaveStorage::Unwrap(const TArray<uint8>& Bytes, TArray<uint8>& OutPayload, uint64& OutGeneration)
{
    OutPayload.Reset();
    OutGeneration = 0;
    if (Bytes.Num() < HeaderSize) return false;
    FMemoryReader Reader(Bytes, true);
    uint32 Magic = 0, Version = 0, CRC = 0;
    uint64 Generation = 0, Count = 0;
    Reader << Magic << Version << Generation << Count << CRC;
    if (Magic != SaveMagic || Version != FormatVersion || Generation == 0 ||
        Count == 0 || Count > static_cast<uint64>(MAX_int32) ||
        Count != static_cast<uint64>(Bytes.Num() - HeaderSize)) return false;
    OutPayload.SetNumUninitialized(static_cast<int32>(Count));
    Reader.Serialize(OutPayload.GetData(), Count);
    if (Reader.IsError() || FCrc::MemCrc32(OutPayload.GetData(), OutPayload.Num()) != CRC)
    {
        OutPayload.Reset();
        return false;
    }
    OutGeneration = Generation;
    return true;
}

bool FMythSaveStorage::LooksLikeLegacySave(const TArray<uint8>& Bytes)
{
    // Unreal SaveGameToSlot/SaveGameToMemory signature: GVAS.
    return Bytes.Num() >= 4 && Bytes[0] == 'G' && Bytes[1] == 'V' &&
           Bytes[2] == 'A' && Bytes[3] == 'S';
}

FMythSaveReadResult FMythSaveStorage::ReadBest(const FString& BasePath)
{
    IFileManager& FM = IFileManager::Get();
    FMythSaveReadResult Best;
    bool bFoundAny = false;
    bool bInvalidGeneration = false;
    for (int32 Index = 0; Index != 2; ++Index)
    {
        const FString File = PathFor(BasePath, Index);
        if (!FM.FileExists(*File)) continue;
        bFoundAny = true;
        FMythSaveReadResult Candidate;
        if (!ReadCandidate(File, Candidate))
        {
            bInvalidGeneration = true;
            continue;
        }
        if (Best.ActiveIndex == INDEX_NONE || Candidate.Generation > Best.Generation)
        {
            Best = MoveTemp(Candidate);
            Best.ActiveIndex = Index;
        }
    }
    if (Best.ActiveIndex != INDEX_NONE)
    {
        Best.Status = bInvalidGeneration ? EMythSaveReadStatus::RecoveredOlderGeneration
                                         : EMythSaveReadStatus::Valid;
        return Best;
    }

    // Refuse to fall back to a stale legacy save if both modern generations are damaged.
    if (bFoundAny)
    {
        Best.Status = EMythSaveReadStatus::Corrupt;
        return Best;
    }
    // Legacy v0.2/v0.3 save; never delete it as part of migration.
    const FString Legacy = BasePath + TEXT(".sav");
    if (FM.FileExists(*Legacy))
    {
        bFoundAny = true;
        TArray<uint8> Raw;
        if (FFileHelper::LoadFileToArray(Raw, *Legacy) && LooksLikeLegacySave(Raw))
        {
            Best.Bytes = MoveTemp(Raw);
            Best.Status = EMythSaveReadStatus::Valid;
            return Best;
        }
    }
    Best.Status = bFoundAny ? EMythSaveReadStatus::Corrupt : EMythSaveReadStatus::Missing;
    return Best;
}

bool FMythSaveStorage::WriteNext(const FString& BasePath, const TArray<uint8>& RawSaveBytes)
{
    if (RawSaveBytes.IsEmpty() || !LooksLikeLegacySave(RawSaveBytes)) return false;
    IFileManager& FM = IFileManager::Get();
    if (!FM.MakeDirectory(*FPaths::GetPath(BasePath), true)) return false;
    const FMythSaveReadResult Current = ReadBest(BasePath);
    // Refuse to overwrite damaged generations when neither is recoverable.
    if (Current.Status == EMythSaveReadStatus::Corrupt) return false;
    if (Current.Generation == MAX_uint64) return false;
    const int32 NewIndex = Current.ActiveIndex == 0 ? 1 : 0;
    const FString Target = PathFor(BasePath, NewIndex);
    const FString Temp = Target + TEXT(".tmp");
    TArray<uint8> Envelope;
    Wrap(RawSaveBytes, Current.Generation + 1, Envelope);
    if (!FFileHelper::SaveArrayToFile(Envelope, *Temp)) return false;
    TArray<uint8> Verify, Payload;
    uint64 Generation = 0;
    const bool bValid = FFileHelper::LoadFileToArray(Verify, *Temp) &&
        Unwrap(Verify, Payload, Generation) && Generation == Current.Generation + 1 &&
        Payload == RawSaveBytes;
    if (!bValid) { FM.Delete(*Temp); return false; }
    // Only replace the inactive generation. The last known-good slot stays intact.
    if (!FM.Move(*Target, *Temp, true, true, false, true))
    {
        FM.Delete(*Temp);
        return false;
    }
    return true;
}
