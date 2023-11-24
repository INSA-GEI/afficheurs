/******************************************************************************
 * | File      	:   GUI_Paint.c
 * | Author      :   Waveshare electronics
 * | Function    :	Achieve drawing: draw points, lines, boxes, circles and
 *                   their size, solid dotted line, solid rectangle hollow
 *                   rectangle, solid circle hollow circle.
 * | Info        :
 *   Achieve display characters: Display a single character, string, number
 *   Achieve time display: adaptive size display time minutes and seconds
 *----------------
 * |	This version:   V3.1
 * | Date        :   2020-07-08
 * | Info        :
 * -----------------------------------------------------------------------------
 * V3.1(2020-07-08):
 * 1.Change: Paint_SetScale(UBYTE scale)
 *		 Add scale 7 for 5.65f e-Parper
 * 2.Change: Paint_SetPixel(UWORD Xpoint, UWORD Ypoint, UWORD Color)
 *		 Add the branch for scale 7
 * 3.Change: Paint_Clear(UWORD Color)
 *		 Add the branch for scale 7
 *
 * -----------------------------------------------------------------------------
 * V3.0(2019-04-18):
 * 1.Change:
 *    Paint_DrawPoint(..., DOT_STYLE DOT_STYLE)
 * => Paint_DrawPoint(..., DOT_STYLE Dot_Style)
 *    Paint_DrawLine(..., LINE_STYLE Line_Style, DOT_PIXEL Dot_Pixel)
 * => Paint_DrawLine(..., DOT_PIXEL Line_width, LINE_STYLE Line_Style)
 *    Paint_DrawRectangle(..., DRAW_FILL Filled, DOT_PIXEL Dot_Pixel)
 * => Paint_DrawRectangle(..., DOT_PIXEL Line_width, DRAW_FILL Draw_Fill)
 *    Paint_DrawCircle(..., DRAW_FILL Draw_Fill, DOT_PIXEL Dot_Pixel)
 * => Paint_DrawCircle(..., DOT_PIXEL Line_width, DRAW_FILL Draw_Filll)
 *
 * -----------------------------------------------------------------------------
 * V2.0(2018-11-15):
 * 1.add: Paint_NewImage()
 *    Create an image's properties
 * 2.add: Paint_SelectImage()
 *    Select the picture to be drawn
 * 3.add: Paint_SetRotate()
 *    Set the direction of the cache
 * 4.add: Paint_RotateImage()
 *    Can flip the picture, Support 0-360 degrees,
 *    but only 90.180.270 rotation is better
 * 4.add: Paint_SetMirroring()
 *    Can Mirroring the picture, horizontal, vertical, origin
 * 5.add: Paint_DrawString_CN()
 *    Can display Chinese(GB1312)
 *
 * -----------------------------------------------------------------------------
 * V1.0(2018-07-17):
 *   Create library
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ******************************************************************************/
#include "GUI_Paint.h"
#include "DEV_Config.h"
#include "Debug.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h> //memset()
#include <math.h>

#include "EPD_7in5_V2.h"
#include "config.h"

PAINT Paint;

/******************************************************************************
function: Create Image
parameter:
	framebuffer :   address of framebuffer
    width   :   The width of the picture
    Height  :   The height of the picture
    Color   :   Whether the picture is inverted
 ******************************************************************************/
void Paint_Init(uint8_t *framebuffer, uint16_t width, uint16_t height, uint16_t Rotate)
{
	//Paint.Image = NULL;
	Paint.Image = framebuffer;

	Paint.WidthMemory = width;
	Paint.HeightMemory = height;
	Paint.Color = WHITE;
	Paint.Scale = 2;

	Paint.WidthByte = (width % 8 == 0)? (width / 8 ): (width / 8 + 1);
	Paint.HeightByte = height;
	//    printf("WidthByte = %d, HeightByte = %d\r\n", Paint.WidthByte, Paint.HeightByte);
	//    printf(" EPD_WIDTH / 8 = %d\r\n",  122 / 8);

	Paint.Rotate = Rotate;
	Paint.Mirror = MIRROR_NONE;

	if(Rotate == ROTATE_0 || Rotate == ROTATE_180) {
		Paint.Width = width;
		Paint.Height = height;
	} else {
		Paint.Width = height;
		Paint.Height = width;
	}

	Paint.sendToDisplayFct = NULL;
	Paint.setDrawingWindowFct = NULL;
}

