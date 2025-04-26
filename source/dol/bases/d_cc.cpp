#include <game/bases/d_cc.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_bg.hpp>
#include <lib/MSL_C/math.h>
#include <lib/nw4r/math/trigonometry.hpp>
#include <game/cLib/c_math.hpp>

dCc_c *dCc_c::mEntryN;
dCc_c *dCc_c::mEntryB;

dCc_c::InitializedUnkClass dCc_c::msInitializedUnkClass = { 0, 0, 0, true, true };
char dCc_c::msIsInitialized;
dCc_c::InitializedUnkClass::_init dCc_c::InitializedUnkClass::_initializer;

// Note that for trapezoid checking, the second collider is always treated as a rectangular collider.
dCc_c::hitCheck dCc_c::_hitCheck[4][4] = {
    {&dCc_c::_hitCheckNormal,     &dCc_c::_hitCheckBoxCircle,  &dCc_c::_hitCheckDaikeiUD,   &dCc_c::_hitCheckDaikeiLR},
    {&dCc_c::_hitCheckBoxCircle,  &dCc_c::_hitCheckCircle,     &dCc_c::_hitCheckDaikeiUD,   &dCc_c::_hitCheckDaikeiLR},
    {&dCc_c::_hitCheckDaikeiUD_R, &dCc_c::_hitCheckDaikeiUD_R, &dCc_c::_hitCheckDaikeiUD_R, &dCc_c::_hitCheckDaikeiUD_R},
    {&dCc_c::_hitCheckDaikeiLR_R, &dCc_c::_hitCheckDaikeiLR_R, &dCc_c::_hitCheckDaikeiLR_R, &dCc_c::_hitCheckDaikeiLR_R}
};

dCc_c::dCc_c() {
    mpNext = nullptr;
    mpPrev = nullptr;
    mIsLinked = false;
    mFriendActor = nullptr;
    mNonCollideMask = true;
    mLayer = 0;
    clear();
}

dCc_c::~dCc_c() {
    release();
}

void dCc_c::clear() {
    mCollidedWith = 0;
    mAttSent = 0;
    mAttReceived = 0;
    mFlag &= ~CC_DISABLE; // Enable the collider
    for (unsigned int i = 0; i < ARRAY_SIZE(mCollOffsetX); i++) {
        mCollOffsetX[i] = 0.0f;
        mCollOffsetY[i] = 0.0f;
    }
}

void dCc_c::entry() {
    if (mIsLinked) {
        return;
    }

    mpNext = mEntryN;
    mEntryN = this;
    if (mpNext == nullptr) {
        mpPrev = mEntryB;
        mEntryB = this;
    } else {
        mpPrev = mpNext->mpPrev;
        mpNext->mpPrev = this;
    }

    mIsLinked = true;
}

void dCc_c::release() {
    if (!mIsLinked) {
        return;
    }

    if (mpNext != nullptr) {
        mpNext->mpPrev = mpPrev;
    } else {
        mEntryB = mpPrev;
    }
    if (mpPrev != nullptr) {
        mpPrev->mpNext = mpNext;
    } else {
        mEntryN = mpNext;
    }

    mpNext = nullptr;
    mpPrev = nullptr;
    mIsLinked = false;
}

void dCc_c::registerCc(dBaseActor_c *actor, CcData_s *collInfo) {
    mpOwner = actor;
    mCcData = *collInfo;
    mFlag = 0;
}

void dCc_c::registerCc(dBaseActor_c *actor, CcData_s *collInfo, u8 nonCollideMask) {
    registerCc(actor, collInfo);
    mNonCollideMask = nonCollideMask;
}

void dCc_c::reset() {
    // Clear collision checker list
    mEntryN = nullptr;
    mEntryB = nullptr;

    // Set the default hit check to the correct type
    const static hitCheck checks[3] = {
        &_hitCheckNormal,
        &_hitCheckLoop,
        &_hitCheckLoop
    };
    _hitCheck[0][0] = checks[dScStage_c::m_loopType];
}

u16 dCc_c::isHit(u16 mask) const {
    return mCollidedWith & mask;
}

u16 dCc_c::isHitAtDmg(u16 mask) const {
    return mAttReceived & mask;
}

float dCc_c::getTopPos() {
    return mCcData.mOffsetY + mpOwner->mPos.y + mCcData.mHeight;
}

float dCc_c::getUnderPos() {
    return mCcData.mOffsetY + mpOwner->mPos.y - mCcData.mHeight;
}

