#pragma once
#include <game/cLib/c_line.hpp>
#include <game/mLib/m_vec.hpp>

class dCenterSaveMng_c {
public:
    dCenterSaveMng_c();
    ~dCenterSaveMng_c();

    bool center_check(const mVec3_c* pos, u8 id, short* outValue);

    void center_entry(const mVec3_c* pos, u8 id, short value);

    static void allDoDelete();

private:
    /// @unofficial
    class Link_c : public cLineNd_c {
    public:
        Link_c() {
            mpSelf = this;
            ms_linkManager.addLastLineNode(this);
        }

        ~Link_c() {
            ms_linkManager.removeLineNode(this);
        }

        float getX() const { return mPos.x; }
        float getY() const { return mPos.y; }

        Link_c *mpSelf;
        mVec3_c mPos;
        short mRotation;
        u8 mID;
    };

public:
    static dCenterSaveMng_c *m_instance;
    static cLineMg_c ms_linkManager;
};
