#pragma once

class daFireBall_Player_c {
public:
    enum CreateKind_e {
        CREATE_ALIVE,
        CREATE_SPIN
    };

    static bool isFireBallEnable(int playerNo, CreateKind_e kind);
};