float dCc_c::getCenterPosY() {
    return mCcData.mOffsetY + mpOwner->mPos.y;
}

float dCc_c::getRightPos() {
    return mCcData.mOffsetX + mpOwner->mPos.x + mCcData.mWidth;
}

float dCc_c::getLeftPos() {
    return mCcData.mOffsetX + mpOwner->mPos.x - mCcData.mWidth;
}

float dCc_c::getCenterPosX() {
    return mCcData.mOffsetX + mpOwner->mPos.x;
}

bool dCc_c::isInside(dCc_c *other) {
    float _unused = (1.0f / 256.f); // [Needed to match order in .sdata2]
    if (mShape == CC_SHAPE_CIRCLE) {
        float dist = (getCenterVec() - other->getCenterVec()).getLength();
        // [Seems like someone just gave up on writing this part.
        // Every circle is always inside any other collider]
        return true;
    }
    float xDist = getCenterPosX() - other->getCenterPosX();
    if (EGG::Mathf::abs(xDist) > EGG::Mathf::abs(mCcData.mWidth - other->mCcData.mWidth)) {
        return false;
    }

    float yDist = getCenterPosY() - other->getCenterPosY();
    if (EGG::Mathf::abs(yDist) > EGG::Mathf::abs(mCcData.mHeight - other->mCcData.mHeight)) {
        return false;
    }
    return true;
}

bool dCc_c::checkCollision(dCc_c *c1, dCc_c *c2, int active) {
    if (c2 == c1) {
        // No self-collisions
        return false;
    }
    if (c2->mpOwner == c1->mpOwner) {
        // No collisions between two colliders that belong to the same actor
        return false;
    }
    if (c2->mpOwner == c1->mFriendActor || c1->mpOwner == c2->mFriendActor) {
        // No collisions between friend actors
        return false;
    }
    if ((c1->mFlag & CC_DISABLE) || (c2->mFlag & CC_DISABLE)) {
        // Disabled colliders don't collide
        return false;
    }
    if ((c1->mNonCollideMask & c2->mNonCollideMask) == 0) {
        // If any bit is set in both non-collide masks, the colliders don't collide
        return false;
    }
    if (c1->mLayer != c2->mLayer) {
        // Colliders only collide if they are on the same layer
        return false;
    }

    CcData_s collInfo1 = c1->mCcData;
    u32 catInteract1 = collInfo1.mCategoryInteract;
    u32 catMask1 = 1 << collInfo1.mCategory;

    CcData_s collInfo2 = c2->mCcData;
    u32 catInteract2 = collInfo2.mCategoryInteract;
    u32 catMask2 = 1 << collInfo2.mCategory;

    // First, check the category interaction.
    // A collision only occurs if the two colliders' categories
    // are set in the other's category interact masks.
    if ((catInteract1 & catMask2) && (catInteract2 & catMask1)) {

        u32 attInteract1 = collInfo1.mAttackCategoryInteract;
        u32 attCatMask1 = 1 << collInfo1.mAttackCategory;
        u32 attInteract2 = collInfo2.mAttackCategoryInteract;
        u32 attCatMask2 = 1 << collInfo2.mAttackCategory;

        if ((
            // Collider 1 can collide with an attacker and collider 2 is of type attacker,
            (catInteract1 & catMask2 & BIT_FLAG(CAT_PLAYER_ATTACK))
            // but the attack masks don't match
            && (attCatMask2 & attInteract1) == 0
        ) || ( // or
            // Collider 2 can collide with an attacker and collider 1 is of type attacker,
            (catInteract2 & catMask1 & BIT_FLAG(CAT_PLAYER_ATTACK))
            // but the attack masks don't match
            && (attCatMask1 & attInteract2) == 0
        )) {
            return false;
        }

        // Check for collision
        if (_hitCheck[c2->mShape][c1->mShape](c1, c2)) {
            if (!active) {
                return true;
            }

            // Set result flags
            if (!(collInfo2.mFlag & CC_DATA_PASSIVE)) {
                c1->mCollidedWith |= catMask2;
                if (attInteract1 & attCatMask2) {
                    c1->mAttReceived |= attInteract1 & attCatMask2;
                }
                if (attCatMask1 & attInteract2) {
                    c1->mAttSent |= attInteract2 & attCatMask1;
                }
            }
            if (!(collInfo1.mFlag & CC_DATA_PASSIVE)) {
                c2->mCollidedWith |= catMask1;
                if (attInteract1 & attCatMask2) {
                    c2->mAttSent |= attInteract1 & attCatMask2;
                }
                if (attCatMask1 & attInteract2) {
                    c2->mAttReceived |= attInteract2 & attCatMask1;
                }
            }

            // Execute callback if needed
            if (!(collInfo2.mFlag & CC_DATA_PASSIVE) && collInfo1.mCallback != nullptr) {
                collInfo1.mCallback(c1, c2);
            }
            if (!(collInfo1.mFlag & CC_DATA_PASSIVE) && collInfo2.mCallback != nullptr) {
                collInfo2.mCallback(c2, c1);
            }
        }
    }
    return c1->mCollidedWith != 0;
}

