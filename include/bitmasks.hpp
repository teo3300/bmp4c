#ifndef BITMASKS_HPP
#define BITMASKS_HPP

#define ALPHA_MASK  (1<<15)

#define BITMASK_5   0b11111
#define RED_SHIFT   10
#define BLUE_SHIFT  0
#define GREEN_SHIFT 5

#define MASK(COLOR)     (BITMASK_5 << COLOR##_SHIFT)
#define RED(COLOR)      ((COLOR & MASK(RED))>>RED_SHIFT)
#define GREEN(COLOR)    ((COLOR & MASK(GREEN))>>GREEN_SHIFT)
#define BLUE(COLOR)     ((COLOR & MASK(BLUE))>>BLUE_SHIFT)

// both colors are not possible due to clear bit[15] (when bit[15] is clear, color must be 0x7fff)
#define EMPTY           0x00

#endif