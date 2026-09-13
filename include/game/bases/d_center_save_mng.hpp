#pragma once
#include <game/cLib/c_line.hpp>
#include <game/mLib/m_vec.hpp>

/// @brief Manages a list of center save entries (position + parameter -> value).
/// @unofficial
/// @ingroup bases
class dCenterSaveMng_c {
public:
    dCenterSaveMng_c(); ///< Registers this instance as the singleton.
    ~dCenterSaveMng_c(); ///< Deletes all entries and clears the singleton instance.

    /// @brief Queries a center save entry by position and parameter.
    /// @param pos The position to check.
    /// @param param The parameter to check.
    /// @param outValue Pointer to store the found value (set to 0 if not found).
    /// @return 1 if found, 0 otherwise.
    int center_check(const mVec3_c* pos, unsigned char param, short* outValue);

    /// @brief Creates or updates a center save entry.
    /// @param pos The position to save.
    /// @param param The parameter to save.
    /// @param value The value to associate with this position + parameter.
    void center_entry(const mVec3_c* pos, unsigned char param, short value);

    /// @brief Deletes all entries from the list.
    static void allDoDelete();

    static dCenterSaveMng_c* getInstance() { return m_instance; }

private:
    struct Entry : public cLineNd_c {
        Entry() {
            mpData = this;
            ms_linkManager.addLastLineNode(this);
        }

        Entry *mpData;    ///< Always points to itself.
        mVec3_c mPos;     ///< Position (x, y, z).
        s16 mValue;       ///< Associated value.
        u8 mParam;        ///< Associated parameter.
    };

    static dCenterSaveMng_c *m_instance;
    static cLineMg_c ms_linkManager;
};