/******************************************************************************
function: Select Image
parameter:
    image : Pointer to the image cache
 ******************************************************************************/
//void Paint_SelectImage(uint8_t *image)
//{
//    Paint.Image = image;
//}

/******************************************************************************
function: Select Image Rotate
parameter:
    Rotate : 0,90,180,270
 ******************************************************************************/
void Paint_SetRotate(uint16_t Rotate)
{
	if(Rotate == ROTATE_0 || Rotate == ROTATE_90 || Rotate == ROTATE_180 || Rotate == ROTATE_270) {
		Debug("Set image Rotate %d\r\n", Rotate);
		Paint.Rotate = Rotate;
	} else {
		Debug("rotate = 0, 90, 180, 270\r\n");
	}
}

void Paint_SetScale(uint8_t scale)
{
	if(scale == 2){
		Paint.Scale = scale;
		Paint.WidthByte = (Paint.WidthMemory % 8 == 0)? (Paint.WidthMemory / 8 ): (Paint.WidthMemory / 8 + 1);
	}else if(scale == 4){
		Paint.Scale = scale;
		Paint.WidthByte = (Paint.WidthMemory % 4 == 0)? (Paint.WidthMemory / 4 ): (Paint.WidthMemory / 4 + 1);
	}else if(scale == 7){//Only applicable with 5in65 e-Paper
		Paint.Scale = scale;
		Paint.WidthByte = (Paint.WidthMemory % 2 == 0)? (Paint.WidthMemory / 2 ): (Paint.WidthMemory / 2 + 1);;
	}else{
		Debug("Set Scale Input parameter error\r\n");
		Debug("Scale Only support: 2 4 7\r\n");
	}
}
/******************************************************************************
function:	Select Image mirror
parameter:
    mirror   :Not mirror,Horizontal mirror,Vertical mirror,Origin mirror
 ******************************************************************************/
void Paint_SetMirroring(uint8_t mirror)
{
	if(mirror == MIRROR_NONE || mirror == MIRROR_HORIZONTAL ||
			mirror == MIRROR_VERTICAL || mirror == MIRROR_ORIGIN) {
		Debug("mirror image x:%s, y:%s\r\n",(mirror & 0x01)? "mirror":"none", ((mirror >> 1) & 0x01)? "mirror":"none");
		Paint.Mirror = mirror;
	} else {
		Debug("mirror should be MIRROR_NONE, MIRROR_HORIZONTAL, \
        MIRROR_VERTICAL or MIRROR_ORIGIN\r\n");
	}
}

void Paint_SetSendToDisplayFunction(sendToDisplayCbk callback)
{
	Paint.sendToDisplayFct = callback;
}

void Paint_SetDrawingWindowFunction(setDrawingWindowCbk callback)
{
	Paint.setDrawingWindowFct = callback;
}

/******************************************************************************
function: Draw Pixels
parameter:
    Xpoint : At point X
    Ypoint : At point Y
    Color  : Painted colors
 ******************************************************************************/
