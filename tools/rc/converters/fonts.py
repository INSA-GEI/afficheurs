#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from PIL import Image, ImageFont, ImageDraw

class FontElement:
    index=0
    character=''
    size=()
    data=[]

    def __init__(self):
        index=0

    def print(self):
        print(self.index)

class FontList:
    name = ""
    height = 0
    length = 0

    def __init__(self):
        chars_list = []

class FontConverter:   

    def __init__(self):
        fonts_list = []

    def printImageData(self,img):
        #img.show("Charatere Show")
        for Y in range(0, img.size[1]):
            for X in range(0, img.size[0]):
                if img.getpixel((X, Y)) == 0:
                    print(' ', end='')
                else:
                    print('*', end='')
                #print (str(img.getpixel((X,Y)))+' ', end='')
            if Y == 0:
                print(' <-', end='')
            if Y == img.size[1]-1:
                print(' <-', end='')
            print('')

    def convertChar(self, character, font_name, font_size):
        pixels = int(round(font_size * 341 / 288)) # font_size
        font = ImageFont.truetype(font_name, pixels)
        character_size = font.getsize(character)
        
        print('Char size for letter ' + character + ' = ' + str(character_size))

        img = Image.new('1', character_size, color=0)

        dc = ImageDraw.Draw(img)
        dc.text((0, 0), character, 255, font)
        self.printImageData(img)
        img.close()

if __name__ == "__main__":
    print("Unitary tests to do here")
