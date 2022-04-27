#include "GUI_Paint.h"

const  uint8_t error_96_map[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0xfc, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0xf8, 0x3f, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x03, 0xf8, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x0f, 0xe0, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x0f, 0xe0, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x1f, 0xc0, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x1f, 0xc0, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x3f, 0x80, 0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0xfc, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0xfc, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x03, 0xf8, 0x03, 0xc0, 0x1f, 0xc0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x03, 0xf8, 0x07, 0xe0, 0x1f, 0xc0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0xf0, 0x07, 0xe0, 0x0f, 0xe0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0xe0, 0x07, 0xe0, 0x07, 0xe0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0f, 0xe0, 0x07, 0xe0, 0x07, 0xf0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x1f, 0xc0, 0x07, 0xe0, 0x03, 0xf8, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x1f, 0xc0, 0x07, 0xe0, 0x03, 0xf8, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x3f, 0x80, 0x07, 0xe0, 0x01, 0xfc, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x3f, 0x80, 0x07, 0xe0, 0x01, 0xfc, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x7f, 0x00, 0x07, 0xe0, 0x00, 0xfe, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x7f, 0x00, 0x07, 0xe0, 0x00, 0xfe, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xfe, 0x00, 0x07, 0xe0, 0x00, 0x7f, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xfc, 0x00, 0x07, 0xe0, 0x00, 0x3f, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0xfc, 0x00, 0x07, 0xe0, 0x00, 0x3f, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xf8, 0x00, 0x07, 0xe0, 0x00, 0x1f, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xf8, 0x00, 0x07, 0xe0, 0x00, 0x1f, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0xf0, 0x00, 0x07, 0xe0, 0x00, 0x0f, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0xf0, 0x00, 0x07, 0xe0, 0x00, 0x0f, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x0f, 0xe0, 0x00, 0x07, 0xe0, 0x00, 0x07, 0xf0, 0x00, 0x00, 
  0x00, 0x00, 0x0f, 0xe0, 0x00, 0x07, 0xe0, 0x00, 0x07, 0xf0, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0xc0, 0x00, 0x07, 0xe0, 0x00, 0x03, 0xf8, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0x80, 0x00, 0x07, 0xe0, 0x00, 0x01, 0xf8, 0x00, 0x00, 
  0x00, 0x00, 0x3f, 0x80, 0x00, 0x07, 0xe0, 0x00, 0x01, 0xfc, 0x00, 0x00, 
  0x00, 0x00, 0x7f, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0xfe, 0x00, 0x00, 
  0x00, 0x00, 0x7f, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0xfe, 0x00, 0x00, 
  0x00, 0x00, 0xfe, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x7f, 0x00, 0x00, 
  0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 
  0x00, 0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x80, 0x00, 
  0x00, 0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x80, 0x00, 
  0x00, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xc0, 0x00, 
  0x00, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xc0, 0x00, 
  0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xe0, 0x00, 
  0x00, 0x0f, 0xe0, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x07, 0xf0, 0x00, 
  0x00, 0x0f, 0xe0, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x07, 0xf0, 0x00, 
  0x00, 0x1f, 0xc0, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x03, 0xf8, 0x00, 
  0x00, 0x1f, 0xc0, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x03, 0xf8, 0x00, 
  0x00, 0x3f, 0x80, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x01, 0xfc, 0x00, 
  0x00, 0x3f, 0x80, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x01, 0xfc, 0x00, 
  0x00, 0x7f, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0xfe, 0x00, 
  0x00, 0x7f, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0xfe, 0x00, 
  0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 
  0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 
  0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 
  0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 
  0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 
  0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 
  0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 
  0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 
  0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 
  0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 
  0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 
  0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const lv_img_dsc_t error_96 = {
  .header.cf = LV_IMG_CF_INDEXED_1BIT,
  .header.always_zero = 0,
  .header.w = 96,
  .header.h = 96,
  .data_size = 1152,
  .data = error_96_map,
};