void Paint_SetPixel(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color)
{
	if(Xpoint > Paint.Width || Ypoint > Paint.Height){
		Debug("Exceeding display boundaries\r\n");
		return;
	}
	uint16_t X, Y;

	switch(Paint.Rotate) {
	case 0:
		X = Xpoint;
		Y = Ypoint;
		break;
	case 90:
		X = Paint.WidthMemory - Ypoint - 1;
		Y = Xpoint;
		break;
	case 180:
		X = Paint.WidthMemory - Xpoint - 1;
		Y = Paint.HeightMemory - Ypoint - 1;
		break;
	case 270:
		X = Ypoint;
		Y = Paint.HeightMemory - Xpoint - 1;
		break;
	default:
		return;
	}

	switch(Paint.Mirror) {
	case MIRROR_NONE:
		break;
	case MIRROR_HORIZONTAL:
		X = Paint.WidthMemory - X - 1;
		break;
	case MIRROR_VERTICAL:
		Y = Paint.HeightMemory - Y - 1;
		break;
	case MIRROR_ORIGIN:
		X = Paint.WidthMemory - X - 1;
		Y = Paint.HeightMemory - Y - 1;
		break;
	default:
		return;
	}

	if(X > Paint.WidthMemory || Y > Paint.HeightMemory){
		Debug("Exceeding display boundaries\r\n");
		return;
	}

	if(Paint.Scale == 2){
		uint32_t Addr = X / 8 + Y * Paint.WidthByte;
		uint8_t Rdata = Paint.Image[Addr];
		if(Color == WHITE)
			Paint.Image[Addr] = Rdata & ~(0x80 >> (X % 8));
		else
			Paint.Image[Addr] = Rdata | (0x80 >> (X % 8));
	}else if(Paint.Scale == 4){
		uint32_t Addr = X / 4 + Y * Paint.WidthByte;
		Color = Color % 4;//Guaranteed color scale is 4  --- 0~3
		uint8_t Rdata = Paint.Image[Addr];

		Rdata = Rdata & (~(0xC0 >> ((X % 4)*2)));
		Paint.Image[Addr] = Rdata | ((Color << 6) >> ((X % 4)*2));
	}else if(Paint.Scale == 7){
		uint32_t Addr = X / 2  + Y * Paint.WidthByte;
		uint8_t Rdata = Paint.Image[Addr];
		Rdata = Rdata & (~(0xF0 >> ((X % 2)*4)));//Clear first, then set value
		Paint.Image[Addr] = Rdata | ((Color << 4) >> ((X % 2)*4));
		//printf("Add =  %d ,data = %d\r\n",Addr,Rdata);
	}
}

/******************************************************************************
function: Clear the color of the picture
parameter:
    Color : Painted colors
 ******************************************************************************/
void Paint_Clear(uint16_t Color)
{
	volatile uint32_t val;
	DEBUG_StartTimeMeasure();

	asm("push {R0,R1}\n\t"
			"eor R0,R0\n\t"
			"eor R1,R1\n\t"
			"debut: \n\t"
			"cmp   R0,%[length]\n\t"
			"bcs   fin\n\t"
			"str   R1,[%[address],R0]\n\t"
			"add   R0,#4\n\t"
			"b  debut\n"
			"fin:\n\t"
			"pop {R0,R1}"

			: : [address]"r"   (Paint.Image),
			  [length]"r"    ((800*480)/8)
	);

	val=DEBUG_EndTimeMeasure();
	val++;
}

/******************************************************************************
function: Clear the color of a window
parameter:
    Xstart : x starting point
    Ystart : Y starting point
    Yend   : y end point
    Color  : Painted colors
 ******************************************************************************/
void Paint_ClearWindows(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t Color)
{
	uint16_t X, Y;
	for (Y = Ystart; Y < Yend; Y++) {
		for (X = Xstart; X < Xend; X++) {//8 pixel =  1 byte
			Paint_SetPixel(X, Y, Color);
		}
	}
}

/******************************************************************************
function: Draw Point(Xpoint, Ypoint) Fill the color
parameter:
    Xpoint		: The Xpoint coordinate of the point
    Ypoint		: The Ypoint coordinate of the point
    Color		: Painted color
    Dot_Pixel	: point size
    Dot_Style	: point Style
 ******************************************************************************/
void Paint_DrawPoint(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color,
		DOT_PIXEL Dot_Pixel, DOT_STYLE Dot_Style)
{
	if (Xpoint > Paint.Width || Ypoint > Paint.Height) return;

	int16_t XDir_Num , YDir_Num;
	if (Dot_Style == DOT_FILL_AROUND) {
		for (XDir_Num = 0; XDir_Num < 2 * Dot_Pixel - 1; XDir_Num++) {
			for (YDir_Num = 0; YDir_Num < 2 * Dot_Pixel - 1; YDir_Num++) {
				if(Xpoint + XDir_Num - Dot_Pixel < 0 || Ypoint + YDir_Num - Dot_Pixel < 0)
					break;
				// printf("x = %d, y = %d\r\n", Xpoint + XDir_Num - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel);
				Paint_SetPixel(Xpoint + XDir_Num - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel, Color);
			}
		}
	} else {
		for (XDir_Num = 0; XDir_Num <  Dot_Pixel; XDir_Num++) {
			for (YDir_Num = 0; YDir_Num <  Dot_Pixel; YDir_Num++) {
				Paint_SetPixel(Xpoint + XDir_Num - 1, Ypoint + YDir_Num - 1, Color);
			}
		}
	}
}

