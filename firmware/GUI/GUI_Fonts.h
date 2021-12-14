/*
 * GUI_fonts.h
 *
 *  Created on: 13 déc. 2021
 *      Author: dimercur
 */

#ifndef GUI_FONTS_H_
#define GUI_FONTS_H_

#define FONT_INTERCHAR_SPACE 1

/* Font support */
/** Describes the properties of a glyph. */
typedef struct {
    uint16_t w_px; /**< size (width) of the glyph */
    uint32_t glyph_index;  /**< Offset un glyph array*/
} lv_font_glyph_dsc_t;

/** Describe the properties of a font*/
typedef struct _lv_font_struct {
	uint16_t unicode_first;	/*First Unicode letter in this font*/
	uint16_t unicode_last;	/*Last Unicode letter in this font*/
	uint16_t h_px;			/*Font height in pixels*/
	const uint8_t *glyph_bitmap;	/*Bitmap of glyphs*/
	const lv_font_glyph_dsc_t *glyph_dsc;		/*Description of glyphs*/
	uint16_t glyph_cnt;			/*Number of glyphs in the font*/
	const uint32_t *unicode_list;	/*Every character in the font from 'unicode_first' to 'unicode_last'*/
	const char *get_bitmap;	/*Function pointer to get glyph's bitmap*/
	const uint16_t *get_width;	/*Function pointer to get glyph's width*/
	uint8_t bpp;				/*Bit per pixel*/
	uint16_t monospace;		/*Fix width (0: if not used)*/
	const uint8_t *next_page;		/*Pointer to a font extension*/

} lv_font_t;

typedef struct {
	uint16_t width;
	uint16_t height;
} lv_font_box_t;

#endif /* GUI_FONTS_H_ */
