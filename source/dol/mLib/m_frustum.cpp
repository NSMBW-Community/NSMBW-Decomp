#include <game/mLib/m_frustum.hpp>
#include <math.h>

void mFrustum_c::set(f32 fovy, f32 aspect, f32 n, f32 f, const mMtx_c &mtx) {
    f32 tangent = tan(NW4R_MATH_FIDX_TO_RAD(NW4R_MATH_DEG_TO_FIDX(fovy * 0.5f)));
    f32 t = tangent * n;
    f32 r = t * aspect;
    set(t, -t, -r, r, n, f, mtx, false);
}

void mFrustum_c::set(f32 t, f32 b, f32 l, f32 r, f32 n, f32 f, const mMtx_c &mtx, bool orthographic) {
    nw4r::math::VEC3 eye;
    mMtx_c inv;
    nw4r::math::VEC3 pts[POINT_MAX];

    PSMTXInverse(mtx, inv);
    PSMTXCopy(mtx, mCamMtx);

    mNearZ = -n;
    mFarZ = -f;

    pts[POINT_NEAR_TL].x = l;
    pts[POINT_NEAR_TL].y = t;
    pts[POINT_NEAR_TL].z = -n;
    pts[POINT_NEAR_TR].x = r;
    pts[POINT_NEAR_TR].y = t;
    pts[POINT_NEAR_TR].z = -n;
    pts[POINT_NEAR_BR].x = r;
    pts[POINT_NEAR_BR].y = b;
    pts[POINT_NEAR_BR].z = -n;
    pts[POINT_NEAR_BL].x = l;
    pts[POINT_NEAR_BL].y = b;
    pts[POINT_NEAR_BL].z = -n;

    pts[POINT_FAR_TL].z = -f;
    pts[POINT_FAR_TR].z = -f;
    pts[POINT_FAR_BR].z = -f;
    pts[POINT_FAR_BL].z = -f;

    if (orthographic) {
        pts[POINT_FAR_TL].x = l;
        pts[POINT_FAR_TL].y = t;
        pts[POINT_FAR_TR].x = r;
        pts[POINT_FAR_TR].y = t;
        pts[POINT_FAR_BR].x = r;
        pts[POINT_FAR_BR].y = b;
        pts[POINT_FAR_BL].x = l;
        pts[POINT_FAR_BL].y = b;

        mPlaneL.Set(&pts[POINT_NEAR_BL], &pts[POINT_FAR_BL], &pts[POINT_NEAR_TL]);
        mPlaneR.Set(&pts[POINT_NEAR_TR], &pts[POINT_FAR_TR], &pts[POINT_NEAR_BR]);
        mPlaneT.Set(&pts[POINT_NEAR_TL], &pts[POINT_FAR_TL], &pts[POINT_NEAR_TR]);
        mPlaneB.Set(&pts[POINT_NEAR_BR], &pts[POINT_FAR_BR], &pts[POINT_NEAR_BL]);

        for (int i = 0; i < POINT_MAX; i++) {
            PSMTXMultVec(inv, pts[i], pts[i]);
        }

        mPlanes[PLANE_L].Set(&pts[POINT_NEAR_BL], &pts[POINT_FAR_BL], &pts[POINT_NEAR_TL]);
        mPlanes[PLANE_R].Set(&pts[POINT_NEAR_TR], &pts[POINT_FAR_TR], &pts[POINT_NEAR_BR]);
        mPlanes[PLANE_T].Set(&pts[POINT_NEAR_TL], &pts[POINT_FAR_TL], &pts[POINT_NEAR_TR]);
        mPlanes[PLANE_B].Set(&pts[POINT_NEAR_BR], &pts[POINT_FAR_BR], &pts[POINT_NEAR_BL]);
    } else {
        f32 s = f / n;

        eye.x = 0.0f;
        eye.y = 0.0f;
        eye.z = 0.0f;

        pts[POINT_FAR_TL].x = s * l;
        pts[POINT_FAR_TL].y = s * t;
        pts[POINT_FAR_TR].x = s * r;
        pts[POINT_FAR_TR].y = s * t;
        pts[POINT_FAR_BR].x = s * r;
        pts[POINT_FAR_BR].y = s * b;
        pts[POINT_FAR_BL].x = s * l;
        pts[POINT_FAR_BL].y = s * b;

        mNearZ = -n;
        mFarZ = -f;

        mPlaneL.Set(&eye, &pts[POINT_NEAR_BL], &pts[POINT_NEAR_TL]);
        mPlaneR.Set(&eye, &pts[POINT_NEAR_TR], &pts[POINT_NEAR_BR]);
        mPlaneT.Set(&eye, &pts[POINT_NEAR_TL], &pts[POINT_NEAR_TR]);
        mPlaneB.Set(&eye, &pts[POINT_NEAR_BR], &pts[POINT_NEAR_BL]);

        for (int i = 0; i < POINT_MAX; i++) {
            PSMTXMultVec(inv, pts[i], pts[i]);
        }
        PSMTXMultVec(inv, eye, eye);

        mPlanes[PLANE_L].Set(&eye, &pts[POINT_NEAR_BL], &pts[POINT_NEAR_TL]);
        mPlanes[PLANE_R].Set(&eye, &pts[POINT_NEAR_TR], &pts[POINT_NEAR_BR]);
        mPlanes[PLANE_T].Set(&eye, &pts[POINT_NEAR_TL], &pts[POINT_NEAR_TR]);
        mPlanes[PLANE_B].Set(&eye, &pts[POINT_NEAR_BR], &pts[POINT_NEAR_BL]);
    }

    mBox.Set(pts, POINT_MAX);
    mPlanes[PLANE_N].Set(&pts[POINT_NEAR_TL], &pts[POINT_NEAR_TR], &pts[POINT_NEAR_BR]);
    mPlanes[PLANE_F].Set(&pts[POINT_FAR_TL], &pts[POINT_FAR_BL], &pts[POINT_FAR_BR]);
}

