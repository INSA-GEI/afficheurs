/*
 * GUI_Images.h
 *
 *  Created on: 13 déc. 2021
 *      Author: dimercur
 */

#ifndef GUI_IMAGES_H_
#define GUI_IMAGES_H_

enum {
	LV_IMG_CF_INDEXED_1BIT=0,
};

/** header of image */
typedef struct _image_header_struct {
	uint16_t w; /* width of image, in pixel */
	uint16_t h; /* height of image, in pixel */
	uint8_t always_zero; /* as said, always zero */
	uint8_t cf; /* color format: only LV_IMG_CF_INDEXED_1BIT supported */
} lv_img_header_t;

/** Describe the properties of an image */
typedef struct _image_struct {
	lv_img_header_t header; /* image's header */
	uint32_t data_size;  	/* image's size, in byte */
	const uint8_t *data;	/* Image's data */
} lv_img_dsc_t;

#endif /* GUI_IMAGES_H_ */