/******************************************************************************
function: Draw a line of arbitrary slope
parameter:
    Xstart ：Starting Xpoint point coordinates
    Ystart ：Starting Xpoint point coordinates
    Xend   ：End point Xpoint coordinate
    Yend   ：End point Ypoint coordinate
    Color  ：The color of the line segment
    Line_width : Line width
    Line_Style: Solid and dotted lines
 ******************************************************************************/
void Paint_DrawLine(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend,
		uint16_t Color, DOT_PIXEL Line_width, LINE_STYLE Line_Style)
{
	if (Xstart > Paint.Width || Ystart > Paint.Height ||
			Xend > Paint.Width || Yend > Paint.Height) {
		Debug("Paint_DrawLine Input exceeds the normal display range\r\n");
		return;
	}

	uint16_t Xpoint = Xstart;
	uint16_t Ypoint = Ystart;
	int dx = (int)Xend - (int)Xstart >= 0 ? Xend - Xstart : Xstart - Xend;
	int dy = (int)Yend - (int)Ystart <= 0 ? Yend - Ystart : Ystart - Yend;

	// Increment direction, 1 is positive, -1 is counter;
	int XAddway = Xstart < Xend ? 1 : -1;
	int YAddway = Ystart < Yend ? 1 : -1;

	//Cumulative error
	int Esp = dx + dy;
	char Dotted_Len = 0;

	for (;;) {
		Dotted_Len++;
		//Painted dotted line, 2 point is really virtual
		if (Line_Style == LINE_STYLE_DOTTED && Dotted_Len % 3 == 0) {
			//Debug("LINE_DOTTED\r\n");
			Paint_DrawPoint(Xpoint, Ypoint, IMAGE_BACKGROUND, Line_width, DOT_STYLE_DFT);
			Dotted_Len = 0;
		} else {
			Paint_DrawPoint(Xpoint, Ypoint, Color, Line_width, DOT_STYLE_DFT);
		}
		if (2 * Esp >= dy) {
			if (Xpoint == Xend)
				break;
			Esp += dy;
			Xpoint += XAddway;
		}
		if (2 * Esp <= dx) {
			if (Ypoint == Yend)
				break;
			Esp += dx;
			Ypoint += YAddway;
		}
	}
}

/******************************************************************************
function: Draw a rectangle
parameter:
    Xstart ：Rectangular  Starting Xpoint point coordinates
    Ystart ：Rectangular  Starting Xpoint point coordinates
    Xend   ：Rectangular  End point Xpoint coordinate
    Yend   ：Rectangular  End point Ypoint coordinate
    Color  ：The color of the Rectangular segment
    Line_width: Line width
    Draw_Fill : Whether to fill the inside of the rectangle
 ******************************************************************************/
void Paint_DrawRectangle(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend,
		uint16_t Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill)
{
	if (Xstart > Paint.Width || Ystart > Paint.Height ||
			Xend > Paint.Width || Yend > Paint.Height) {
		Debug("Input exceeds the normal display range\r\n");
		return;
	}

	if (Draw_Fill) {
		uint16_t Ypoint;
		for(Ypoint = Ystart; Ypoint < Yend; Ypoint++) {
			Paint_DrawLine(Xstart, Ypoint, Xend, Ypoint, Color , Line_width, LINE_STYLE_SOLID);
		}
	} else {
		Paint_DrawLine(Xstart, Ystart, Xend, Ystart, Color, Line_width, LINE_STYLE_SOLID);
		Paint_DrawLine(Xstart, Ystart, Xstart, Yend, Color, Line_width, LINE_STYLE_SOLID);
		Paint_DrawLine(Xend, Yend, Xend, Ystart, Color, Line_width, LINE_STYLE_SOLID);
		Paint_DrawLine(Xend, Yend, Xstart, Yend, Color, Line_width, LINE_STYLE_SOLID);
	}
}

/******************************************************************************
function: Draw a rectangle
parameter:
    Xstart ：Rectangular  Starting Xpoint point coordinates
    Ystart ：Rectangular  Starting Xpoint point coordinates
    Xend   ：Rectangular  End point Xpoint coordinate
    Yend   ：Rectangular  End point Ypoint coordinate
    Color  ：The color of the Rectangular segment
    Line_width: Line width
    Draw_Fill : Whether to fill the inside of the rectangle
 ******************************************************************************/
