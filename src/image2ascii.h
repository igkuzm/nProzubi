/**
 * File              : image2ascii.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 17.05.2023
 * Last Modified Date: 25.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef IMAGE_TO_ASCII_H
#define IMAGE_TO_ASCII_H

#include "prozubilib/log.h"
#include "prozubilib/alloc.h"

#define STB_IMAGE_IMPLEMENTATION
#include "/home/kuzmich/src/stb/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "/home/kuzmich/src/stb/stb_image_resize.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "/home/kuzmich/src/stb/stb_image_write.h"

/* 
 * image2ascii
 * return length of allocated ascii c string (with cols and rows) converted from 
 * image data (jpg, png, gif, bmp ...)
 * @image_data - pointer to image data
 * @len - size of image data
 * @cols - number of columns of ascii string (0 - to have original width of image)
 * @rows - number of rows of ascii string (0 to have original resolution)
 * @ascii - pointer to ascii string
 */
static size_t
image2ascii(
		unsigned char * image_data,
		size_t len,
		int cols,
		int rows,
		char **ascii,
		void *userdata,
		void (*on_error)(void *userdata, const char * error)
		)
{
	//const char map[11] = "@#%xo;:,. "; // map of image convert
	const char map[11] = " .,:;ox%#@"; // map of image convert
	stbi_uc *image, *buf;              // image pointer
	int w, h, c, x, y, index;
	size_t i = 0;
	
	/*read image */
	image = stbi_load_from_memory(
			image_data, len, &w, &h, &c, 0);
	if (!image){
		if (on_error)
			on_error(userdata, 
					STR("can't get image from data: %p with len %ld", image_data, len));
		return -1;
	}

	/* resize image */
	if (cols < 1)
		cols = w;
	if (rows < 1)
		rows = h * cols / w;

	buf = MALLOC(cols*rows*c, 
		if (on_error)
			on_error(userdata, 
					STR("can't allocate buffer with size: %d", 
					cols * rows * c)), 
		return -1);

	stbir_resize_uint8(image, w, h, 0, buf, cols, rows, 0, c);
	stbi_image_free(image);	

	/* set image to gray */
	int gc = c == 4 ? 2 : 1; // set channels for gray
	image = MALLOC(cols * rows * gc, 
			if (on_error)
				on_error(userdata, 
					STR("can't allocate buffer for gray image with size: %d",
					cols * rows * gc)), 
			return -1);
	stbi_uc *p, *pg;
	for (p  = buf, pg = image; 
			 p != buf + (cols * rows * c);
			 p += c, pg += gc)
	{
		*pg = (uint8_t)((*p + *(p + 1) + *(p + 2)) / 3.0);
		if (c == 4)
			*(pg + 1) = *(p + 3);
	}
	stbi_image_free(buf);	

	/* convert to ascii */
	buf = MALLOC((cols * rows * 2) + rows + 1,
			if (on_error)
				on_error(userdata, 
					STR("can't allocate ascii buffer with size: %d", 
					cols * rows + rows + 1)), 
			return -1);
	stbi_uc *ptr = image;
	for (y = 0; y < rows; y++)
	{
			for (x = 0; x < cols; x++)
			{
				index = (int)(*(ptr) / (255 / (sizeof(map) / sizeof(map[0]))));
				index > 9 ? index = 9 : 1;
				index < 0 ? index = 0 : 1;
				buf[i++] = map[index];
				buf[i++] = map[index];
				ptr++;
			}
			buf[i++] = '\n';
	}
	buf[i] = '\0';

	free(image);
	
	if (ascii)
		*ascii = (char *)buf;

	return i;
}

#endif /* ifndef IMAGE_TO_ASCII_H */
