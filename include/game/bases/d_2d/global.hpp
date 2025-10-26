#pragma once
#include <types.h>
#include <game/mLib/m_2d.hpp>
#include <nw4r/ut.h>
#include <nw4r/lyt.h>

namespace d2d {

void init();
void draw();
void drawBefore();
void drawAfter();
void drawBtween(u8, u8);
int setAlpha_patrolPane_patrol(nw4r::lyt::Pane *, void *);
void setAlpha(m2d::Simple_c *, u8);

} // namespace d2d
