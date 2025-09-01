#ifndef RVL_SDK_AXFX_REVERB_HI_EXP_H
#define RVL_SDK_AXFX_REVERB_HI_EXP_H
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct AXFX_BUS AXFX_BUS;
typedef struct AXFX_BUFFERUPDATE AXFX_BUFFERUPDATE;

typedef struct AXFX_REVERBHI_EXP {
    f32* earlyLine[3];  // at 0x0
    u32 earlyPos[3];    // at 0xC
    u32 earlyLength;    // at 0x18
    u32 earlyMaxLength; // at 0x1C
    f32 earlyCoef[3];   // at 0x20

    f32* preDelayLine[3];  // at 0x2C
    u32 preDelayPos;       // at 0x38
    u32 preDelayLength;    // at 0x3C
    u32 preDelayMaxLength; // at 0x40

    f32* combLine[3][3];  // at 0x44
    u32 combPos[3];       // at 0x68
    u32 combLength[3];    // at 0x74
    u32 combMaxLength[3]; // at 0x80
    f32 combCoef[3];      // at 0x8C

    f32* allpassLine[3][2];  // at 0x98
    u32 allpassPos[2];       // at 0xB0
    u32 allpassLength[2];    // at 0xB8
    u32 allpassMaxLength[2]; // at 0xC0

    f32* lastAllpassLine[3];     // at 0xC8
    u32 lastAllpassPos[3];       // at 0xD4
    u32 lastAllpassLength[3];    // at 0xE0
    u32 lastAllpassMaxLength[3]; // at 0xEC

    f32 allpassCoef;     // at 0xF8
    f32 lastLpfOut[3];   // at 0xFC
    f32 lpfCoef;         // at 0x108
    u32 active;          // at 0x10C
    u32 earlyMode;       // at 0x110
    f32 preDelayTimeMax; // at 0x114
    f32 preDelayTime;    // at 0x118
    u32 fusedMode;       // at 0x11C
    f32 fusedTime;       // at 0x120
    f32 coloration;      // at 0x124
    f32 damping;         // at 0x128
    f32 crosstalk;       // at 0x12C
    f32 earlyGain;       // at 0x130
    f32 fusedGain;       // at 0x134
    AXFX_BUS* busIn;     // at 0x138
    AXFX_BUS* busOut;    // at 0x13C
    f32 outGain;         // at 0x140
    f32 sendGain;        // at 0x144
} AXFX_REVERBHI_EXP;

u32 AXFXReverbHiExpGetMemSize(const AXFX_REVERBHI_EXP* fx);
BOOL AXFXReverbHiExpInit(AXFX_REVERBHI_EXP* fx);
void AXFXReverbHiExpShutdown(AXFX_REVERBHI_EXP* fx);
BOOL AXFXReverbHiExpSettings(AXFX_REVERBHI_EXP* fx);
void AXFXReverbHiExpCallback(AXFX_BUFFERUPDATE* update, AXFX_REVERBHI_EXP* fx);

#ifdef __cplusplus
}
#endif
#endif
