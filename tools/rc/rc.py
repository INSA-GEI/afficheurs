#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Ressource compiler main program"""

my_letter = 'A'
font_name = '/usr/share/fonts/truetype/liberation2/LiberationMono-Regular.ttf'
#font_name = '/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf'
font_size = 9

Letters = "AÀal0"
#Letters = "Y"

if __name__ == "__main__":
    from converters import fonts

    cf = fonts.FontConverter()

    for letter in Letters:
        cf.convertChar(letter, font_name, font_size)
        