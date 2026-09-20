#include <game/mLib/m_frustum.hpp>
#include <math.h>

void mFrustum_c::set(f32 fovy, f32 aspect, f32 near, f32 far, const mMtx_c &cameraMtx) {
    f32 tangent = tan(NW4R_MATH_FIDX_TO_RAD(NW4R_MATH_DEG_TO_FIDX(fovy * 0.5f)));
    f32 top = tangent * near;
    f32 right = top * aspect;
    set(top, -top, -right, right, near, far, cameraMtx, false);
}

void mFrustum_c::set(f32 top, f32 bottom, f32 left, f32 right, f32 near, f32 far, const mMtx_c &cameraMtx, bool orthographic) {
    mVec3_c eyePos;
    mMtx_c invCameraMtx;
    mVec3_c pts[POINT_MAX];

    PSMTXInverse(cameraMtx, invCameraMtx);
    PSMTXCopy(cameraMtx, mCamMtx);

    mNearZ = -near;
    mFarZ = -far;

    pts[POINT_NEAR_TL].set(left, top, -near);
    pts[POINT_NEAR_TR].set(right, top, -near);
    pts[POINT_NEAR_BR].set(right, bottom, -near);
    pts[POINT_NEAR_BL].set(left, bottom, -near);

    pts[POINT_FAR_TL].z = -far;
    pts[POINT_FAR_TR].z = -far;
    pts[POINT_FAR_BR].z = -far;
    pts[POINT_FAR_BL].z = -far;

    if (orthographic) {
        pts[POINT_FAR_TL].x = left;
        pts[POINT_FAR_TL].y = top;
        pts[POINT_FAR_TR].x = right;
        pts[POINT_FAR_TR].y = top;
        pts[POINT_FAR_BR].x = right;
        pts[POINT_FAR_BR].y = bottom;
        pts[POINT_FAR_BL].x = left;
        pts[POINT_FAR_BL].y = bottom;

        mPlaneL.Set(&pts[POINT_NEAR_BL], &pts[POINT_FAR_BL], &pts[POINT_NEAR_TL]);
        mPlaneR.Set(&pts[POINT_NEAR_TR], &pts[POINT_FAR_TR], &pts[POINT_NEAR_BR]);
        mPlaneT.Set(&pts[POINT_NEAR_TL], &pts[POINT_FAR_TL], &pts[POINT_NEAR_TR]);
        mPlaneB.Set(&pts[POINT_NEAR_BR], &pts[POINT_FAR_BR], &pts[POINT_NEAR_BL]);

        for (int i = 0; i < POINT_MAX; i++) {
            PSMTXMultVec(invCameraMtx, pts[i], pts[i]);
        }

        mPlanes[PLANE_L].Set(&pts[POINT_NEAR_BL], &pts[POINT_FAR_BL], &pts[POINT_NEAR_TL]);
        mPlanes[PLANE_R].Set(&pts[POINT_NEAR_TR], &pts[POINT_FAR_TR], &pts[POINT_NEAR_BR]);
        mPlanes[PLANE_T].Set(&pts[POINT_NEAR_TL], &pts[POINT_FAR_TL], &pts[POINT_NEAR_TR]);
        mPlanes[PLANE_B].Set(&pts[POINT_NEAR_BR], &pts[POINT_FAR_BR], &pts[POINT_NEAR_BL]);
    } else {
        f32 depthRatio = far / near;

        eyePos.set(0.0f, 0.0f, 0.0f);

        pts[POINT_FAR_TL].x = depthRatio * left;
        pts[POINT_FAR_TL].y = depthRatio * top;
        pts[POINT_FAR_TR].x = depthRatio * right;
        pts[POINT_FAR_TR].y = depthRatio * top;
        pts[POINT_FAR_BR].x = depthRatio * right;
        pts[POINT_FAR_BR].y = depthRatio * bottom;
        pts[POINT_FAR_BL].x = depthRatio * left;
        pts[POINT_FAR_BL].y = depthRatio * bottom;

        mNearZ = -near;
        mFarZ = -far;

        mPlaneL.Set(&eyePos, &pts[POINT_NEAR_BL], &pts[POINT_NEAR_TL]);
        mPlaneR.Set(&eyePos, &pts[POINT_NEAR_TR], &pts[POINT_NEAR_BR]);
        mPlaneT.Set(&eyePos, &pts[POINT_NEAR_TL], &pts[POINT_NEAR_TR]);
        mPlaneB.Set(&eyePos, &pts[POINT_NEAR_BR], &pts[POINT_NEAR_BL]);

        for (int i = 0; i < POINT_MAX; i++) {
            PSMTXMultVec(invCameraMtx, pts[i], pts[i]);
        }
        PSMTXMultVec(invCameraMtx, eyePos, eyePos);

        mPlanes[PLANE_L].Set(&eyePos, &pts[POINT_NEAR_BL], &pts[POINT_NEAR_TL]);
        mPlanes[PLANE_R].Set(&eyePos, &pts[POINT_NEAR_TR], &pts[POINT_NEAR_BR]);
        mPlanes[PLANE_T].Set(&eyePos, &pts[POINT_NEAR_TL], &pts[POINT_NEAR_TR]);
        mPlanes[PLANE_B].Set(&eyePos, &pts[POINT_NEAR_BR], &pts[POINT_NEAR_BL]);
    }

    mBox.Set(pts, POINT_MAX);
    mPlanes[PLANE_N].Set(&pts[POINT_NEAR_TL], &pts[POINT_NEAR_TR], &pts[POINT_NEAR_BR]);
    mPlanes[PLANE_F].Set(&pts[POINT_FAR_TL], &pts[POINT_FAR_BL], &pts[POINT_FAR_BR]);
}

