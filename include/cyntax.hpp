#ifndef CYNTAX_HPP
#define CYNTAX_HPP

#define INLINE static inline

#define HASHTAB_SCALE_FACTOR 1
#define MAX_PALETTE_SIZE 256
#define MAX_HASHTAB_SIZE (MAX_PALETTE_SIZE<<HASHTAB_SCALE_FACTOR)

#define FULL_PALETTE    MAX_PALETTE_SIZE
#define SMALL_PALETTE   16

#define LINE_SHIFT 3
#define LINE_SIZE  (1<<LINE_SHIFT)
#define CHUNK_SHIFT (LINE_SHIFT<<1)
#define CHUNK_SIZE (1<<CHUNK_SHIFT)

#define u8  unsigned char
#define u16 unsigned short  int
#define u32 unsigned        int
#define u64 unsigned long   int
#define sint signed int
#define uint unsigned int

#endif