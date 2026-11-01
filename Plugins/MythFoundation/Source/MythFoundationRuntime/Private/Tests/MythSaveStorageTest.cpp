#include "MythSaveStorage.h"
#include "Misc/AutomationTest.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMythSaveStorageTest,
    "MythARPG.Foundation.Save.TwoGenerationsAndRecovery",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMythSaveStorageTest::RunTest(const FString& Parameters)
{
    TArray<uint8> First = { 'G', 'V', 'A', 'S', 1, 2, 3, 4 };
    TArray<uint8> Second = { 'G', 'V', 'A', 'S', 9, 8, 7, 6 };
    TArray<uint8> Envelope, Payload;
    uint64 Generation = 0;
    FMythSaveStorage::Wrap(First, 7, Envelope);
    TestTrue(TEXT("Envelope round trip"), FMythSaveStorage::Unwrap(Envelope, Payload, Generation));
    TestEqual(TEXT("Generation"), Generation, uint64(7));
    TestTrue(TEXT("Raw bytes preserved"), Payload == First);
    Envelope.Last() ^= 0x7f;
    TestFalse(TEXT("Corrupt payload is rejected"), FMythSaveStorage::Unwrap(Envelope, Payload, Generation));

    const FString TestDir = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("MythStorageTests"), FGuid::NewGuid().ToString());
    const FString Base = FPaths::Combine(TestDir, TEXT("Campaign"));
    TestTrue(TEXT("First generation writes"), FMythSaveStorage::WriteNext(Base, First));
    TestTrue(TEXT("Second generation writes"), FMythSaveStorage::WriteNext(Base, Second));
    FMythSaveReadResult Latest = FMythSaveStorage::ReadBest(Base);
    TestEqual(TEXT("Latest generation"), Latest.Generation, uint64(2));
    TestTrue(TEXT("Latest payload"), Latest.Bytes == Second);
    // Damage newest slot; prior generation must remain intact.
    const FString Broken = Base + TEXT("_B.mythsav");
    TestTrue(TEXT("Corruption injection"), FFileHelper::SaveStringToFile(TEXT("bad"), *Broken));
    Latest = FMythSaveStorage::ReadBest(Base);
    TestTrue(TEXT("Previous generation used"), Latest.Status == EMythSaveReadStatus::RecoveredOlderGeneration);
    TestTrue(TEXT("Previous payload intact"), Latest.Bytes == First);
    TestTrue(TEXT("Write can recover older generation"), FMythSaveStorage::WriteNext(Base, Second));
    Latest = FMythSaveStorage::ReadBest(Base);
    TestTrue(TEXT("Recovered progression"), Latest.Bytes == Second);

    IFileManager::Get().DeleteDirectory(*TestDir, false, true);
    return true;
}
#endif