void Paint_DrawFillRectangle(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t Color)
{
	uint8_t shift;

	if (Xstart > Paint.Width || Ystart > Paint.Height ||
			Xend > Paint.Width || Yend > Paint.Height) {
		Debug("Input exceeds the normal display range\r\n");
		return;
	}

	for (uint16_t Y = Ystart; Y < Yend; Y++) {
		for (uint16_t X = Xstart; X < Xend; X++ ) {
			uint32_t Addr = (X + Y*800)/8;
			shift = (uint8_t)X%8;
			shift = 7-shift;

			if (Color == BLACK)
				Paint.Image[Addr] = Paint.Image[Addr] | ((uint8_t)0x1<<shift);
			else
				Paint.Image[Addr] = Paint.Image[Addr] & ~((uint8_t)0x1<<shift);
		}
	}
}

/******************************************************************************
function: Draw a rectangle
parameter:
    Xstart ：Rectangular  Starting Xpoint point coordinates
    Ystart ：Rectangular  Starting Xpoint point coordinates
    Xend   ：Rectangular  End point Xpoint coordinate
    Yend   ：Rectangular  End point Ypoint coordinate
    Color  ：The color of the Rectangular segment
    Line_width: Line width
    Draw_Fill : Whether to fill the inside of the rectangle
 ******************************************************************************/
void Paint_DrawFillRectangleOptim(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t Color)
{
	uint8_t shift;
	uint16_t plain_x_start;
	uint16_t plain_x_end;

	if (Xstart > Paint.Width || Ystart > Paint.Height ||
			Xend > Paint.Width || Yend > Paint.Height) {
		Debug("Input exceeds the normal display range\r\n");
		return;
	}

	for (uint16_t Y = Ystart; Y < Yend; Y++) {
		if (Xstart%8 ==0)
			plain_x_start = (Xstart>>3)<<3;
		else
			plain_x_start = 8+((Xstart>>3)<<3);

		plain_x_end = (Xend>>3)<<3;

		if (Xstart%8 !=0) {
			for (uint16_t X= Xstart; X < plain_x_start; X++ ) {
				uint32_t Addr = (X + Y*800)>>3;
				shift = (uint8_t)X%8;
				shift = 7-shift;

				if (Color == BLACK)
					Paint.Image[Addr] = Paint.Image[Addr] | ((uint8_t)0x1<<shift);
				else
					Paint.Image[Addr] = Paint.Image[Addr] & ~((uint8_t)0x1<<shift);
			}
		}

		for (uint16_t X = plain_x_start; X<plain_x_end; X++) {
			uint32_t Addr = (X + Y*800)>>3;
			if (Color == BLACK)
				Paint.Image[Addr] = BLACK;
			else
				Paint.Image[Addr] = (uint8_t)(~BLACK);
		}

		if (Xend%8 !=0) {
			for (uint16_t X= plain_x_end; X < Xend; X++ ) {
				uint32_t Addr = (X + Y*800)>>3;
				shift = (uint8_t)X%8;
				shift = 7-shift;

				if (Color == BLACK)
					Paint.Image[Addr] = Paint.Image[Addr] | ((uint8_t)0x1<<shift);
				else
					Paint.Image[Addr] = Paint.Image[Addr] & ~((uint8_t)0x1<<shift);
			}
		}
	}
}

/******************************************************************************
function: Use the 8-point method to draw a circle of the
            specified size at the specified position->
parameter:
    X_Center  ：Center X coordinate
    Y_Center  ：Center Y coordinate
    Radius    ：circle Radius
    Color     ：The color of the ：circle segment
    Line_width: Line width
    Draw_Fill : Whether to fill the inside of the Circle
 ******************************************************************************/
