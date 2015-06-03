#include "xbrz.h"
#include "xbrz2x.hpp"
#include "xbrz.cpp"

// https://github.com/snes9xgit/snes9x/blob/1d140638da49554dac5bc5e39f44d438264a1451/win32/render.cpp#L2656
#ifdef _WIN32
    #define R5G6B5 // windows port uses RGB565
#endif

#ifdef R5G6B5
	#define	Mask_2	0x07E0	// 00000 111111 00000
	#define	Mask13	0xF81F	// 11111 000000 11111
	#define	Mask_1	0x001F	// 00000 000000 11111
	#define	Mask_3	0xF800	// 11111 000000 00000
	#define CONVERT_16_TO_32(pixel) \
        (((((pixel) >> 11)        ) << /*RedShift+3*/  19) | \
         ((((pixel) >> 5)   & 0x3f) << /*GreenShift+2*/10) | \
          (((pixel)         & 0x1f) << /*BlueShift+3*/ 3))
	#define NUMBITS (16)
    #define CONVERT_32_TO_16(pixel) \
        (((((pixel) & 0xf80000) >> 8) | \
          (((pixel) & 0xfc00)   >> 5) | \
          (((pixel) & 0xf8)     >> 3)) & 0xffff)
#else
	#define	Mask_2	0x03E0	// 00000 11111 00000
	#define	Mask13	0x7C1F	// 11111 00000 11111
	#define	Mask_1	0x001F	// 00000 00000 11111
	#define	Mask_3	0x7C00	// 11111 00000 00000
	#define CONVERT_16_TO_32(pixel) \
        (((((pixel) >> 10)        ) << /*RedShift+3*/  19) | \
         ((((pixel) >> 5)   & 0x1f) << /*GreenShift+3*/11) | \
          (((pixel)         & 0x1f) << /*BlueShift+3*/ 3))
	#define NUMBITS (15)
    #define CONVERT_32_TO_16(pixel) \
        (((((pixel) & 0xf80000) >> 9) | \
          (((pixel) & 0xf800)   >> 6) | \
          (((pixel) & 0xf8)     >> 3)) & 0xffff)
#endif

/*#################### XBRZ support ####################*/

//copy image and convert from RGB565/555 to ARGB
inline
void copyImage16To32(const uint16_t* src, int width, int height, int srcPitch,
                     uint32_t* trg, int yFirst, int yLast)
{
    yFirst = std::max(yFirst, 0);
    yLast  = std::min(yLast, height);
    if (yFirst >= yLast || height <= 0 || width <= 0) return;

    for (int y = yFirst; y < yLast; ++y)
    {
        uint32_t* trgLine = trg + y * width;
        const uint16_t* srcLine = reinterpret_cast<const uint16_t*>(reinterpret_cast<const char*>(src) + y * srcPitch);

        for (int x = 0; x < width; ++x)
            trgLine[x] = CONVERT_16_TO_32(srcLine[x]);
    }
}

//stretch image and convert from ARGB to RGB565/555
inline
void stretchImage32To16(const uint32_t* src, int srcWidth, int srcHeight,
                        uint16_t* trg, int trgWidth, int trgHeight, int trgPitch,
                        int yFirst, int yLast)
{
    yFirst = std::max(yFirst, 0);
    yLast  = std::min(yLast, trgHeight);
    if (yFirst >= yLast || srcHeight <= 0 || srcWidth <= 0) return;

    for (int y = yFirst; y < yLast; ++y)
    {
        uint16_t* trgLine = reinterpret_cast<uint16_t*>(reinterpret_cast<char*>(trg) + y * trgPitch);
        const int ySrc = srcHeight * y / trgHeight;
        const uint32_t* srcLine = src + ySrc * srcWidth;
        for (int x = 0; x < trgWidth; ++x)
        {
            const int xSrc = srcWidth * x / trgWidth;
            trgLine[x] = CONVERT_32_TO_16(srcLine[xSrc]);
        }
    }
}

// Implementation

void XbrzFilter::size(unsigned &outwidth, unsigned &outheight, unsigned width, unsigned height) {
  if(width > 256 || height > 240) return filter_direct.size(outwidth, outheight, width, height);

  outwidth  = 2 * width;
  outheight = 2 * height;
}

// Not thread-safe, but we don't need to alloc a large array everytime
static std::vector<uint32_t> src_32;

void XbrzFilter::render(uint32_t *output, unsigned outpitch, const uint16_t *input, unsigned pitch, unsigned width, unsigned height) {
  if(width > 256 || height > 240) {
    filter_direct.render(output, outpitch, input, pitch, width, height);
    return;
  }

  src_32.resize(width * height);
  ::copyImage16To32(input, width, height, pitch, src_32.data(), 0, height);
  xbrz::scale(2, src_32.data(), width, height, pitch / 2, output, outpitch, xbrz::RGB);
}