void dCc_c::execute() {
    for (dCc_c *c1 = mEntryN; c1 != nullptr; c1 = c1->mpNext) {
        for (dCc_c *c2 = c1->mpNext; c2 != nullptr; c2 = c2->mpNext) {
            checkCollision(c1, c2, 1);
        }
    }
}

bool dCc_c::_hitCheckSquare(dCc_c *c1, dCc_c *c2, mVec2_c pos1, mVec2_c pos2) {
    CcData_s &ci1 = c1->mCcData;
    CcData_s &ci2 = c2->mCcData;

    // Compute the distance between the two colliders and the maximum distances for a collision
    float xDist = pos1.x - pos2.x;
    float collSizeX = ci1.mWidth + ci2.mWidth;
    float yDist = pos1.y - pos2.y;
    float collSizeY = ci1.mHeight + ci2.mHeight;

    if (EGG::Mathf::abs(xDist) < collSizeX && EGG::Mathf::abs(yDist) < collSizeY) {
        c1->mCollPos = pos1;
        c2->mCollPos = pos2;

        // [Need to research what this check is for]
        if ((ci1.mFlag & CC_DATA_NO_OFFSET || ci2.mFlag & CC_DATA_NO_OFFSET)
        && ci1.mCategory != CAT_GOAL_POLE && ci2.mCategory != CAT_GOAL_POLE) {
            return true;
        }

        // The offset is half the amount of overlap since we distribute the
        // shifting between the two colliders

        float offsetX = (collSizeX - EGG::Mathf::abs(xDist)) / 2;
        if (xDist < 0.0f) {
            offsetX = -offsetX;
        }

        float offsetY = (collSizeY - EGG::Mathf::abs(yDist)) / 2;
        if (yDist < 0.0f) {
            offsetY = -offsetY;
        }

        c1->mCollOffsetX[ci2.mCategory] = offsetX;
        c1->mCollOffsetY[ci2.mCategory] = offsetY;

        c2->mCollOffsetX[ci1.mCategory] = -offsetX;
        c2->mCollOffsetY[ci1.mCategory] = -offsetY;
        return true;
    }
    return false;
}

bool dCc_c::_hitCheckNormal(dCc_c *c1, dCc_c *c2) {
    mVec2_c v1 = c1->getCenterVec();
    mVec2_c v2 = c2->getCenterVec();
    return _hitCheckSquare(c1, c2, v1, v2);
}

bool dCc_c::_hitCheckLoop(dCc_c *c1, dCc_c *c2) {
    // For a looping stage, we check for overlap twice:
    // - Once with the original positions (after accounting for the loop offset) and
    // - Once with one of the colliders shifted by the loop offset.
    // This second check is done for the case where
    // the colliders are on opposite sides of the loop.

    mVec2_c p1(dScStage_c::getLoopPosX(c1->getCenterPosX()), c1->getCenterPosY());
    mVec2_c p2(dScStage_c::getLoopPosX(c2->getCenterPosX()), c2->getCenterPosY());
    if (_hitCheckSquare(c1, c2, p1, p2)) {
        return true;
    }

    if (p1.x < p2.x) {
        p1.incX(dBg_c::m_bg_p->mLoopOffset);
    } else {
        p2.incX(dBg_c::m_bg_p->mLoopOffset);
    }
    if (_hitCheckSquare(c1, c2, p1, p2)) {
        return true;
    }
    return false;
}