bool mFrustum_c::intersectSphere(const mSphere_c *sphere) const {
    mVec3_c sphereCenterPos;

    sphereCenterPos.z = mCamMtx._20 * sphere->mCenter.x +
                        mCamMtx._21 * sphere->mCenter.y +
                        mCamMtx._22 * sphere->mCenter.z + mCamMtx._23;

    if (sphereCenterPos.z - sphere->mRadius > mNearZ) {
        return false;
    }
    if (sphereCenterPos.z + sphere->mRadius < mFarZ) {
        return false;
    }

    sphereCenterPos.x = mCamMtx._00 * sphere->mCenter.x +
                        mCamMtx._01 * sphere->mCenter.y +
                        mCamMtx._02 * sphere->mCenter.z + mCamMtx._03;

    if (sphereCenterPos.x * mPlaneL.n.x + sphereCenterPos.z * mPlaneL.n.z > sphere->mRadius) {
        return false;
    }
    if (sphereCenterPos.x * mPlaneR.n.x + sphereCenterPos.z * mPlaneR.n.z > sphere->mRadius) {
        return false;
    }

    sphereCenterPos.y = mCamMtx._10 * sphere->mCenter.x +
                        mCamMtx._11 * sphere->mCenter.y +
                        mCamMtx._12 * sphere->mCenter.z + mCamMtx._13;

    if (sphereCenterPos.y * mPlaneT.n.y + sphereCenterPos.z * mPlaneT.n.z > sphere->mRadius) {
        return false;
    }
    if (sphereCenterPos.y * mPlaneB.n.y + sphereCenterPos.z * mPlaneB.n.z > sphere->mRadius) {
        return false;
    }

    return true;
}

bool mFrustum_c::intersectAABB(const mAABB_c *box) const {
    if (!nw4r::math::IntersectionAABB(box, &mBox)) {
        return false;
    }

    for (int i = 0; i < PLANE_MAX; i++) {
        mVec3_c point;
        point.x = mPlanes[i].n.x >= 0.0f ? box->min.x : box->max.x;
        point.y = mPlanes[i].n.y >= 0.0f ? box->min.y : box->max.y;
        point.z = mPlanes[i].n.z >= 0.0f ? box->min.z : box->max.z;

        if (mPlanes[i].Test(point) > 0.0f) {
            return false;
        }
    }
    return true;
}
