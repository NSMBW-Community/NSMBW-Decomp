#pragma once
#include <types.h>
#include <lib/rvl/mtx/vec.h>
/// @file
/// @details [cM3d stands for C Math 3D. It is unknown why a namespace wasn't used].

/**
 * @brief Performs a vector multiply-add.
 * @details The function can be summed up as: @p out = @p b * @p scale + @p a
 */
void cM3d_calcInDivPos1(const Vec *pA, const Vec *pB, float scale, Vec *pDest);

/**
 * @brief Calculates a vector distance and performs a vector multiply-add with it.
 * @details The function can be summed up as: @p out = ( @p b - @p a ) * @p scale + @p a
 */
void cM3d_calcInDivPos2(const Vec *pA, const Vec *pB, float scale, Vec *pDest);
