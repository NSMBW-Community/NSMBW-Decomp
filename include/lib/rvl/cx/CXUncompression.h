#pragma once
/// @file

#ifdef __cplusplus
extern "C" {
#endif

/// @brief The possible compression types.
typedef enum {
    CX_COMPRESSION_NONE         = 0x00, ///< No compression.
    CX_COMPRESSION_LZ           = 0x10, ///< LZ77 compression.
    CX_COMPRESSION_HUFFMAN      = 0x20, ///< @unused Huffman encoding.
    CX_COMPRESSION_RL           = 0x30, ///< @unused Run length encoding.
    CX_COMPRESSION_LH           = 0x40, ///< @unused LZ77 + Huffman encoding.
    CX_COMPRESSION_LRC          = 0x50, ///< @unused LZ77 + Range encoding.
    CX_COMPRESSION_DIFF         = 0x80, ///< @unused Differential filter.
    CX_COMPRESSION_TYPE_MASK    = 0xF0,
} CXCompressionType;

void CXUncompressLZ(const void *orig, void *dest);
int CXGetUncompressedSize(void *data);

#ifdef __cplusplus
}
#endif