void Paint_DrawCircle(uint16_t X_Center, uint16_t Y_Center, uint16_t Radius,
		uint16_t Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill)
{
	if (X_Center > Paint.Width || Y_Center >= Paint.Height) {
		Debug("Paint_DrawCircle Input exceeds the normal display range\r\n");
		return;
	}

	//Draw a circle from(0, R) as a starting point
	int16_t XCurrent, YCurrent;
	XCurrent = 0;
	YCurrent = Radius;

	//Cumulative error,judge the next point of the logo
	int16_t Esp = 3 - (Radius << 1 );

	int16_t sCountY;
	if (Draw_Fill == DRAW_FILL_FULL) {
		while (XCurrent <= YCurrent ) { //Realistic circles
			for (sCountY = XCurrent; sCountY <= YCurrent; sCountY ++ ) {
				Paint_DrawPoint(X_Center + XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//1
				Paint_DrawPoint(X_Center - XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//2
				Paint_DrawPoint(X_Center - sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//3
				Paint_DrawPoint(X_Center - sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//4
				Paint_DrawPoint(X_Center - XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//5
				Paint_DrawPoint(X_Center + XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//6
				Paint_DrawPoint(X_Center + sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//7
				Paint_DrawPoint(X_Center + sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);
			}
			if (Esp < 0 )
				Esp += 4 * XCurrent + 6;
			else {
				Esp += 10 + 4 * (XCurrent - YCurrent );
				YCurrent --;
			}
			XCurrent ++;
		}
	} else { //Draw a hollow circle
		while (XCurrent <= YCurrent ) {
			Paint_DrawPoint(X_Center + XCurrent, Y_Center + YCurrent, Color, Line_width, DOT_STYLE_DFT);//1
			Paint_DrawPoint(X_Center - XCurrent, Y_Center + YCurrent, Color, Line_width, DOT_STYLE_DFT);//2
			Paint_DrawPoint(X_Center - YCurrent, Y_Center + XCurrent, Color, Line_width, DOT_STYLE_DFT);//3
			Paint_DrawPoint(X_Center - YCurrent, Y_Center - XCurrent, Color, Line_width, DOT_STYLE_DFT);//4
			Paint_DrawPoint(X_Center - XCurrent, Y_Center - YCurrent, Color, Line_width, DOT_STYLE_DFT);//5
			Paint_DrawPoint(X_Center + XCurrent, Y_Center - YCurrent, Color, Line_width, DOT_STYLE_DFT);//6
			Paint_DrawPoint(X_Center + YCurrent, Y_Center - XCurrent, Color, Line_width, DOT_STYLE_DFT);//7
			Paint_DrawPoint(X_Center + YCurrent, Y_Center + XCurrent, Color, Line_width, DOT_STYLE_DFT);//0

			if (Esp < 0 )
				Esp += 4 * XCurrent + 6;
			else {
				Esp += 10 + 4 * (XCurrent - YCurrent );
				YCurrent --;
			}
			XCurrent ++;
		}
	}
}

/******************************************************************************
function: Show English characters
parameter:
    Xpoint           ：X coordinate
    Ypoint           ：Y coordinate
    Acsii_Char       ：To display the English characters
    Font             ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
 ******************************************************************************/

uint32_t Paint_GetGlyphOffsetAndWidth(const uint16_t Unicode_char, const lv_font_t* Font, uint16_t *width) {
	uint16_t charOffset=0;

	/* Compute char offset in glyph description table */
	if (Font->unicode_list != NULL) {
		int i=0;

		while (Font->unicode_list[i] != 0) {
			if (Font->unicode_list[i] == Unicode_char) {
				charOffset = i;
				break;
			} else i++;
		}
	} else {
		if ((Unicode_char<Font->unicode_first) || (Unicode_char>Font->unicode_last)) {
			charOffset = 0; /* If Unicode_char is invalid (outside range), use by default first char in font (offset 0) */
		}
		else {
			charOffset = (Unicode_char - Font->unicode_first);
		}
	}

	*width = Font->glyph_dsc[charOffset].w_px;
	return Font->glyph_dsc[charOffset].glyph_index;
}

uint16_t Paint_DrawChar (uint16_t Xpoint, uint16_t Ypoint, const uint16_t Unicode_char,
		const lv_font_t* Font, uint16_t Color_Foreground, uint16_t Color_Background) {
	uint16_t Page, Column;
	uint16_t glyphWidth =0;
	uint32_t glyphOffset =0;

	if (Xpoint > Paint.Width || Ypoint > Paint.Height) {
		Debug("Paint_DrawChar Input exceeds the normal display range\r\n");
		return glyphWidth;
	}

	glyphOffset=Paint_GetGlyphOffsetAndWidth(Unicode_char, Font, &glyphWidth);

	const uint8_t *ptr = &Font->glyph_bitmap[glyphOffset];

	for (Page = 0; Page < Font->h_px; Page ++ ) {
		for (Column = 0; Column < glyphWidth; Column ++ ) {

			//To determine whether the font background color and screen background color is consistent
			if (FONT_BACKGROUND == Color_Background) { //this process is to speed up the scan
				if (*ptr & (0x80 >> (Column % 8)))
					Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Foreground);
				// Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
			} else {
				if (*ptr & (0x80 >> (Column % 8))) {
					Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Foreground);
					// Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
				} else {
					Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Background);
					// Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
				}
			}
			//One pixel is 8 bits
			if (Column % 8 == 7)
				ptr++;
		}// Write a line
		if (glyphWidth % 8 != 0)
			ptr++;
	}// Write all

	return glyphWidth;
}

void Paint_DrawString (uint16_t Xstart, uint16_t Ystart, const char *pString,
		const lv_font_t* Font, uint16_t Color_Foreground, uint16_t Color_Background) {

	uint16_t Xpoint = Xstart;
	uint16_t Ypoint = Ystart;

	uint16_t charWidth=0;
	uint16_t fontHeight = Font->h_px;

	if (Xstart > Paint.Width || Ystart > Paint.Height) {
		Debug("Paint_DrawString_EN Input exceeds the normal display range\r\n");
		return;
	}

	while (* pString != '\0') {
		if (* pString == '\n') {
			Ypoint = Ypoint  + fontHeight;
			Xpoint = Xstart;
			if (Ypoint >= Paint.Height) return;
		}
		else if (Xpoint < Paint.Width) {
			charWidth = Paint_DrawChar(Xpoint, Ypoint, (uint16_t)*pString, Font, Color_Foreground, Color_Background);
			//The next word of the abscissa increases the font of the broadband
			Xpoint += charWidth+FONT_INTERCHAR_SPACE;
		}

		//The next character of the address
		pString ++;
	}
}

/*
 * *****************************************
 * *****************************************
 */

lv_font_box_t Paint_GetStringBox(const char *pString, const lv_font_t* Font)
{
	uint16_t lineWidth=0;
	uint16_t charWidth=0;
	uint16_t fontHeight = Font->h_px;
	lv_font_box_t box = {
						.width = 0,
						.height = Font->h_px};

	while (* pString != '\0')
	{
		if (* pString == '\n')
		{
			box.height += fontHeight;
			if (lineWidth>box.width) box.width = lineWidth;
			lineWidth=0;
		}
		else
		{
			Paint_GetGlyphOffsetAndWidth((uint16_t)*pString, Font, &charWidth);
			lineWidth += charWidth+FONT_INTERCHAR_SPACE;
		}
		pString ++;
	}
	if (lineWidth>box.width) box.width = lineWidth;
	return box;
}

/******************************************************************************
function:	Display monochrome bitmap
parameter:
    image_buffer ：A picture data converted to a bitmap
info:
    Use a computer to convert the image into a corresponding array,
    and then embed the array directly into Imagedata.cpp as a .c file.
 ******************************************************************************/
void Paint_DrawBitMap(const uint8_t* image_buffer)
{
	uint16_t x, y;
	uint32_t Addr = 0;

	for (y = 0; y < Paint.HeightByte; y++) {
		for (x = 0; x < Paint.WidthByte; x++) {//8 pixel =  1 byte
			Addr = x + y * Paint.WidthByte;
			Paint.Image[Addr] = (unsigned char)image_buffer[Addr];
		}
	}
}

/******************************************************************************
function:	Display monochrome bitmap
parameter:
    image_buffer ：A picture data converted to a bitmap
info:
    Use a computer to convert the image into a corresponding array,
    and then embed the array directly into Imagedata.cpp as a .c file.
 ******************************************************************************/
void Paint_DrawImage(uint16_t x, uint16_t y, lv_img_dsc_t *img, DRAW_IMAGE inverted)
{
	uint8_t tmp=0;
	uint8_t *ptr;
	uint16_t color=BLACK;

	if (x > Paint.Width || y > Paint.Height) {
		Debug("Paint_DrawString_EN Input exceeds the normal display range\r\n");
		return;
	}

	if (inverted == DRAW_IMAGE_INVERTED) color = WHITE;

	ptr = (uint8_t *) img->data;
	for (int j = 0; j < img->header.h; j++) {
		for (int i = 0; i < img->header.w; i++) {//8 pixel =  1 byte
			if (i % 8 == 0) {
				tmp = *ptr;
				ptr++;
			}

			if (tmp & 0x80)
				Paint_DrawPoint(x+i, y+j, color, 1, DOT_STYLE_DFT);

			tmp = tmp <<1;
		}
	}
}

///******************************************************************************
//function:	SDisplay half of monochrome bitmap
//parameter:
//	Region : 1 Upper half
//					 2 Lower half
//info:
//******************************************************************************/
//void Paint_DrawBitMap_Half(const unsigned char* image_buffer, UBYTE Region)
//{
//    UWORD x, y;
//    UDOUBLE Addr = 0;
//		
//		if(Region == 1){
//			for (y = 0; y < Paint.HeightByte; y++) {
//					for (x = 0; x < Paint.WidthByte; x++) {//8 pixel =  1 byte
//							Addr = x + y * Paint.WidthByte;
//							Paint.Image[Addr] = (unsigned char)image_buffer[Addr];
//					}
//			}
//		}else{
//			for (y = 0; y < Paint.HeightByte; y++) {
//					for (x = 0; x < Paint.WidthByte; x++) {//8 pixel =  1 byte
//							Addr = x + y * Paint.WidthByte ;
//							Paint.Image[Addr] =
//							(unsigned char)image_buffer[Addr+ (Paint.HeightByte)*Paint.WidthByte];
//					}
//			}
//		}
//}

///******************************************************************************
//function:	SDisplay half of monochrome bitmap
//parameter:
//	Region : 1 Upper half
//					 2 Lower half
//info:
//******************************************************************************/
//void Paint_DrawBitMap_OneQuarter(const unsigned char* image_buffer, UBYTE Region)
//{
//    UWORD x, y;
//    UDOUBLE Addr = 0;
//		
//		if(Region == 1){
//			for (y = 0; y < Paint.HeightByte; y++) {
//					for (x = 0; x < Paint.WidthByte; x++) {//8 pixel =  1 byte
//							Addr = x + y * Paint.WidthByte;
//							Paint.Image[Addr] = (unsigned char)image_buffer[Addr];
//					}
//			}
//		}else if(Region == 2){
//			for (y = 0; y < Paint.HeightByte; y++) {
//					for (x = 0; x < Paint.WidthByte; x++) {//8 pixel =  1 byte
//							Addr = x + y * Paint.WidthByte ;
//							Paint.Image[Addr] =
//							(unsigned char)image_buffer[Addr+ (Paint.HeightByte)*Paint.WidthByte];
//					}
//			}
//		}else if(Region == 3){
//			for (y = 0; y < Paint.HeightByte; y++) {
//					for (x = 0; x < Paint.WidthByte; x++) {//8 pixel =  1 byte
//							Addr = x + y * Paint.WidthByte ;
//							Paint.Image[Addr] =
//							(unsigned char)image_buffer[Addr+ (Paint.HeightByte)*Paint.WidthByte*2];
//					}
//			}
//		}else if(Region == 4){
//			for (y = 0; y < Paint.HeightByte; y++) {
//					for (x = 0; x < Paint.WidthByte; x++) {//8 pixel =  1 byte
//							Addr = x + y * Paint.WidthByte ;
//							Paint.Image[Addr] =
//							(unsigned char)image_buffer[Addr+ (Paint.HeightByte)*Paint.WidthByte*3];
//					}
//			}
//		}
//}

void Paint_DrawBitMap_Block(const uint8_t* image_buffer, uint8_t Region)
{
	uint16_t x, y;
	uint32_t Addr = 0;
	for (y = 0; y < Paint.HeightByte; y++) {
		for (x = 0; x < Paint.WidthByte; x++) {//8 pixel =  1 byte
			Addr = x + y * Paint.WidthByte ;
			Paint.Image[Addr] = \
					(unsigned char)image_buffer[Addr+ (Paint.HeightByte)*Paint.WidthByte*(Region - 1)];
		}
	}
}

void Paint_SendToDisplay()
{
	if (Paint.sendToDisplayFct != NULL) {
		Paint.sendToDisplayFct(Paint.Image, Paint.HeightByte*Paint.WidthByte);
	}

	Paint.sendToDisplayFct(Paint.Image, Paint.HeightByte*Paint.WidthByte);

}

void Paint_SetDrawingWindow(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend)
{
	if (Paint.setDrawingWindowFct != NULL) {
		Paint.setDrawingWindowFct(Xstart, Ystart, Xend, Yend);
	}
}
