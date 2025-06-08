#include <types.h>
#include <game/sLib/s_State.hpp>

namespace EGG {
    class CoreController {
    public:
        void startPatternRumble(const char *, int, bool);
    };
};

namespace mPad {
    enum CH_e {
        MPAD_CH_0,
        MPAD_CH_1,
        MPAD_CH_2,
        MPAD_CH_3
    };

    static EGG::CoreController *g_core[4];
};

class dRemoconMng_c {
public:
    class dConnect_c {
    public:
        class dExtension_c {
        public:
            virtual ~dExtension_c();

            mPad::CH_e mControllerID;
            u32 mExtensionType;
            sFStateMgr_c<dExtension_c, sStateMethodUsr_FI_c> mState;
        };

        virtual ~dConnect_c();

        mPad::CH_e mControllerID;
        dExtension_c mExtension;
        u32 m_50;
        bool mWillDisconnect;
        bool mIsRumbleEnabled;
        u8 mPad1[2];
        u32 m_58; ///< some counter
        sFStateMgr_c<dConnect_c, sStateMethodUsr_FI_c> mState;
    };

    dRemoconMng_c(mPad::CH_e);
    virtual ~dRemoconMng_c();
    void execute();
    void reset();

    dConnect_c *mRemocons[4];

    static dRemoconMng_c * m_instance;
};
