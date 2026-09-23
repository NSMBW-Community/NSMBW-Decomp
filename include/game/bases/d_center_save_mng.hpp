#pragma once
#include <game/cLib/c_line.hpp>
#include <game/mLib/m_vec.hpp>

/// @brief A manager for saving and restoring rotation state of daObjCenter2_c actors.
/// @ingroup bases
class dCenterSaveMng_c {
private:
    /// @brief An entry in the rotation state manager.
    /// @unofficial
    class Entry_c : public cLineNd_c {
    public:
        Entry_c() {
            mpSelf = this;
            ms_linkManager.addLastLineNode(this);
        }

        ~Entry_c() {
            ms_linkManager.removeLineNode(this);
        }

        float getX() const { return mPos.x; }
        float getY() const { return mPos.y; }

        Entry_c *mpSelf; ///< A pointer to this entry. [Not sure why this is needed, always points to itself.]
        mVec3_c mPos; ///< The position of the related rotation controller.
        short mRotation; ///< The saved target rotation of the related rotation controller.
        u8 mRotationID; ///< The rotation ID of the related rotation controller.
    };

public:
    dCenterSaveMng_c(); ///< Constructs a new manager.
    ~dCenterSaveMng_c(); ///< Destroys the manager.

    /// @brief Tries to restore the rotation from a given position and ID.
    /// @param pos The position to check.
    /// @param id The ID to check.
    /// @param outRotation The output rotation value if a match is found.
    /// @return Whether a rotation was found and restored.
    bool center_check(const mVec3_c* pos, u8 id, short *outRotation);

    /// @brief Saves the rotation state for a given position and ID.
    /// @param pos The position to save.
    /// @param id The ID to save.
    /// @param rotation The rotation value to save.
    void center_entry(const mVec3_c* pos, u8 id, short rotation);

private:
    /// @brief Clear all saved rotation states.
    static void allDoDelete();

public:
    static dCenterSaveMng_c *m_instance; ///< The singleton instance of the manager.
    static cLineMg_c ms_linkManager; ///< The linked list manager for the saved rotation states.
};
