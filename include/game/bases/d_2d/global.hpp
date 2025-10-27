#pragma once
#include <types.h>
#include <game/mLib/m_2d.hpp>
#include <nw4r/ut.h>
#include <nw4r/lyt.h>

/// @brief 2D engine namespace.
/// Deals with drawing anything 2D, like the HUD and small scores.
namespace d2d {

void init(); ///< Initializes the 2D engine.

void draw(); ///< Draws all registered 2D objects.
void drawBefore(); ///< Draws all registered background 2D objects.
void drawAfter(); ///< Draws all registered foreground 2D objects.
void drawBtween(u8 after, u8 before); ///< Draws all registered 2D objects between the @p after and @p before draw orders.

/// @brief Visitor function for setting the opacity of panes.
/// @param pane Pointer to the pane being visited.
/// @param opacity Pointer to the opacity value to set.
int setAlpha_patrolPane_patrol(nw4r::lyt::Pane *pane, void *opacity);
void setAlpha(m2d::Simple_c *layout, u8 opacity); ///< Sets the opacity of an entire layout.

} // namespace d2d
