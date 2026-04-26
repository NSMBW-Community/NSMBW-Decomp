#pragma once

#include <nw4r/types_nw4r.h>
#include <lib/revolution/OS/OSLink.h>

namespace nw4r {
namespace db {

/// @brief Represents a loaded debug map file.
struct MapFile {
    u8 *mpBuf; ///< The raw map file buffer.
    OSModuleInfo *mpModuleInfo; ///< The module information this map file is associated with, or @p nullptr if the module is not relocatable.
    int mEntryNum; ///< The entry number of the map file on disc, or @p -1 if the map file is loaded directly from memory.
    MapFile *mpNext; ///< A pointer to the next registered map file.
};

MapFile *MapFile_RegistOnDvd(void *, const char *, const OSModuleInfo *);
void MapFile_Unregist(MapFile *);

} // namespace db
} // namespace nw4r
