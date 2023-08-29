
#include "myjpeg.h"
#include <string.h>

void YUYV_JPEG_FILE(JSAMPLE *image_data, int image_width, int image_height, const char *filename, int quality)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	/* 1.初始化 */
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	/*2. 数据保存到哪里去 (比如：文件) */
	FILE *outfile;
	if ((outfile = fopen(filename, "wb")) == NULL)
	{
		fprintf(stderr, "can't open %s\n", filename);
		exit(1);
	}
	jpeg_stdio_dest(&cinfo, outfile);

	/*3. 设置压缩参数 */
	cinfo.image_width = image_width;
	cinfo.image_height = image_height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_YCbCr;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);
	cinfo.raw_data_in = true;
	/*一次压缩16行，Y、U、V三个平面分开存放*/
	JSAMPLE y_plane[16][cinfo.image_width];
	JSAMPLE u_plane[8][cinfo.image_width / 2];
	JSAMPLE v_plane[8][cinfo.image_width / 2];
	/*指向每一行的指针*/
	JSAMPROW y_rows[16];
	JSAMPROW u_rows[8];
	JSAMPROW v_rows[8];
	/*指针初始化*/
	for (int i = 0; i < 16; ++i)
	{
		y_rows[i] = &y_plane[i][0];
	}
	for (int i = 0; i < 8; ++i)
	{
		u_rows[i] = &u_plane[i][0];
	}
	for (int i = 0; i < 8; ++i)
	{
		v_rows[i] = &v_plane[i][0];
	}
	/*最终压缩这些行(16行)*/
	JSAMPARRAY rows[] = {y_rows, u_rows, v_rows};
	/* 4.开始压缩 */
	jpeg_start_compress(&cinfo, true);

	while (cinfo.next_scanline < cinfo.image_height)
	{
		for (JDIMENSION i = 0; i < 16; ++i)
		{
			int offset = (cinfo.next_scanline + i) * cinfo.image_width * 2;
			for (JDIMENSION j = 0; j < cinfo.image_width; j += 2) //把每一行的YUV分量提取出来，保存到对应的分量数组
			{
				y_plane[i][j] = image_data[offset + j * 2 + 0];
				y_plane[i][j + 1] = image_data[offset + j * 2 + 2];

				if (i % 2 == 0)
				{
					u_plane[i / 2][j / 2] = image_data[offset + j * 2 + 1];
					v_plane[i / 2][j / 2] = image_data[offset + j * 2 + 3];
				}
			}
		}

		jpeg_write_raw_data(&cinfo, rows, 16);
	}

	/* 结束 */
	jpeg_finish_compress(&cinfo);
	fclose(outfile);
	jpeg_destroy_compress(&cinfo);

	/* 完成压缩 */
}

void YUYV_JPEG_MEMORY(JSAMPLE *image_data, int image_width, int image_height,
					  unsigned char **outbuffer, unsigned long *outsize)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	/* 1.初始化 */
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	/*2. 数据保存到哪里去 (比如：内存) */
	jpeg_mem_dest(&cinfo, outbuffer, outsize);

	/*3. 设置压缩参数 */
	cinfo.image_width = image_width;
	cinfo.image_height = image_height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_YCbCr;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 90, TRUE);
	cinfo.raw_data_in = TRUE;
	/*一次压缩16行，Y、U、V三个平面分开存放*/
	JSAMPLE y_plane[16][cinfo.image_width];
	JSAMPLE u_plane[8][cinfo.image_width / 2];
	JSAMPLE v_plane[8][cinfo.image_width / 2];
	/*指向每一行的指针*/
	JSAMPROW y_rows[16];
	JSAMPROW u_rows[8];
	JSAMPROW v_rows[8];
	/*指针初始化*/
	for (int i = 0; i < 16; ++i)
	{
		y_rows[i] = &y_plane[i][0];
	}
	for (int i = 0; i < 8; ++i)
	{
		u_rows[i] = &u_plane[i][0];
	}
	for (int i = 0; i < 8; ++i)
	{
		v_rows[i] = &v_plane[i][0];
	}
	/*最终压缩这些行(16行)*/
	JSAMPARRAY rows[] = {y_rows, u_rows, v_rows};

	/* 4.开始压缩 */
	jpeg_start_compress(&cinfo, TRUE);

	while (cinfo.next_scanline < cinfo.image_height)
	{
		for (int i = 0; i < 16; ++i)
		{
			int offset = (cinfo.next_scanline + i) * cinfo.image_width * 2;
			for (int j = 0; j < cinfo.image_width; j += 2) //把每一行的YUV分量提取出来，保存到对应的分量数组
			{
				y_plane[i][j] = image_data[offset + j * 2 + 0];
				y_plane[i][j + 1] = image_data[offset + j * 2 + 2];

				if (i % 2 == 0)
				{
					u_plane[i / 2][j / 2] = image_data[offset + j * 2 + 1];
					v_plane[i / 2][j / 2] = image_data[offset + j * 2 + 3];
				}
			}
		}

		jpeg_write_raw_data(&cinfo, rows, 16);
	}
	/* 结束 */
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	/* 完成压缩 */
}

// EXTERN(void) jpeg_mem_src (j_decompress_ptr cinfo, const unsigned char *inbuffer, unsigned long insize);
int JPEG_RGB_MEMORY(const unsigned char *inbuffer, unsigned long insize, unsigned char *outbuffer)
{
	struct jpeg_decompress_struct cinfo;
	// struct my_error_mgr jerr;
	struct jpeg_error_mgr jerr;

	/* More stuff */

	JSAMPARRAY buffer; /* Output row buffer(2D) */
	int row_stride;	   /* physical row width in output buffer */

	cinfo.err = jpeg_std_error(&jerr);

	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&cinfo);

	//设置数据来源及大小
	jpeg_mem_src(&cinfo, inbuffer, insize); //重点

	(void)jpeg_read_header(&cinfo, TRUE);

	(void)jpeg_start_decompress(&cinfo);

	/* JSAMPLEs per row in output buffer */
	row_stride = cinfo.output_width * cinfo.output_components;
	/* Make a one-row-high sample array that will go away when done with image */
	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

	//行号从1开始算, 不是从0开始的！！！
	while (cinfo.output_scanline < cinfo.output_height)
	{
		/* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
		(void)jpeg_read_scanlines(&cinfo, buffer, 1);
		/* Assume put_scanline_someplace wants a pointer and sample count. */
		// put_scanline_someplace(buffer[0], row_stride);
		memcpy(outbuffer + ((cinfo.output_scanline - 1) * row_stride), buffer[0], row_stride);
	}

	//-----------------------------

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	return 1;
}
