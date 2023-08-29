#ifndef MYJPEG_H
#define MYJPEG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "jpeglib.h"
#include <setjmp.h>

/**
  @brief 压缩原始yuyv422图像数据为jpeg文件
  @param image_data   原始YUYV数据
  @param image_width  原始图像的宽
  @param image_height 原始图像的高
  @param filename     目标文件名(xxx.jpg)
  @param quality      图像质量，取值范围0-100
  @return void
  */

void YUYV_JPEG_FILE(JSAMPLE *image_data, int image_width, int image_height,
                    const char *filename, int quality);

/**
 *功能：YUYV422转JPEG到内存
 *参数：image_data   原始YUYV数据
 *参数：image_width  原始图像的宽
 *参数：image_height 原始图像的高
 *参数：outbuffer	 压缩后数据缓冲区地址保存在此处，必须初始化
 *参数：outsize		 压缩后图像大小保存在此处，必须初始化
 */

void YUYV_JPEG_MEMORY(JSAMPLE *image_data, int image_width, int image_height,
                      unsigned char **outbuffer, unsigned long *outsize);

// outbuffer[w*h*3]
/**
  @brief jpg转rgb
  @param inbuffer     jpg数据源首地址
  @param insize       jpeg数据大小
  @param outbuffer    rgb数据缓冲区首地址(需要用户分配空间)
  @return success return 1, error return 0
  */
int JPEG_RGB_MEMORY(const unsigned char *inbuffer, unsigned long insize, unsigned char *outbuffer);

#endif
