#include "convert.h"

void YUV422toRGB888(int width, int height, unsigned char *src, unsigned char *dst)
{
	int line, column;
	unsigned char *py, *pu, *pv;
	unsigned char *tmp = dst;

	py = src;
	pu = src + 1;
	pv = src + 3;
	#define CLIP(x) ((x) >= 0xFF ? 0xFF : ((x) <= 0x00 ? 0x00 : (x)))
	for (line = 0; line < height; ++line)
	{
		for (column = 0; column < width; ++column)
		{
			*tmp++ = CLIP((int)*py + ((int)*pu - 128) + ((103 * ((int)*pu - 128)) >> 8));
			*tmp++ = CLIP((int)*py - (88 * ((int)*pv - 128) >> 8) - ((183 * ((int)*pu - 128)) >> 8));
			*tmp++ = CLIP((int)*py + ((int)*pv - 128) + ((198 * ((int)*pv - 128)) >> 8));
			// increase py every time
			py += 2;
			// increase pu,pv every second time
			if ((column & 1) == 1)
			{
				pu += 4;
				pv += 4;
			}
		} //end for col
	}	  //end for line
}