bool dCc_c::_hitCheckCircle(dCc_c *c1, dCc_c *c2) {
    mVec2_c p1 = c1->getCenterVec();
    mVec2_c p2 = c2->getCenterVec();

    // [Not sure why we are looking at the height here...
    // It seems they maybe wanted the circles to also support ellipses?
    // Either way, the collision calculations treat it as a circle.]
    float collSizeX = c1->mCcData.mWidth + c2->mCcData.mWidth;
    float collSizeY = c1->mCcData.mHeight + c2->mCcData.mHeight;
    float collSizeRadius = (collSizeX + collSizeY) / 2;

    mVec2_c distVec = p2 - p1;
    if (distVec.getLength() <= collSizeRadius) {
        // Push the circles apart in the direction of the collision
        float dist = collSizeRadius - distVec.getLength();
        s16 ang = cM::atan2s(distVec.y, EGG::Mathf::abs(distVec.x));
        // [This calculation is incorrect. It should be dist / 2 * ...
        // so that the shifting is distributed between the two colliders]
        float offsetX = dist * nw4r::math::CosS(ang);
        float offsetY = -dist * nw4r::math::SinS(ang);

        c1->mCollOffsetX[c2->mCcData.mCategory] = offsetX;
        c1->mCollOffsetY[c2->mCcData.mCategory] = offsetY;
        c1->mCollPos.x = p1.x;
        c1->mCollPos.y = p1.y;

        c2->mCollOffsetX[c1->mCcData.mCategory] = -offsetX;
        c2->mCollOffsetY[c1->mCcData.mCategory] = -offsetY;
        c2->mCollPos.x = p2.x;
        c2->mCollPos.y = p2.y;

        return true;
    }

    return false;
}

enum dir_e {
    LEFT = 0,
    BELOW = 0,
    RIGHT = 1,
    ABOVE = 1,
};

#define DIR2SCALE(dir) (1.0f - dir * 2.0f)

bool dCc_c::_hitCheckBoxCircle(dCc_c *c1, dCc_c *c2) {
    dCc_c *boxCc, *circleCc;

    if (c1->mShape == CC_SHAPE_BOX) {
        boxCc = c1;
        circleCc = c2;
    } else {
        boxCc = c2;
        circleCc = c1;
    }

    mVec2_c circlePos = circleCc->getCenterVec();
    mVec2_c boxPos = boxCc->getCenterVec();

    float circleRadius = circleCc->mCcData.mWidth;
    dir_e boxSideX = (boxCc->getCenterPosX() < circleCc->getCenterPosX()) ? LEFT : RIGHT;
    dir_e boxSideY = (boxCc->getCenterPosY() < circleCc->getCenterPosY()) ? BELOW : ABOVE;

    float closerEdgeX[] = {
        boxCc->getCenterPosX() + boxCc->mCcData.mWidth, // Left edge, if box is on the left
        boxCc->getCenterPosX() - boxCc->mCcData.mWidth // Right edge, if box is on the right
    };
    float closerEdgeY[] = {
        boxCc->getCenterPosY() + boxCc->mCcData.mHeight, // Top edge, if box is below
        boxCc->getCenterPosY() - boxCc->mCcData.mHeight // Bottom edge, if box is above
    };

    if (closerEdgeY[ABOVE] < circlePos.y && circlePos.y < closerEdgeY[BELOW]) {
        // The circle midpoint is fully within the Y bounds of the box
        float colliderXDist = DIR2SCALE(boxSideX) * (circlePos.x - closerEdgeX[boxSideX]);
        if (colliderXDist < circleRadius) {
            // Move the circle and box apart just in the X direction

            float circleOffsetX = DIR2SCALE(boxSideX) * (circleRadius - colliderXDist) / 2;
            circleCc->mCollOffsetX[circleCc->mCcData.mCategory] = circleOffsetX;
            circleCc->mCollOffsetY[circleCc->mCcData.mCategory] = 0.0f;
            circleCc->mCollPos = circlePos;

            float boxOffsetX = -circleCc->mCollOffsetX[circleCc->mCcData.mCategory];
            boxCc->mCollOffsetX[circleCc->mCcData.mCategory] = boxOffsetX;
            boxCc->mCollOffsetY[circleCc->mCcData.mCategory] = 0.0f;
            boxCc->mCollPos = boxPos;

            return true;
        }
    }
    if (closerEdgeX[RIGHT] < circlePos.x && circlePos.x < closerEdgeX[LEFT]) {
        // The circle midpoint is fully within the X bounds of the box
        float smthA = DIR2SCALE(boxSideY) * (circlePos.y - closerEdgeY[boxSideY]);
        if (smthA < circleRadius) {
            // Move the circle and box apart just in the Y direction

            circleCc->mCollOffsetX[circleCc->mCcData.mCategory] = 0.0f;
            float circleOffsetY = DIR2SCALE(boxSideY) * (circleRadius - smthA) / 2;
            circleCc->mCollOffsetY[circleCc->mCcData.mCategory] = circleOffsetY;
            circleCc->mCollPos = circlePos;

            boxCc->mCollOffsetX[circleCc->mCcData.mCategory] = 0.0f;
            float boxOffsetY = -circleCc->mCollOffsetY[circleCc->mCcData.mCategory];
            boxCc->mCollOffsetY[circleCc->mCcData.mCategory] = boxOffsetY;
            boxCc->mCollPos = boxPos;

            return true;
        }
    }

    // The circle is overlapping with just a corner of the box
    // Calculate the distance between the center of the circle and the closest corner
    mVec2_c closerCorner(closerEdgeX[boxSideX], closerEdgeY[boxSideY]);
    mVec2_c distVec = circlePos - closerCorner;
    float distance = distVec.normalise();
    if (distance < circleRadius) {
        float offsetBy = (circleRadius - distance) / 2;
        // [This should have been done the same way as in _hitCheckCircle - this way, we shift by too much.]
        circleCc->mCollOffsetX[circleCc->mCcData.mCategory] = distVec.x * offsetBy;
        circleCc->mCollOffsetY[circleCc->mCcData.mCategory] = distVec.y * offsetBy;
        circleCc->mCollPos = circlePos;

        boxCc->mCollOffsetX[boxCc->mCcData.mCategory] = -circleCc->mCollOffsetX[circleCc->mCcData.mCategory];
        boxCc->mCollOffsetY[boxCc->mCcData.mCategory] = -circleCc->mCollOffsetY[circleCc->mCcData.mCategory];
        boxCc->mCollPos = boxPos;

        return true;
    }

    return false;
}

