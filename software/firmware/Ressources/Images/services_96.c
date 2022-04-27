#include "GUI_Paint.h"

const  uint8_t services_96_map[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x1f, 0xff, 0xf8, 0xe0, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xf8, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xf8, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xfc, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xfe, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xfe, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xc3, 0xff, 0xff, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0x00, 0xff, 0xff, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xfe, 0x00, 0x7f, 0xff, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x7f, 0xfe, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0xff, 0x80, 0x3f, 0xfc, 0x00, 0x3f, 0xfe, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x3f, 0xfc, 0x00, 0x3f, 0xfc, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x3f, 0xfc, 0x00, 0x3f, 0xfc, 0x00, 
  0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x3f, 0xfc, 0x00, 0x3f, 0xfc, 0x00, 
  0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x7f, 0xfe, 0x00, 0x7f, 0xfe, 0x00, 
  0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0xff, 0xfe, 0x00, 0x7f, 0xff, 0x80, 
  0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0xfc, 0x0f, 0x00, 0xff, 0xff, 0x80, 
  0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x60, 0x07, 0xc3, 0xff, 0xff, 0x80, 
  0x00, 0x00, 0x00, 0x1f, 0xff, 0xf8, 0x00, 0x03, 0xff, 0xff, 0xff, 0x80, 
  0x00, 0x00, 0x00, 0x1f, 0xff, 0xf8, 0x00, 0x01, 0xff, 0xff, 0xff, 0x80, 
  0x00, 0x07, 0xc0, 0x3f, 0xff, 0xfc, 0x03, 0xe0, 0xff, 0xff, 0xff, 0x00, 
  0x00, 0x07, 0xf1, 0xff, 0xff, 0xff, 0x1f, 0xe0, 0xff, 0xff, 0xff, 0x00, 
  0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x7f, 0xff, 0xfe, 0x00, 
  0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x3f, 0xff, 0xfe, 0x00, 
  0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xfc, 0x00, 
  0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x1f, 0xff, 0xf8, 0x00, 
  0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x1f, 0xff, 0xf8, 0x00, 
  0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x0f, 0xf8, 0xe0, 0x00, 
  0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xf0, 0x00, 0x00, 
  0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xf0, 0x00, 0x00, 
  0x00, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0x0f, 0xe0, 0x00, 0x00, 
  0x00, 0xff, 0xff, 0xfe, 0x00, 0x7f, 0xff, 0xff, 0x0f, 0xe0, 0x00, 0x00, 
  0x00, 0xff, 0xff, 0xf8, 0x00, 0x1f, 0xff, 0xff, 0x0f, 0xe0, 0x00, 0x00, 
  0x00, 0x3f, 0xff, 0xf0, 0x00, 0x1f, 0xff, 0xfe, 0x0f, 0x00, 0x00, 0x00, 
  0x00, 0x1f, 0xff, 0xf0, 0x00, 0x0f, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x0f, 0xff, 0xe0, 0x00, 0x07, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x07, 0xff, 0xe0, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x07, 0xff, 0xc0, 0x00, 0x03, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x03, 0xff, 0xc0, 0x00, 0x03, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x03, 0xff, 0xc0, 0x00, 0x03, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x03, 0xff, 0xc0, 0x00, 0x03, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x03, 0xff, 0xc0, 0x00, 0x03, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x07, 0xff, 0xc0, 0x00, 0x03, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x07, 0xff, 0xe0, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x0f, 0xff, 0xe0, 0x00, 0x07, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x1f, 0xff, 0xf0, 0x00, 0x0f, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x7f, 0xff, 0xf0, 0x00, 0x0f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xff, 0xff, 0xfc, 0x00, 0x1f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xff, 0xff, 0xfe, 0x00, 0x7f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xff, 0xff, 0xff, 0x81, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x0f, 0xfa, 0xff, 0xff, 0xff, 0xaf, 0xf0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x03, 0xc0, 0x3f, 0xff, 0xfc, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x1f, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x1f, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const lv_img_dsc_t services_96 = {
  .header.cf = LV_IMG_CF_INDEXED_1BIT,
  .header.always_zero = 0,
  .header.w = 96,
  .header.h = 96,
  .data_size = 1152,
  .data = services_96_map,
};
