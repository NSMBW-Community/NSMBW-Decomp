#pragma once
#include <game/bases/d_cc.hpp>

class dEn_c;

class FumiCcInfo_c {
public:
    FumiCcInfo_c(dCc_c *cc1, dCc_c *cc2) : mCc1(cc1), mCc2(cc2) {}
    virtual ~FumiCcInfo_c() {}

    dCc_c *mCc1;
    dCc_c *mCc2;
};

/// @unofficial
class FumiCheckBase_c {
public:
    FumiCheckBase_c() {}
    virtual ~FumiCheckBase_c() {}
    virtual void operate(int &, dEn_c *, FumiCcInfo_c &);
};

class NonUniqueFumiCheck_c : public FumiCheckBase_c {
public:
    NonUniqueFumiCheck_c() {}
    virtual ~NonUniqueFumiCheck_c() {}
    virtual void operate(int &, dEn_c *, FumiCcInfo_c &);
};

class dEnFumiCheck_c {
public:
    dEnFumiCheck_c(dEn_c *owner) {
        mUnused = 0;
        mFumiCheck = nullptr;
        mOwner = owner;
    }

    virtual ~dEnFumiCheck_c() {
        delete mFumiCheck;
    }

    /// @unofficial
    void refresh(FumiCheckBase_c *newPtr) {
        delete mFumiCheck;
        mFumiCheck = newPtr;
    }

    int mUnused;
    FumiCheckBase_c *mFumiCheck;
    dEn_c *mOwner;
};

class dEnFumiProc_c {
public:
    dEnFumiProc_c(dEn_c *owner) : mFumiCheck(owner) {}

    /// @unofficial
    void refresh(FumiCheckBase_c *newPtr) {
        mFumiCheck.refresh(newPtr);
    }

    int operate(FumiCcInfo_c &, int);

    dEnFumiCheck_c mFumiCheck;
};