enum LINE_CHECK_RESULT_e {
    INV = -1,
    ZERO = 0,
    ONE = 1,
    TWO = 2,
    THREE = 3
};

inline float ratioX(mVec2_c diff, float v) {
    return v * diff.y / diff.x;
}

inline float ratioY(mVec2_c diff, float v) {
    return v * diff.x / diff.y;
}

int dCc_c::_lineCheckUD(mVec2_c p1, mVec2_c p2, float x, float y) {
    if (p1.x > x || p2.x < x) {
        return INV;
    }

    float scaled = ratioX(p2 - p1, x - p1.x);
    if (scaled + p1.y < y) {
        return TWO;
    }

    return THREE;
}

bool dCc_c::_hitCheckDaikeiUD_R(dCc_c *ccBox, dCc_c *ccTrp) {
    return _hitCheckDaikeiUD(ccTrp, ccBox);
}

bool dCc_c::_hitCheckDaikeiUD(dCc_c *ccTrp, dCc_c * ccBox) {
    mVec2_c trpCenter = ccTrp->getCenterVec();
    mVec2_c boxCenter = ccBox->getCenterVec();

    // The left and right sides of the trapezoid are parallel,
    // so the width of this shape is the same everywhere.

    float collSizeX = ccTrp->mCcData.mWidth + ccBox->mCcData.mWidth;

    if (EGG::Mathf::abs(trpCenter.x - boxCenter.x) >= collSizeX) {
        return false;
    }

    float boxLeft = ccBox->getLeftPos();
    float boxRight = ccBox->getRightPos();
    float boxTop = ccBox->getTopPos();
    float boxBottom = ccBox->getUnderPos();

    mVec2_c trpCorner1(ccTrp->getLeftPos(), ccTrp->getTrpOffset(0) + trpCenter.y);
    mVec2_c trpCorner2(ccTrp->getLeftPos(), ccTrp->getTrpOffset(1) + trpCenter.y);
    mVec2_c trpCorner3(ccTrp->getRightPos(), ccTrp->getTrpOffset(2) + trpCenter.y);
    mVec2_c trpCorner4(ccTrp->getRightPos(), ccTrp->getTrpOffset(3) + trpCenter.y);

    if (trpCorner1.y < boxBottom && trpCorner3.y < boxBottom) {
        return false;
    }
    if (trpCorner2.y > boxTop && trpCorner4.y > boxTop) {
        return false;
    }

    if (!(boxLeft <= trpCorner1.x) || !(trpCorner3.x <= boxRight)) {
        if (_lineCheckUD(trpCorner1, trpCorner3, boxLeft, boxBottom) != THREE
         && _lineCheckUD(trpCorner1, trpCorner3, boxRight, boxBottom) != THREE) {
            return false;
        }

        if (_lineCheckUD(trpCorner2, trpCorner4, boxLeft, boxTop) != TWO
         && _lineCheckUD(trpCorner2, trpCorner4, boxRight, boxTop) != TWO) {
            return false;
        }
    }

    // No shifting of the colliders is done here.

    ccTrp->mCollOffsetX[ccBox->mCcData.mCategory] = 0;
    ccTrp->mCollOffsetY[ccBox->mCcData.mCategory] = 0;
    ccTrp->mCollPos.set(ccTrp->getCenterPosX(), ccTrp->getCenterPosY());

    ccBox->mCollOffsetX[ccTrp->mCcData.mCategory] = 0;
    ccBox->mCollOffsetY[ccTrp->mCcData.mCategory] = 0;
    ccBox->mCollPos.set(ccBox->getCenterPosX(), ccBox->getCenterPosY());

    return true;
}

