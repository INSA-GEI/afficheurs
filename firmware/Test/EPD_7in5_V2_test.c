/*****************************************************************************
 * | File      	:   EPD_7in5_test.c
 * | Author      :   Waveshare team
 * | Function    :   7.5inch e-paper test demo
 * | Info        :
 *----------------
 * |	This version:   V1.0
 * | Date        :   2019-06-13
 * | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
 ******************************************************************************/
#include "EPD_Test.h"
#include "EPD_7in5_V2.h"

int EPD_7in5_V2_test(void)
{
	printf("EPD_7IN5_V2_test Demo\r\n");
	if(DEV_Module_Init()!=0){
		return -1;
	}

	printf("e-Paper Init and Clear...\r\n");
	EPD_7IN5_V2_Init();

	printf("Paint_Init\r\n");
	Paint_Init(EPD_7IN5_V2_GetFramebuffer(),EPD_7IN5_V2_WIDTH, EPD_7IN5_V2_HEIGHT, ROTATE_0);
	Paint_SetSendToDisplayFunction(EPD_7IN5_V2_WritePicture);
	//Paint_Clear(WHITE);

	//EPD_7IN5_V2_Clear();
	//printf("Clear Display\r\n");
	//Paint_SendToDisplay();
	//DEV_Delay_ms(500);

#if 0   // show image for array
	printf("show image for array\r\n");
	//Paint_SelectImage(framebuffer);
	Paint_Clear(WHITE);
	//Paint_DrawBitMap(gImage_7in5_V2);
	Paint_DrawBitMap(pikachu);

	//EPD_7IN5_V2_WritePicture(framebuffer);
	Paint_SendToDisplay();
	DEV_Delay_ms(10000);
#endif

#if 1   // Test du support des nouvelles fonts
	//1.Select Image
	printf("Clear framebuffer\r\n");
	Paint_Clear(WHITE);

	// 2.Drawing on the image

	// Draw room number
	char *text ="GEI 109";
	lv_font_box_t stringBox = Paint_GetStringBox(text, &montserrat_li_150);
	Paint_DrawString(EPD_7IN5_V2_WIDTH-stringBox.width-20, 10, text, &montserrat_li_150, BLACK, WHITE);

	// Draw room type
	text = "Informatique";
	stringBox = Paint_GetStringBox(text, &play_r_60);
	Paint_DrawString(EPD_7IN5_V2_WIDTH-stringBox.width-20,150,
					text, &play_r_60, BLACK, WHITE);

	// Draw current reservation with two lines
	Paint_DrawLine(420, 150+80, EPD_7IN5_V2_WIDTH-20, 150+80, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	//Paint_DrawLine(320, 150+81, EPD_7IN5_V2_WIDTH-20, 150+81, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	Paint_DrawLine(220, 150+82, EPD_7IN5_V2_WIDTH-20, 150+82, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	//Paint_DrawLine(120, 150+83, EPD_7IN5_V2_WIDTH-20, 150+83, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	Paint_DrawLine(20, 150+84, EPD_7IN5_V2_WIDTH-20, 150+84, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

	text = "TP Conception d'interface 2/4";
	stringBox = Paint_GetStringBox(text, &montserrat_m_40);
	Paint_DrawString((EPD_7IN5_V2_WIDTH-stringBox.width)/2,150+85+15,
					 text, &montserrat_m_40, BLACK, WHITE);

	Paint_DrawLine(20, 150+155, EPD_7IN5_V2_WIDTH-20, 150+155, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	//Paint_DrawLine(20, 150+156, EPD_7IN5_V2_WIDTH-120, 150+156, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	Paint_DrawLine(20, 150+157, EPD_7IN5_V2_WIDTH-220, 150+157, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	//Paint_DrawLine(20, 150+158, EPD_7IN5_V2_WIDTH-320, 150+158, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	Paint_DrawLine(20, 150+159, EPD_7IN5_V2_WIDTH-420, 150+159, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

	// Draw INSA Logo
	Paint_DrawImage(20,(150+80-logoINSA_110.header.h)/2,(lv_img_dsc_t *) &logoINSA_110, DRAW_IMAGE_NORMAL);

	// Draw information about reservation
	uint16_t ypos_top = (150+160) + (((EPD_7IN5_V2_HEIGHT-30)-(150+160)) - 110)/2;
	uint16_t ypos_bottom = (EPD_7IN5_V2_HEIGHT-30) - 48 - (((EPD_7IN5_V2_HEIGHT-30)-(150+160)) - 110)/2;

	Paint_DrawImage(40, ypos_top, (lv_img_dsc_t *) &clock_start_48, DRAW_IMAGE_NORMAL);
	Paint_DrawImage(40, ypos_bottom, (lv_img_dsc_t *) &clock_end_48, DRAW_IMAGE_NORMAL);

	text = "10:00";
	stringBox = Paint_GetStringBox(text, &montserrat_m_26);
	Paint_DrawString(40 + clock_start_48.header.w +20, ypos_top+11, text, &montserrat_m_26, BLACK, WHITE);

	text = "12:45";
	stringBox = Paint_GetStringBox(text, &montserrat_m_26);
	Paint_DrawString(40 + clock_end_48.header.w +20, ypos_bottom+11, text, &montserrat_m_26, BLACK, WHITE);

	Paint_DrawImage(300, ypos_top, (lv_img_dsc_t *) &attendees_48, DRAW_IMAGE_NORMAL);
	Paint_DrawImage(300, ypos_bottom, (lv_img_dsc_t *) &teacher_48, DRAW_IMAGE_NORMAL);

	text = "4AE-SE";
	stringBox = Paint_GetStringBox(text, &montserrat_m_26);
	Paint_DrawString(300 + attendees_48.header.w +20, ypos_top+11, text, &montserrat_m_26, BLACK, WHITE);

	text = "S. DI MERCURIO";
	stringBox = Paint_GetStringBox(text, &montserrat_m_26);
	Paint_DrawString(300 + attendees_48.header.w +20, ypos_bottom+11, text, &montserrat_m_26, BLACK, WHITE);

	// Draw footer with next reservation, connection status and battery state
	Paint_DrawRectangle(0, EPD_7IN5_V2_HEIGHT-30, EPD_7IN5_V2_WIDTH, EPD_7IN5_V2_HEIGHT, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
	text = "Relaxation neuronale (13:00 - 13:45)";
	stringBox = Paint_GetStringBox(text, &montserrat_m_26);
	Paint_DrawString((EPD_7IN5_V2_WIDTH-stringBox.width)/2,EPD_7IN5_V2_HEIGHT-30+(30-26)/2,
				     text, &montserrat_m_26, WHITE, BLACK);

	Paint_DrawImage((EPD_7IN5_V2_WIDTH-stringBox.width)/2 - clock_next_24.header.w - 8,
			        (EPD_7IN5_V2_HEIGHT-30 + (30-clock_next_24.header.h)/2),
				    (lv_img_dsc_t *) &clock_next_24, DRAW_IMAGE_INVERTED);

	Paint_DrawImage(10,(EPD_7IN5_V2_HEIGHT-30 + (30-connection_mid_24.header.h)/2),
			        (lv_img_dsc_t *) &connection_mid_24, DRAW_IMAGE_INVERTED);

	Paint_DrawImage(10+connection_mid_24.header.w+4,(EPD_7IN5_V2_HEIGHT-30 + (30-on_line_24.header.h)/2),
			        (lv_img_dsc_t *) &on_line_24, DRAW_IMAGE_INVERTED);

	Paint_DrawImage(EPD_7IN5_V2_WIDTH-10-24,(EPD_7IN5_V2_HEIGHT-30 + (30-battery_high_24.header.h)/2),
			        (lv_img_dsc_t *) &battery_high_24, DRAW_IMAGE_INVERTED);

	Paint_SendToDisplay();

	printf("EPD_Display\r\n");
	DEV_Delay_ms(10000);
#endif

#if 0   // Drawing on the image
	//1.Select Image
	printf("Clear framebuffer\r\n");
	Paint_Clear(WHITE);

	// 2.Drawing on the image
	printf("Drawing primitives\r\n");
	Paint_DrawPoint(10, 80, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
	Paint_DrawPoint(10, 90, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
	Paint_DrawPoint(10, 100, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
	Paint_DrawLine(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	Paint_DrawLine(70, 70, 20, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
	Paint_DrawRectangle(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
	Paint_DrawRectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
	Paint_DrawCircle(45, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
	Paint_DrawCircle(105, 95, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
	Paint_DrawLine(85, 95, 125, 95, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
	Paint_DrawLine(105, 75, 105, 115, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
	Paint_DrawString_EN(10, 0, "waveshare", &Font16, BLACK, WHITE);
	Paint_DrawString_EN(10, 20, "hello world", &Font12, WHITE, BLACK);
	Paint_DrawNum(10, 33, 123456789, &Font12, BLACK, WHITE);
	Paint_DrawNum(10, 50, 987654321, &Font16, WHITE, BLACK);

	/*EPD_7IN5_V2_WritePicture(framebuffer, 0);
	Paint_Clear(WHITE);
	EPD_7IN5_V2_WritePicture(framebuffer, 1);*/
	Paint_SendToDisplay();

	printf("EPD_Display\r\n");
	DEV_Delay_ms(10000);
#endif

#if 0
	printf("Clear...\r\n");
	//EPD_7IN5_V2_Clear();
	Paint_Clear(WHITE);
	Paint_SendToDisplay();

	printf("Goto Sleep...\r\n");
	EPD_7IN5_V2_Sleep();
	//free(framebuffer);
	//framebuffer = NULL;

#endif

	// close 5V
	printf("close 5V, Module enters 0 power consumption ...\r\n");
	DEV_Module_Exit();

	return 0;
}

