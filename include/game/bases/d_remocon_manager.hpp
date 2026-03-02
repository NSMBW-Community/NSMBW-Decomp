#include <types.h>
#include <game/mLib/m_pad.hpp>
#include <game/sLib/s_State.hpp>
#include <lib/egg/core/eggController.h>

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
        u32 m_58;
        sFStateMgr_c<dConnect_c, sStateMethodUsr_FI_c> mState;

        static bool m_isBoot;
    };

    dRemoconMng_c(mPad::CH_e);
    virtual ~dRemoconMng_c();
    void execute();
    void reset();

    dConnect_c *mRemocons[4];

    static dRemoconMng_c *m_instance;
};
