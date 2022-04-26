/**
  ******************************************************************************
  * @file    fonts.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-February-2014
  * @brief   Header for fonts.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FONTS_LIST_H
#define __FONTS_LIST_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "GUI_Paint.h"

/* LVGL Fonts */
//liberation_sans family
extern const lv_font_t liberation_sans_b_24;
extern const lv_font_t liberation_sans_b_36;
extern const lv_font_t liberation_sans_r_26;

// montserrat light italic family
extern const lv_font_t montserrat_li_150;
extern const lv_font_t montserrat_li_26;
extern const lv_font_t montserrat_li_34;
extern const lv_font_t montserrat_li_40;

// montserrat medium family
extern const lv_font_t montserrat_m_26;
extern const lv_font_t montserrat_m_34;
extern const lv_font_t montserrat_m_40;

// montserrat medium italic family
extern const lv_font_t montserrat_mi_150;

// montserrat regular family
extern const lv_font_t montserrat_r_26;
extern const lv_font_t montserrat_r_34;
extern const lv_font_t montserrat_r_40;

// orbitron medium family
extern const lv_font_t orbitron_m_60;
extern const lv_font_t orbitron_m_30;

// play regular family
extern const lv_font_t play_r_60;
extern const lv_font_t play_r_30;

// ubuntu regular italic family
extern const lv_font_t ubuntu_ri_150;

#ifdef __cplusplus
}
#endif
  
#endif /* __FONTS_LIST_H */
 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