int dCc_c::_lineCheckLR(mVec2_c p1, mVec2_c p2, float x, float y) {
    if (p1.y < y || p2.y > y) {
        return INV;
    }

    float scaled = ratioY(p1 - p2, y - p2.y);
    if (scaled + p2.x < x) {
        return ZERO;
    }

    return ONE;
}

bool dCc_c::_hitCheckDaikeiLR_R(dCc_c *ccBox, dCc_c *ccTrp) {
    return _hitCheckDaikeiLR(ccTrp, ccBox);
}

bool dCc_c::_hitCheckDaikeiLR(dCc_c *ccTrp, dCc_c *ccBox) {
    mVec2_c p1 = mVec2_c(ccTrp->getCenterPosX(), ccTrp->getCenterPosY());
    mVec2_c p2 = mVec2_c(ccBox->getCenterPosX(), ccBox->getCenterPosY());

    // The top and bottom sides of the trapezoid are parallel,
    // so the height of this shape is the same everywhere.

    float heightSum = ccTrp->mCcData.mHeight + ccBox->mCcData.mHeight;

    if (EGG::Mathf::abs(p1.y - p2.y) >= heightSum) {
        return false;
    }

    float boxLeft = ccBox->getLeftPos();
    float boxRight = ccBox->getRightPos();
    float boxTop = ccBox->getTopPos();
    float boxBottom = ccBox->getUnderPos();

    mVec2_c trpCorner1(ccTrp->getTrpOffset(0) + p1.x, ccTrp->getTopPos());
    mVec2_c trpCorner2(ccTrp->getTrpOffset(1) + p1.x, ccTrp->getTopPos());
    mVec2_c trpCorner3(ccTrp->getTrpOffset(2) + p1.x, ccTrp->getUnderPos());
    mVec2_c trpCorner4(ccTrp->getTrpOffset(3) + p1.x, ccTrp->getUnderPos());

    if (trpCorner1.x > boxRight && trpCorner3.x > boxRight) {
        return false;
    }
    if (trpCorner2.x < boxLeft && trpCorner4.x < boxLeft) {
        return false;
    }

    if (!(boxBottom <= trpCorner3.y) || !(trpCorner1.y <= boxTop)) {
        if (_lineCheckLR(trpCorner1, trpCorner3, boxRight, boxTop) != ZERO
         && _lineCheckLR(trpCorner1, trpCorner3, boxRight, boxBottom) != ZERO) {
            return false;
        }

        if (_lineCheckLR(trpCorner2, trpCorner4, boxLeft, boxTop) != ONE
         && _lineCheckLR(trpCorner2, trpCorner4, boxLeft, boxBottom) != ONE) {
            return false;
        }
    }

    // No shifting of the colliders is done here.

    ccTrp->mCollOffsetX[ccBox->mCcData.mCategory] = 0;
    ccTrp->mCollOffsetY[ccBox->mCcData.mCategory] = 0;
    ccTrp->mCollPos.set(ccTrp->getCenterPosX(), ccTrp->getCenterPosY());

    ccBox->mCollOffsetX[ccTrp->mCcData.mCategory] = 0;
    ccBox->mCollOffsetY[ccTrp->mCcData.mCategory] = 0;
    ccBox->mCollPos.set(ccBox->getCenterPosX(), ccBox->getCenterPosY());

    return true;
}
