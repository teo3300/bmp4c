#ifndef ERRORS_HPP
#define ERRORS_HPP

#include "cyntax.hpp"
#define SET(n) ((1<<(n)))

// error opening file
#define OPEN_IMAGE_ERROR            SET(0)
// no data
#define NO_DATA_ERROR               SET(1)
// magic bytes not matching
#define MAGIC_BYTES_ERROR           SET(2)
// error reading bitmapfileheader
#define BMFH_ERROR                  SET(3)
// bitdepth is not 16bpp
#define BITDEPTH_ERROR              SET(4)
// bitmap compression not supported
#define COMPRESSION_ERROR           SET(5)
// error parsing bitmapinfoheader
#define BMIH_ERROR                  SET(6)
// bad canvas alloc
#define CANVAS_ALLOC_ERROR          SET(7)
// bad hashtab alloc (colors)
#define HASHTAB_ALLOC_ERROR         SET(8)
// incompatible img size with data file size
#define SIZE_ERROR                  SET(9)
// error while parsing image data
#define PARSING_DATA_ERROR          SET(10)
// no available space in palette
#define PALETTE_ERROR               SET(11)
// splitsize is illegal (w==0, h!=0)
#define ILLEGAL_SPLIT_REQ_ERROR     SET(12)
// impossible to split image with the requested size preserving 8*8 chunks
#define SPLIT_ALIGN_ERROR           SET(13)
#endif