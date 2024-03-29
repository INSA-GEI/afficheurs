#include "GUI_Paint.h"

const  uint8_t finish_line_48_map[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0xff, 0xff, 0xff, 0xff, 0xc0, 
  0x03, 0xff, 0xff, 0xff, 0xff, 0xc0, 
  0x03, 0xff, 0xff, 0xff, 0xff, 0xc0, 
  0x03, 0x80, 0xff, 0x00, 0xff, 0xc0, 
  0x03, 0x80, 0xff, 0x00, 0xff, 0xc0, 
  0x03, 0x80, 0xff, 0x00, 0xff, 0xc0, 
  0x03, 0x80, 0xff, 0x00, 0xff, 0xc0, 
  0x03, 0x80, 0xff, 0x00, 0xff, 0xc0, 
  0x03, 0x80, 0xff, 0x00, 0xff, 0xc0, 
  0x03, 0x80, 0xff, 0x00, 0xff, 0xc0, 
  0x03, 0xff, 0x00, 0xff, 0x01, 0xc0, 
  0x03, 0xff, 0x00, 0xff, 0x01, 0xc0, 
  0x03, 0xff, 0x00, 0xff, 0x01, 0xc0, 
  0x03, 0xff, 0x00, 0xff, 0x01, 0xc0, 
  0x03, 0xff, 0x00, 0xff, 0x01, 0xc0, 
  0x03, 0xff, 0x00, 0xff, 0x01, 0xc0, 
  0x03, 0xff, 0x00, 0xff, 0x01, 0xc0, 
  0x03, 0xff, 0x00, 0xff, 0x01, 0xc0, 
  0x03, 0x80, 0xff, 0x00, 0xff, 0xc0, 
  0x03, 0x80, 0xff, 0x00, 0xff, 0xc0, 
  0x03, 0x80, 0xff, 0x00, 0xff, 0xc0, 
  0x03, 0x80, 0xff, 0x00, 0xff, 0xc0, 
  0x03, 0x80, 0xff, 0x00, 0xff, 0xc0, 
  0x03, 0x80, 0xff, 0x00, 0xff, 0xc0, 
  0x03, 0x80, 0xff, 0x00, 0xff, 0xc0, 
  0x03, 0xff, 0xff, 0xff, 0xff, 0xc0, 
  0x03, 0xff, 0xff, 0xff, 0xff, 0xc0, 
  0x03, 0xff, 0xff, 0xff, 0xff, 0xc0, 
  0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const lv_img_dsc_t finish_line_48 = {
  .header.always_zero = 0,
  .header.w = 48,
  .header.h = 48,
  .data_size = 288,
  .header.cf = LV_IMG_CF_INDEXED_1BIT,
  .data = finish_line_48_map,
};
