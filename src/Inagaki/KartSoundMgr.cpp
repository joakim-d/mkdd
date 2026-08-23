#include "Inagaki/GameSoundMgr.h"

#include "Inagaki/GameAudioAudience.h"
#include "Inagaki/GameAudioCommon.h"
#include "Inagaki/GameAudioCamera.h"
#include "Inagaki/GameAudioMain.h"
#include "Inagaki/GameSoundTable.h"
#include "JSystem/JAudio/Interface/JAISound.h"
#include "JSystem/JAudio/JAUSoundObject.h"
#include "JSystem/JAudio/System/JASGadget.h"
#include "JSystem/JGeometry/Vec.h"
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

const f32 DashEngineAdjustInitialValue[] = {
    0.3f, 0.3f, 0.3f, 0.3f,
    0.3f, 0.3f, 0.3f, 0.3f,
    0.3f
};

const f32 DashEngineIncPerFrame[] = {
    0.025f, 0.025f, 0.025f, 0.095f,
    0.025f, 0.025f, 0.025f, 0.025f,
    0.025f
};

const f32 DashEngineDecPerFrame[] = {
    0.005f, 0.005f, 0.005f, 0.005f,
    0.005f, 0.005f, 0.005f, 0.005f,
    0.005f
};

u8 KartSoundMgr::smKartCount;
u8 KartSoundMgr::smEntryKartCount;
u8 KartSoundMgr::smGoalKartCount;

u8 KartSoundMgr::smKartRankClassMem[7] = {};

KartSoundMgr::KartSoundMgr(Vec *pos, JKRHeap *heap, u8 p3, u8 p4) : SoundMgr(pos, heap, 12) {
    _114 = 1.f;
    _110 = 1.f;
    _11c = 0;
    mCameraVolume = 1.f;
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

KartSoundMgr::~KartSoundMgr() {
    setChibiFlag(false, false);
    for(u8 index = 0; index < 7; index++)
    {
        smKartRankClassMem[index] = 0;
    }

    if(_66 == 0)
    {
        const s32 kartCount = mKartCount;
        if(kartCount < 4)
        {
            CustomAudience<4>::smCameraVolume[kartCount] = 1.f;
        }
    }

    smKartCount = 0;
    smEntryKartCount = 0;
    smGoalKartCount = 0;
}

void KartSoundMgr::startSoundHandleNumber(u8 handleIndex, u32 soundID, u32 fadeCount) {
    if(mKillSw || _66 == 2)
    {
        return;
    }

    JAISoundStarter* soundStarter = JASGlobalInstance<JAISoundStarter>::getInstance();

    JAISoundHandle& handle = (*this)[handleIndex];

    soundStarter->startSound(soundID, &handle, NULL);

    JAISound* sound;

    if(!handle.isSoundAttached())
    {
        return;
    }

    if(handle->audible_ == NULL)
    {
        Main* main = Main::getAudio();
        u32 scene = 0;
        CameraMgr* camera = main->getCamera();
        u32 sceneMax = camera->getSceneMax();

        if(sceneMax > 1 && sceneMax > mKartCount)
        {
            scene = (1 << mKartCount) ^ 0xf;
        }

        sound = handle.operator->();

        JGeometry::TVec3f vec(*mSoundPos);
        sound->newAudible(vec, &_18, scene, NULL);
    }
    sound = handle.operator->();

    sound->fader_.fadeInFromOut2(fadeCount);

    setEcho(&handle, _6c);
}

void KartSoundMgr::dispose() {
    JAUSoundObject::dispose();

    clearInvincibleBgm(3);
}

void KartSoundMgr::startSoundEngine(u8, u32) {} // UNUSED

void KartSoundMgr::init() {
    _60 = 0;
    _5d = 0;
    _68 = 0.f;
    _63 = 0;
    _5c = 0;
    _6c = 0.f;
    _70 = 0.f;
    _80 = 0;
    _84 = 0.f;
    _88 = 0.f;
    _8c = 0;
    _8d = 0;
    _8e = 0;
    _90 = 0;
    _92 = 0;
    _94 = 0;
    _96 = 0;
    _98 = 0.f;
    _9c = 0;
    _a0 = 0.f;

    for(u8 index = 0; index < 4; index++)
    {
        _a4[index] = 0.f;
        _b4[index] = 0.f;
        _c4[index] = 0.f;
        _e4[index] = 0;
        _d4[index] = 0.f;
        _e8[index] = 0;
    }

    _ec = 0.f;
    _f0 = 0.f;
    _f4 = 0;
    _f8 = 0;
    _100 = 0;
    _101 = 0;
    _102 = 0;
    _103 = 0;
    _104 = 0;
    _105 = 1;
    _fc = 0.f;
    _5f = 0;
    _114 = 0.f;
    _110 = 0.f;
    _11c = 0;
    mCameraVolume = 1.f;
    _120 = 1.f;
    _12c = 0.f;
    _78 = 0;
    _5e = 0;
    _74 = 0;
    _65 = 0xff;

    if(_66 == 0)
    {
        const s32 kartCount = mKartCount;
        const f32 volume = mCameraVolume;
        if(kartCount < 4)
        {
            CustomAudience<4>::smCameraVolume[kartCount] = volume;
        }
    }

    Main* main = Main::getAudio();
    CustomSoundTable* soundTable = main->getSoundTable();
    for(s32 index = 0; index < _10; index++)
    {
        if(!(*this)[index].isSoundAttached())
        {
            continue;
        }
        JAISound* sound = (*this)[index].operator->();
        u32 swBit = soundTable->getSwBit(sound->getID().mId.mFullId);
        if(!(swBit & 0x00800000))
        {
            continue;
        }
        (*this)[index]->stop();
    }

    setChibiFlag(false, false);
    clearInvincibleBgm(3);
    smGoalKartCount = 0;
}

void KartSoundMgr::changeAttribute(u8) {} // UNUSED

void KartSoundMgr::changeDriver(bool) {}

void KartSoundMgr::checkCourseSound(u8) {} // UNUSED

void KartSoundMgr::frameWork(u8 p1) {
    if(_66 == 0) {
        _130->search(6, *mSoundPos);
        Course::Area* area = _130->getArea();
        if(area != NULL) {
            ECourseID courseID = Parameters::getRaceCourse();
            switch(courseID) {
            case 0x2a:
                if(p1 <= 3)
                {
                    JAISoundHandle& handle = (*this)[7];
                    if(!handle.isSoundAttached())
                    {
                        startSoundHandleNumber(7, 0x40024, 0);
                    }
                }
                break;

            case 0x29:
                _5c = 1;
                _6c = 0.5f;
                break;
            }
        }
        else {
            ECourseID courseID = Parameters::getRaceCourse();
            switch(courseID) {
            case 0x29:
                _5c = 0;
                break;
            }
        }
    }
    checkEcho();

    if(mKillSw || _66 == 2) {
        return;
    }

    crushRenzokuTaisaku();
    slipParamSet();
    if(_63 != 3) {
        _65 = p1;
    }
    checkAfterGoalVolume();
}

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