bool mFrustum_c::intersectSphere(const mSphere_c *pSphere) const {
    nw4r::math::VEC3 pos;

    pos.z = mCamMtx._20 * pSphere->mCenter.x + mCamMtx._21 * pSphere->mCenter.y +
            mCamMtx._22 * pSphere->mCenter.z + mCamMtx._23;

    if (pos.z - pSphere->mRadius > mNearZ) {
        return false;
    }
    if (pos.z + pSphere->mRadius < mFarZ) {
        return false;
    }

    pos.x = mCamMtx._00 * pSphere->mCenter.x + mCamMtx._01 * pSphere->mCenter.y +
            mCamMtx._02 * pSphere->mCenter.z + mCamMtx._03;

    if (pos.x * mPlaneL.n.x + pos.z * mPlaneL.n.z > pSphere->mRadius) {
        return false;
    }
    if (pos.x * mPlaneR.n.x + pos.z * mPlaneR.n.z > pSphere->mRadius) {
        return false;
    }

    pos.y = mCamMtx._10 * pSphere->mCenter.x + mCamMtx._11 * pSphere->mCenter.y +
            mCamMtx._12 * pSphere->mCenter.z + mCamMtx._13;

    if (pos.y * mPlaneT.n.y + pos.z * mPlaneT.n.z > pSphere->mRadius) {
        return false;
    }
    return !(pos.y * mPlaneB.n.y + pos.z * mPlaneB.n.z > pSphere->mRadius);
}

bool mFrustum_c::intersectAABB(const mAABB_c *pBox) const {
    if (!nw4r::math::IntersectionAABB(pBox, &mBox)) {
        return false;
    }

    nw4r::math::VEC3 v;
    for (int i = 0; i < PLANE_MAX; i++) {
        v.x = mPlanes[i].n.x >= 0.0f ? pBox->min.x : pBox->max.x;
        v.y = mPlanes[i].n.y >= 0.0f ? pBox->min.y : pBox->max.y;
        v.z = mPlanes[i].n.z >= 0.0f ? pBox->min.z : pBox->max.z;

        if (mPlanes[i].Test(v) > 0.0f) {
            return false;
        }
    }
    return true;
}
