#include "Inagaki/GameAudioCommon.h"
#include "Inagaki/GameAudioCamera.h"
#include "Inagaki/GameAudioMain.h"
#include "Inagaki/GameSoundMgr.h"
#include "Kaneshige/Course/CrsArea.h"
#include "JSystem/JAudio/JASFakeMatch2.h"

namespace GameAudio {

// TODO
static const u8 cKartRankClassTable0[7] = {0, 0, 1, 1, 2, 2, 2};
static const u8 cKartRankClassTable1[7] = {0, 0, 1, 1, 2, 2, 2};

u32 BoundSe[0x19];
u32 WheelSpinSe[0x19];
u32 SpinSe[0x19];
u32 SpinTurnSe[0x19];

f32 GA_ENEMY_VOLUME_DOWN_VALUE = 0.85f;
const f32 EngineKeisuuRaceUp[] = {
    0.006f, 0.006f, 0.005f, 0.003f, 
    0.015f, 0.015f, 0.013f, 0.011f, 
    0.02f,
};

const f32 EngineKeisuuRaceDown[] = {
    0.005f, 0.003f, 0.005f, 0.002f, 
    0.012f, 0.012f, 0.012f, 0.009f, 
    0.019f,
};

u8 KartSoundMgr::smKartCount;
u8 KartSoundMgr::smEntryKartCount;
u8 KartSoundMgr::smGoalKartCount;

u8 KartSoundMgr::smKartRankClassMem[7] = {};

KartSoundMgr::KartSoundMgr(Vec *pos, JKRHeap *heap, u8 p3, u8 p4) : SoundMgr(pos, heap, 12) {
    _114 = 1.f;
    _110 = 1.f;
    _11c = 0;
    _124 = 1.f;
    _120 = 1.f;
    _12c = 0;
    _C = 0xff;

    _61 = p3;

    if(p3 == 0) {
        smKartCount++;
        mKartCount = smKartCount;
    }
    else {
        mKartCount = 4;
    }

    if(p3 != 2) {
        smEntryKartCount++;
    }

    _66 = p4;
    _130 = new CrsArea();
    u8 index = 0;

    if(_66 == 0) {
        _64 = 3;
    }
    else {
        u32 randomValue = GameAudio::Random::getSignalEngineRandomU32();
        u8 rankClassIndex = randomValue % 7;

        while(smKartRankClassMem[rankClassIndex] == 1 && index < 7)
        {
            rankClassIndex = rankClassIndex + 1;
            index++;
            if(rankClassIndex == 7)
            {
                rankClassIndex = 0;
            }
        }

        smKartRankClassMem[rankClassIndex] = 1;
        u8 playerMode = Parameters::getPlayerMode();
        if(playerMode != 1)
        {
            _64 = cKartRankClassTable0[rankClassIndex];
        }
        else {
            _64 = cKartRankClassTable1[rankClassIndex];
        }
    }

    init();

    _7c = 0;

    const u32 sceneMax = Main::getAudio()->getCamera()->getSceneMax();

    if(sceneMax > 1 && sceneMax > mKartCount)
    {
        _7c = (1 << mKartCount) ^ 0xf;
    }
}

KartSoundMgr::~KartSoundMgr() {}

void KartSoundMgr::startSoundHandleNumber(u8, u32, u32) {
    setEcho(NULL, 0);
}

void KartSoundMgr::dispose() {}

void KartSoundMgr::startSoundEngine(u8, u32) {} // UNUSED

void KartSoundMgr::init() {}

void KartSoundMgr::changeAttribute(u8) {} // UNUSED

void KartSoundMgr::changeDriver(bool) {}

void KartSoundMgr::checkCourseSound(u8) {} // UNUSED

void KartSoundMgr::frameWork(u8) {}

void KartSoundMgr::checkAfterGoalVolume() {}

void KartSoundMgr::setHandleVolume(JAISoundHandle&, f32) {} // UNUSED

void KartSoundMgr::setWaterDepth(f32) {} // UNUSED

void KartSoundMgr::setWaterDepth(u8, f32) {}

void KartSoundMgr::setSlip(u8, u8, u8, f32) {}

void KartSoundMgr::setConductStatus(f32, f32, bool, bool, bool, u8, CrsArea *) {}

void KartSoundMgr::setWaterCutoffPort(u16) {}

void KartSoundMgr::getEngineIDOffsetAtt() {} // UNUSED

void KartSoundMgr::countGoalKart() {}

void KartSoundMgr::setConductLocomotiveAccel() {}

void KartSoundMgr::setConductLocomotiveSpeed(bool) {}

void KartSoundMgr::setConductPressed() {}

void KartSoundMgr::setConductSignal() {}

void KartSoundMgr::setConductOutOfCourse(u8) {}

void KartSoundMgr::setConductTrouble(f32, u8) {}

void KartSoundMgr::setConductRace(bool) {}

void KartSoundMgr::setConductAfterGoal(bool) {}

void KartSoundMgr::setCrushSe(CrsGround::EMat, f32) {}

void KartSoundMgr::setCrushSe(u32, f32) {}

void KartSoundMgr::setBrakeSe(u32) {}

void KartSoundMgr::setDashSe(u32) {}

void KartSoundMgr::setMiniturboSe(u32) {}

void KartSoundMgr::setJumpUpSe(u32) {}

void KartSoundMgr::setBoundSe(f32) {}

void KartSoundMgr::setWheelSpinSe() {}

void KartSoundMgr::setSpinSe() {}

void KartSoundMgr::setSpinTurnSe() {}

void KartSoundMgr::setSe(u32) {}

void KartSoundMgr::setChibiPitch(JAISoundHandle *) {}

void KartSoundMgr::adjustEngine() {}

void KartSoundMgr::crushRenzokuTaisaku() {}

void KartSoundMgr::slipParamSet() {}

void KartSoundMgr::checkEcho() {}

void KartSoundMgr::setInvincibleBgm(u8) {}

void KartSoundMgr::clearInvincibleBgm(u8) {}

void KartSoundMgr::setChibiFlag(bool, bool) {}

}
