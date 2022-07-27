#!/usr/bin/env python3
# Ported from https://github.com/RootCubed/RELocator/blob/master/uncompLZ.c

from pathlib import Path

def uncompressLZ(infile: Path, outfile: Path) -> None:
    print('Opening', infile, end='...\n')
    with open(infile, 'rb') as f:

        # Get compressed size
        f.seek(0, 2)
        compressedSize = f.tell()
        f.seek(1, 0)
        print('Compressed size:', compressedSize)

        # Get uncompressed size
        uncompressedSize = int.from_bytes(f.read(3), 'little')
        print('Uncompressed size:', uncompressedSize)

        # Read compressed data
        src = f.read()

    # Set up loop
    currPos = writeTo = bytesWritten = 0
    dst = bytearray(uncompressedSize)

    # Decompress loop
    print('Uncompressing data...')
    while bytesWritten < uncompressedSize:
        flags = src[currPos]
        currPos += 1
        for i in range(8):

            # Regular copy
            if (flags & 0x80) == 0:
                dst[writeTo] = src[currPos]
                writeTo += 1
                currPos += 1
                bytesWritten += 1

            # Compressed!
            else:
                duplLength = src[currPos] >> 4
                if duplLength == 0:
                    duplLength = (src[currPos] << 4 | src[currPos + 1] >> 4) + 0x11
                    currPos += 1
                elif duplLength == 1:
                    duplLength = ((src[currPos] & 0xF) << 12 | src[currPos+1] << 4 | src[currPos+2] >> 4) + 0x111
                    currPos += 2
                else:
                    duplLength += 1

                offset = (src[currPos] & 0xF) << 8
                offset |= src[currPos+1]
                offset += 1
                currPos += 2
                pos = writeTo - offset
                dst[writeTo:writeTo+duplLength] = dst[pos:pos+duplLength]

                writeTo += duplLength
                bytesWritten += duplLength

            flags <<= 1

            if bytesWritten == uncompressedSize:
                break
            if currPos >= compressedSize:
                bytesWritten = uncompressedSize
                break

    # Write the data
    print('Writing data...')
    with open(outfile, 'wb') as f:
        f.write(dst)

    # Flaunt our success
    print('Done!')


if __name__ == '__main__':

    # Parse arguments separately so this file can be imported from other ones
    import argparse
    parser = argparse.ArgumentParser(description='Uncompresses an LZ-compressed file.')
    parser.add_argument('infile', type=Path, help='Path to compressed file.')
    parser.add_argument('outfile', type=Path, help='Path the uncompressed file is written to.')
    args = parser.parse_args()

    # Call actual function
    if args.infile.is_file():
        uncompressLZ(args.infile, args.outfile)
    else:
        print('File', args.infile, 'not found!')
