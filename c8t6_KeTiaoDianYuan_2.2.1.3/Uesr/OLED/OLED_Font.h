#ifndef __OLED_FONT_H
#define __OLED_FONT_H

#include "stm32f10x.h"

/*OLED字模库，宽8像素，高16像素*/
//在PCtoLCD2002中的取模方式为：列行式、逆向
const uint8_t OLED_F8x16[][16]=
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//  0
	
	0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x33,0x30,0x00,0x00,0x00,//! 1
	
	0x00,0x10,0x0C,0x06,0x10,0x0C,0x06,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//" 2
	
	0x40,0xC0,0x78,0x40,0xC0,0x78,0x40,0x00,
	0x04,0x3F,0x04,0x04,0x3F,0x04,0x04,0x00,//# 3
	
	0x00,0x70,0x88,0xFC,0x08,0x30,0x00,0x00,
	0x00,0x18,0x20,0xFF,0x21,0x1E,0x00,0x00,//$ 4
	
	0xF0,0x08,0xF0,0x00,0xE0,0x18,0x00,0x00,
	0x00,0x21,0x1C,0x03,0x1E,0x21,0x1E,0x00,//% 5
	
	0x00,0xF0,0x08,0x88,0x70,0x00,0x00,0x00,
	0x1E,0x21,0x23,0x24,0x19,0x27,0x21,0x10,//& 6
	
	0x10,0x16,0x0E,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//' 7
	
	0x00,0x00,0x00,0xE0,0x18,0x04,0x02,0x00,
	0x00,0x00,0x00,0x07,0x18,0x20,0x40,0x00,//( 8
	
	0x00,0x02,0x04,0x18,0xE0,0x00,0x00,0x00,
	0x00,0x40,0x20,0x18,0x07,0x00,0x00,0x00,//) 9
	
	0x40,0x40,0x80,0xF0,0x80,0x40,0x40,0x00,
	0x02,0x02,0x01,0x0F,0x01,0x02,0x02,0x00,//* 10
	
	0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,
	0x01,0x01,0x01,0x1F,0x01,0x01,0x01,0x00,//+ 11
	
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x80,0xB0,0x70,0x00,0x00,0x00,0x00,0x00,//, 12
	
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,//- 13
	
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00,//. 14
	
	0x00,0x00,0x00,0x00,0x80,0x60,0x18,0x04,
	0x00,0x60,0x18,0x06,0x01,0x00,0x00,0x00,/// 15
	
	0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,
	0x00,0x0F,0x10,0x20,0x20,0x10,0x0F,0x00,//0 16
	
	0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,
	0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//1 17
	
	0x00,0x70,0x08,0x08,0x08,0x88,0x70,0x00,
	0x00,0x30,0x28,0x24,0x22,0x21,0x30,0x00,//2 18
	
	0x00,0x30,0x08,0x88,0x88,0x48,0x30,0x00,
	0x00,0x18,0x20,0x20,0x20,0x11,0x0E,0x00,//3 19
	
	0x00,0x00,0xC0,0x20,0x10,0xF8,0x00,0x00,
	0x00,0x07,0x04,0x24,0x24,0x3F,0x24,0x00,//4 20
	
	0x00,0xF8,0x08,0x88,0x88,0x08,0x08,0x00,
	0x00,0x19,0x21,0x20,0x20,0x11,0x0E,0x00,//5 21
	
	0x00,0xE0,0x10,0x88,0x88,0x18,0x00,0x00,
	0x00,0x0F,0x11,0x20,0x20,0x11,0x0E,0x00,//6 22
	
	0x00,0x38,0x08,0x08,0xC8,0x38,0x08,0x00,
	0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,//7 23
	
	0x00,0x70,0x88,0x08,0x08,0x88,0x70,0x00,
	0x00,0x1C,0x22,0x21,0x21,0x22,0x1C,0x00,//8 24
	
	0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,
	0x00,0x00,0x31,0x22,0x22,0x11,0x0F,0x00,//9 25
	
	0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,
	0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,//: 26
	
	0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,
	0x00,0x00,0x80,0x60,0x00,0x00,0x00,0x00,//; 27
	
	0x00,0x00,0x80,0x40,0x20,0x10,0x08,0x00,
	0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x00,//< 28
	
	0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,
	0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00,//= 29
	
	0x00,0x08,0x10,0x20,0x40,0x80,0x00,0x00,
	0x00,0x20,0x10,0x08,0x04,0x02,0x01,0x00,//> 30
	
	0x00,0x70,0x48,0x08,0x08,0x08,0xF0,0x00,
	0x00,0x00,0x00,0x30,0x36,0x01,0x00,0x00,//? 31
	
	0xC0,0x30,0xC8,0x28,0xE8,0x10,0xE0,0x00,
	0x07,0x18,0x27,0x24,0x23,0x14,0x0B,0x00,//@ 32
	
	0x00,0x00,0xC0,0x38,0xE0,0x00,0x00,0x00,
	0x20,0x3C,0x23,0x02,0x02,0x27,0x38,0x20,//A 33
	
	0x08,0xF8,0x88,0x88,0x88,0x70,0x00,0x00,
	0x20,0x3F,0x20,0x20,0x20,0x11,0x0E,0x00,//B 34
	
	0xC0,0x30,0x08,0x08,0x08,0x08,0x38,0x00,
	0x07,0x18,0x20,0x20,0x20,0x10,0x08,0x00,//C 35
	
	0x08,0xF8,0x08,0x08,0x08,0x10,0xE0,0x00,
	0x20,0x3F,0x20,0x20,0x20,0x10,0x0F,0x00,//D 36
	
	0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,
	0x20,0x3F,0x20,0x20,0x23,0x20,0x18,0x00,//E 37
	
	0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,
	0x20,0x3F,0x20,0x00,0x03,0x00,0x00,0x00,//F 38
	
	0xC0,0x30,0x08,0x08,0x08,0x38,0x00,0x00,
	0x07,0x18,0x20,0x20,0x22,0x1E,0x02,0x00,//G 39
	
	0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,
	0x20,0x3F,0x21,0x01,0x01,0x21,0x3F,0x20,//H 40
	
	0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0x00,
	0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//I 41
	
	0x00,0x00,0x08,0x08,0xF8,0x08,0x08,0x00,
	0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,0x00,//J 42
	
	0x08,0xF8,0x88,0xC0,0x28,0x18,0x08,0x00,
	0x20,0x3F,0x20,0x01,0x26,0x38,0x20,0x00,//K 43
	
	0x08,0xF8,0x08,0x00,0x00,0x00,0x00,0x00,
	0x20,0x3F,0x20,0x20,0x20,0x20,0x30,0x00,//L 44
	
	0x08,0xF8,0xF8,0x00,0xF8,0xF8,0x08,0x00,
	0x20,0x3F,0x00,0x3F,0x00,0x3F,0x20,0x00,//M 45
	
	0x08,0xF8,0x30,0xC0,0x00,0x08,0xF8,0x08,
	0x20,0x3F,0x20,0x00,0x07,0x18,0x3F,0x00,//N 46
	
	0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,
	0x0F,0x10,0x20,0x20,0x20,0x10,0x0F,0x00,//O 47
	
	0x08,0xF8,0x08,0x08,0x08,0x08,0xF0,0x00,
	0x20,0x3F,0x21,0x01,0x01,0x01,0x00,0x00,//P 48
	
	0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,
	0x0F,0x18,0x24,0x24,0x38,0x50,0x4F,0x00,//Q 49
	
	0x08,0xF8,0x88,0x88,0x88,0x88,0x70,0x00,
	0x20,0x3F,0x20,0x00,0x03,0x0C,0x30,0x20,//R 50
	
	0x00,0x70,0x88,0x08,0x08,0x08,0x38,0x00,
	0x00,0x38,0x20,0x21,0x21,0x22,0x1C,0x00,//S 51
	
	0x18,0x08,0x08,0xF8,0x08,0x08,0x18,0x00,
	0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00,//T 52
	
	0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,
	0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,//U 53
	
	0x08,0x78,0x88,0x00,0x00,0xC8,0x38,0x08,
	0x00,0x00,0x07,0x38,0x0E,0x01,0x00,0x00,//V 54
	
	0xF8,0x08,0x00,0xF8,0x00,0x08,0xF8,0x00,
	0x03,0x3C,0x07,0x00,0x07,0x3C,0x03,0x00,//W 55
	
	0x08,0x18,0x68,0x80,0x80,0x68,0x18,0x08,
	0x20,0x30,0x2C,0x03,0x03,0x2C,0x30,0x20,//X 56
	
	0x08,0x38,0xC8,0x00,0xC8,0x38,0x08,0x00,
	0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00,//Y 57
	
	0x10,0x08,0x08,0x08,0xC8,0x38,0x08,0x00,
	0x20,0x38,0x26,0x21,0x20,0x20,0x18,0x00,//Z 58
	
	0x00,0x00,0x00,0xFE,0x02,0x02,0x02,0x00,
	0x00,0x00,0x00,0x7F,0x40,0x40,0x40,0x00,//[ 59
	
	0x00,0x0C,0x30,0xC0,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x01,0x06,0x38,0xC0,0x00,//\ 60
	
	0x00,0x02,0x02,0x02,0xFE,0x00,0x00,0x00,
	0x00,0x40,0x40,0x40,0x7F,0x00,0x00,0x00,//] 61
	
	0x00,0x00,0x04,0x02,0x02,0x02,0x04,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//^ 62
	
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,//_ 63
	
	0x00,0x02,0x02,0x04,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//` 64
	
	0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,
	0x00,0x19,0x24,0x22,0x22,0x22,0x3F,0x20,//a 65
	
	0x08,0xF8,0x00,0x80,0x80,0x00,0x00,0x00,
	0x00,0x3F,0x11,0x20,0x20,0x11,0x0E,0x00,//b 66
	
	0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,
	0x00,0x0E,0x11,0x20,0x20,0x20,0x11,0x00,//c 67
	
	0x00,0x00,0x00,0x80,0x80,0x88,0xF8,0x00,
	0x00,0x0E,0x11,0x20,0x20,0x10,0x3F,0x20,//d 68
	
	0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,
	0x00,0x1F,0x22,0x22,0x22,0x22,0x13,0x00,//e 69
	
	0x00,0x80,0x80,0xF0,0x88,0x88,0x88,0x18,
	0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//f 70
	
	0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,
	0x00,0x6B,0x94,0x94,0x94,0x93,0x60,0x00,//g 71
	
	0x08,0xF8,0x00,0x80,0x80,0x80,0x00,0x00,
	0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20,//h 72
	
	0x00,0x80,0x98,0x98,0x00,0x00,0x00,0x00,
	0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//i 73
	
	0x00,0x00,0x00,0x80,0x98,0x98,0x00,0x00,
	0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,//j 74
	
	0x08,0xF8,0x00,0x00,0x80,0x80,0x80,0x00,
	0x20,0x3F,0x24,0x02,0x2D,0x30,0x20,0x00,//k 75
	
	0x00,0x08,0x08,0xF8,0x00,0x00,0x00,0x00,
	0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//l 76
	
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,
	0x20,0x3F,0x20,0x00,0x3F,0x20,0x00,0x3F,//m 77
	
	0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,
	0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20,//n 78
	
	0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,
	0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,//o 79
	
	0x80,0x80,0x00,0x80,0x80,0x00,0x00,0x00,
	0x80,0xFF,0xA1,0x20,0x20,0x11,0x0E,0x00,//p 80
	
	0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,
	0x00,0x0E,0x11,0x20,0x20,0xA0,0xFF,0x80,//q 81
	
	0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,
	0x20,0x20,0x3F,0x21,0x20,0x00,0x01,0x00,//r 82
	
	0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,
	0x00,0x33,0x24,0x24,0x24,0x24,0x19,0x00,//s 83
	
	0x00,0x80,0x80,0xE0,0x80,0x80,0x00,0x00,
	0x00,0x00,0x00,0x1F,0x20,0x20,0x00,0x00,//t 84
	
	0x80,0x80,0x00,0x00,0x00,0x80,0x80,0x00,
	0x00,0x1F,0x20,0x20,0x20,0x10,0x3F,0x20,//u 85
	
	0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,
	0x00,0x01,0x0E,0x30,0x08,0x06,0x01,0x00,//v 86
	
	0x80,0x80,0x00,0x80,0x00,0x80,0x80,0x80,
	0x0F,0x30,0x0C,0x03,0x0C,0x30,0x0F,0x00,//w 87
	
	0x00,0x80,0x80,0x00,0x80,0x80,0x80,0x00,
	0x00,0x20,0x31,0x2E,0x0E,0x31,0x20,0x00,//x 88
	
	0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,
	0x80,0x81,0x8E,0x70,0x18,0x06,0x01,0x00,//y 89
	
	0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,
	0x00,0x21,0x30,0x2C,0x22,0x21,0x30,0x00,//z 90
	
	0x00,0x00,0x00,0x00,0x80,0x7C,0x02,0x02,
	0x00,0x00,0x00,0x00,0x00,0x3F,0x40,0x40,//{ 91
	
	0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,//| 92
	
	0x00,0x02,0x02,0x7C,0x80,0x00,0x00,0x00,
	0x00,0x40,0x40,0x3F,0x00,0x00,0x00,0x00,//} 93
	
	0x00,0x06,0x01,0x01,0x02,0x02,0x04,0x04,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//~ 94
};


//0~9的阳码
const uint8_t OLED_F8x16_Bright[][16]=
{
	0xFF,0x1F,0xEF,0xF7,0xF7,0xEF,0x1F,0xFF,
	0xFF,0xF0,0xEF,0xDF,0xDF,0xEF,0xF0,0xFF,  /* 0 */

	0xFF,0xFF,0xEF,0xEF,0x07,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xDF,0xDF,0xC0,0xDF,0xDF,0xFF,  /* 1 */

	0xFF,0x8F,0xF7,0xF7,0xF7,0xF7,0x0F,0xFF,
	0xFF,0xCF,0xD7,0xDB,0xDD,0xDE,0xCF,0xFF,  /* 2 */

	0xFF,0xCF,0xF7,0xF7,0xF7,0x77,0x8F,0xFF,
	0xFF,0xE7,0xDF,0xDE,0xDE,0xDD,0xE3,0xFF,  /* 3 */

	0xFF,0xFF,0x7F,0xBF,0xCF,0x07,0xFF,0xFF,
	0xFF,0xF9,0xFA,0xDB,0xDB,0xC0,0xDB,0xDB,  /* 4 */

	0xFF,0x07,0x77,0x77,0x77,0xF7,0xF7,0xFF,
	0xFF,0xE6,0xDF,0xDF,0xDF,0xEE,0xF1,0xFF,  /* 5 */

	0xFF,0x1F,0xEF,0x77,0x77,0x6F,0xFF,0xFF,
	0xFF,0xF0,0xEE,0xDF,0xDF,0xDF,0xE0,0xFF,  /* 6 */

	0xFF,0xE7,0xF7,0xF7,0x77,0x97,0xE7,0xFF,
	0xFF,0xFF,0xFF,0xC1,0xFE,0xFF,0xFF,0xFF,  /* 7 */

	0xFF,0x8F,0x77,0xF7,0xF7,0x77,0x8F,0xFF,
	0xFF,0xE3,0xDD,0xDE,0xDE,0xDD,0xE3,0xFF,  /* 8 */

	0xFF,0x0F,0xF7,0xF7,0xF7,0xEF,0x1F,0xFF,
	0xFF,0xFE,0xED,0xDD,0xDD,0xEE,0xF0,0xFF,  /* 9 */
};


//汉字
const uint8_t OLED_F16x16[][32]=
{
	0x00,0xFE,0x20,0x20,0x3F,0x20,0x00,0xFC,0x24,0xE4,0x24,0x22,0x23,0xE2,0x00,0x00,
	0x80,0x7F,0x01,0x01,0xFF,0x80,0x60,0x1F,0x80,0x41,0x26,0x18,0x26,0x41,0x80,0x00,  /*"版",0*/

	0x00,0x10,0x10,0x10,0x10,0xD0,0x30,0xFF,0x30,0xD0,0x10,0x10,0x10,0x10,0x00,0x00,
	0x10,0x08,0x04,0x02,0x09,0x08,0x08,0xFF,0x08,0x08,0x09,0x02,0x04,0x08,0x10,0x00,  /*"本",1*/
	
	0x80,0x80,0x80,0xBE,0xA2,0xA2,0xA2,0xA2,0xA2,0xA2,0xA2,0xBE,0x80,0x80,0x80,0x00,
	0x00,0x00,0x00,0x06,0x05,0x04,0x04,0x04,0x44,0x84,0x44,0x3C,0x00,0x00,0x00,0x00,  /*"号",2*/

	0x08,0x08,0x08,0xF8,0x08,0x08,0x08,0x10,0x10,0xFF,0x10,0x10,0x10,0xF0,0x00,0x00,
	0x10,0x30,0x10,0x1F,0x08,0x88,0x48,0x30,0x0E,0x01,0x40,0x80,0x40,0x3F,0x00,0x00,  /*"功",3*/

	0x00,0x14,0xA4,0x44,0x24,0x34,0xAD,0x66,0x24,0x94,0x04,0x44,0xA4,0x14,0x00,0x00,
	0x08,0x09,0x08,0x08,0x09,0x09,0x09,0xFD,0x09,0x09,0x0B,0x08,0x08,0x09,0x08,0x00,  /*"率",4*/

	0x00,0x00,0xF8,0x88,0x88,0x88,0x88,0xFF,0x88,0x88,0x88,0x88,0xF8,0x00,0x00,0x00,
	0x00,0x00,0x1F,0x08,0x08,0x08,0x08,0x7F,0x88,0x88,0x88,0x88,0x9F,0x80,0xF0,0x00,  /*"电",5*/

	0x10,0x60,0x02,0x8C,0x00,0x44,0x64,0x54,0x4D,0x46,0x44,0x54,0x64,0xC4,0x04,0x00,
	0x04,0x04,0x7E,0x01,0x80,0x40,0x3E,0x00,0x00,0xFE,0x00,0x00,0x7E,0x80,0xE0,0x00,  /*"流",6*/

	0x00,0x00,0xF8,0x88,0x88,0x88,0x88,0xFF,0x88,0x88,0x88,0x88,0xF8,0x00,0x00,0x00,
	0x00,0x00,0x1F,0x08,0x08,0x08,0x08,0x7F,0x88,0x88,0x88,0x88,0x9F,0x80,0xF0,0x00,  /*"电",7*/

	0x00,0x00,0xFE,0x02,0x82,0x82,0x82,0x82,0xFA,0x82,0x82,0x82,0x82,0x82,0x02,0x00,
	0x80,0x60,0x1F,0x40,0x40,0x40,0x40,0x40,0x7F,0x40,0x40,0x44,0x58,0x40,0x40,0x00,  /*"压",8*/

	0x90,0x88,0x87,0xFC,0x84,0x84,0x80,0x02,0xF2,0x12,0x12,0x12,0x12,0xF2,0x02,0x00,
	0x80,0x60,0x18,0x07,0x08,0x30,0x00,0x40,0x45,0x59,0x41,0x41,0x51,0x4D,0x40,0x00,  /*"短",9*/

	0x00,0x3E,0x22,0xE2,0x22,0x3E,0x00,0x10,0x88,0x57,0x24,0x54,0x8C,0x00,0x00,0x00,
	0x40,0x7E,0x40,0x3F,0x22,0x22,0x00,0x01,0xFE,0x42,0x42,0x42,0xFE,0x01,0x01,0x00,  /*"路",10*/

	0x40,0x42,0xCC,0x00,0x00,0x44,0x54,0x54,0x54,0x7F,0x54,0x54,0x54,0x44,0x40,0x00,
	0x00,0x00,0x7F,0x20,0x10,0x00,0xFF,0x15,0x15,0x15,0x55,0x95,0x7F,0x00,0x00,0x00,  /*"请",11*/

	0x10,0x10,0x14,0xD4,0x54,0x54,0x54,0xFC,0x52,0x52,0x52,0xD3,0x12,0x10,0x10,0x00,
	0x40,0x40,0x50,0x57,0x55,0x55,0x55,0x7F,0x55,0x55,0x55,0x57,0x50,0x40,0x40,0x00,  /*"重",12*/

	0x00,0x00,0x00,0xFC,0x44,0x44,0x44,0x45,0x46,0x44,0x44,0x44,0x44,0x7C,0x00,0x00,
	0x40,0x20,0x18,0x07,0x00,0xFC,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0xFC,0x00,0x00,  /*"启",13*/

	0x00,0x08,0x28,0x48,0x88,0x08,0x08,0xFF,0x88,0x08,0x89,0x4A,0x28,0x08,0x00,0x00,
	0x00,0x10,0x10,0x08,0x04,0x42,0x81,0x7F,0x00,0x01,0x02,0x04,0x08,0x10,0x10,0x00,  /*"求",14*/

	0x00,0x00,0xFC,0x04,0x44,0x84,0x04,0x25,0xC6,0x04,0x04,0x04,0x04,0xE4,0x04,0x00,
	0x40,0x30,0x0F,0x40,0x40,0x41,0x4E,0x40,0x40,0x63,0x50,0x4C,0x43,0x40,0x40,0x00,  /*"应",15*/

	0x00,0xFE,0x22,0x5A,0x86,0x00,0xFE,0x92,0x92,0x92,0x92,0x92,0xFE,0x00,0x00,0x00,
	0x00,0xFF,0x04,0x08,0x07,0x00,0xFF,0x40,0x20,0x03,0x0C,0x14,0x22,0x41,0x40,0x00,  /*"限",16*/

	0x40,0x40,0x42,0xCC,0x00,0x40,0xA0,0x9E,0x82,0x82,0x82,0x9E,0xA0,0x20,0x20,0x00,
	0x00,0x00,0x00,0x3F,0x90,0x88,0x40,0x43,0x2C,0x10,0x28,0x46,0x41,0x80,0x80,0x00,  /*"设",17*/

	0x10,0x0C,0x44,0x44,0x44,0x44,0x45,0xC6,0x44,0x44,0x44,0x44,0x44,0x14,0x0C,0x00,
	0x80,0x40,0x20,0x1E,0x20,0x40,0x40,0x7F,0x44,0x44,0x44,0x44,0x44,0x40,0x40,0x00,  /*"定",18*/

	0x80,0x82,0x82,0x82,0xFE,0x82,0x82,0x82,0x82,0x82,0xFE,0x82,0x82,0x82,0x80,0x00,
	0x00,0x80,0x40,0x30,0x0F,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,  /*"开",19*/

	0x00,0x00,0x10,0x11,0x16,0x10,0x10,0xF0,0x10,0x10,0x14,0x13,0x10,0x00,0x00,0x00,
	0x81,0x81,0x41,0x41,0x21,0x11,0x0D,0x03,0x0D,0x11,0x21,0x41,0x41,0x81,0x81,0x00,  /*"关",20*/

	0x00,0xF8,0x01,0x22,0x20,0x22,0x22,0xA2,0xFA,0x22,0x22,0x22,0x02,0xFE,0x00,0x00,
	0x00,0xFF,0x00,0x08,0x04,0x02,0x11,0x20,0x1F,0x00,0x00,0x40,0x80,0x7F,0x00,0x00,  /*"闭",21*/

	0x00,0xFC,0x20,0x24,0xA8,0xFF,0xA8,0x24,0x00,0xFC,0x44,0x44,0xC4,0x42,0x40,0x00,
	0x00,0x3F,0x22,0x21,0x20,0x2F,0x20,0xA3,0x60,0x1F,0x00,0x00,0xFF,0x00,0x00,0x00,  /*"断",22*/

	0x80,0x80,0x88,0x88,0x88,0x88,0x88,0xFF,0x88,0x88,0x88,0x88,0x88,0x80,0x80,0x00,
	0x20,0x20,0x10,0x08,0x04,0x02,0x01,0xFF,0x01,0x02,0x04,0x08,0x10,0x20,0x20,0x00,  /*"未",23*/

	0x20,0x18,0x0F,0x08,0xF8,0x08,0x08,0x00,0x00,0xFC,0x04,0x04,0x04,0xFC,0x00,0x00,
	0x81,0x41,0x31,0x0D,0x03,0x0D,0x71,0x01,0x00,0x3F,0x10,0x10,0x10,0x3F,0x00,0x00,  /*"知",24*/

	0x04,0x84,0xE4,0x5C,0x44,0xC4,0x20,0x10,0xE8,0x27,0x24,0xE4,0x34,0x2C,0xE0,0x00,
	0x02,0x01,0x7F,0x10,0x10,0x3F,0x80,0x60,0x1F,0x09,0x09,0x3F,0x49,0x89,0x7F,0x00,  /*"确",25*/

	0x10,0x0C,0x44,0x44,0x44,0x44,0x45,0xC6,0x44,0x44,0x44,0x44,0x44,0x14,0x0C,0x00,
	0x80,0x40,0x20,0x1E,0x20,0x40,0x40,0x7F,0x44,0x44,0x44,0x44,0x44,0x40,0x40,0x00,  /*"定",26*/

	0x40,0x50,0x4E,0x48,0x48,0xFF,0x48,0x48,0x48,0x40,0xF8,0x00,0x00,0xFF,0x00,0x00,
	0x00,0x00,0x3E,0x02,0x02,0xFF,0x12,0x22,0x1E,0x00,0x0F,0x40,0x80,0x7F,0x00,0x00,  /*"制",27*/

	0x40,0x44,0xC4,0x44,0x44,0x44,0x40,0x10,0x10,0xFF,0x10,0x10,0x10,0xF0,0x00,0x00,
	0x10,0x3C,0x13,0x10,0x14,0xB8,0x40,0x30,0x0E,0x01,0x40,0x80,0x40,0x3F,0x00,0x00,  /*"动",28*/

	0x08,0x04,0x23,0x22,0x26,0x2A,0x22,0xFA,0x24,0x23,0x22,0x26,0x2A,0x02,0x02,0x00,
	0x01,0x09,0x09,0x09,0x19,0x69,0x09,0x09,0x49,0x89,0x7D,0x09,0x09,0x09,0x01,0x00,  /*"等",29*/

	0x00,0x10,0x88,0xC4,0x33,0x40,0x48,0x48,0x48,0x7F,0x48,0xC8,0x48,0x48,0x40,0x00,
	0x02,0x01,0x00,0xFF,0x00,0x02,0x0A,0x32,0x02,0x42,0x82,0x7F,0x02,0x02,0x02,0x00,  /*"待",30*/

	0x00,0x17,0x15,0xD5,0x55,0x57,0x55,0x7D,0x55,0x57,0x55,0xD5,0x15,0x17,0x00,0x00,
	0x40,0x40,0x40,0x7F,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x7F,0x40,0x40,0x40,0x00,  /*"置",31*/

	0x40,0x40,0x42,0xCC,0x00,0x50,0x4E,0xC8,0x48,0x7F,0xC8,0x48,0x48,0x40,0x00,0x00,
	0x00,0x40,0x20,0x1F,0x20,0x50,0x4C,0x43,0x40,0x40,0x4F,0x50,0x50,0x5C,0x40,0x00,  /*"选",32*/

	0x08,0x08,0x08,0xF8,0x08,0x08,0x00,0xF2,0x12,0x1A,0xD6,0x12,0x12,0xF2,0x02,0x00,
	0x10,0x30,0x10,0x0F,0x08,0x08,0x80,0x4F,0x20,0x18,0x07,0x10,0x20,0x4F,0x80,0x00,  /*"项",33*/
};


//图片
const uint8_t OLED_BMP_64x64[][512]=
{
	//校徽
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0xE0,0xE0,0xE0,0xF0,0xF0,0xF8,0xF8,0xF8,0xF8,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0xFC,0x7C,0x3C,0x3C,0x3C,0x3C,0x3C,0x3C,0x78,0xF8,0xF8,0xF0,0xF0,0xF0,0xE0,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xE0,0xF8,0xFC,0xFE,0xFE,0xFF,0x7F,0x1F,0x0F,0x0F,0x07,0x07,0xC7,0xE7,0x7F,0x3F,0x1F,0x0F,0x07,0x03,0x81,0x81,0xC1,0xC0,0x60,0x30,0x38,0x18,0x98,0x8C,0x8C,0x8E,0x86,0x86,0x86,0x87,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x0F,0x0E,0x1C,0x78,0xF0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0xE0,0xF8,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,0x07,0x01,0x00,0x00,0x00,0xE0,0xF0,0x7C,0x0F,0x07,0xC1,0xE0,0xE0,0xF8,0xFC,0x7C,0x3F,0x3B,0x3D,0x3C,0x3E,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x3C,0xF8,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0xE1,0xFF,0xFF,0xFE,0xFC,0xF0,0x80,0x00,0x00,
	0x00,0xE0,0xFF,0xFF,0xFF,0xFF,0x1F,0x0F,0xFF,0xFF,0x00,0x00,0x00,0x00,0xE0,0xFF,0x0F,0xC0,0xF0,0xFC,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x1F,0x3F,0x7F,0x7F,0xFF,0x7F,0x7F,0x7F,0x3F,0x1F,0x0F,0xC3,0xF8,0x7F,0x9F,0xC0,0xE0,0xF0,0xF8,0xFC,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xBF,0xFF,0xF8,0x00,
	0x00,0x0F,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x0F,0x3E,0xF0,0xC0,0xC0,0x07,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x81,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0xF0,0xFC,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,0x3F,0xDF,0xEF,0xF7,0xFF,0xFC,0xFF,0xFF,0xFF,0x3F,0x00,
	0x00,0x00,0x00,0x0F,0x3F,0xFC,0xF0,0xE0,0x80,0x00,0x00,0x00,0x01,0x03,0x07,0x0E,0x1C,0x19,0x3F,0x3F,0x7F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFE,0xFC,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xFC,0xFC,0xFC,0xFF,0x7F,0x7F,0x7F,0x3F,0x3F,0x9F,0xDF,0x4F,0x6F,0x27,0xF7,0xD9,0x6D,0x74,0xB6,0xFB,0xFD,0x7F,0xFF,0xFF,0xED,0x7F,0x1F,0x03,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x07,0x0F,0x3E,0x7C,0xF8,0xF0,0xE0,0xE0,0xC0,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x81,0x80,0xA0,0xA0,0xB0,0xD8,0xDA,0x48,0x6D,0x6D,0xB6,0xBE,0xFB,0xCF,0xED,0xEF,0xFE,0xFB,0x7F,0x7D,0x3F,0x0F,0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x03,0x07,0x0F,0x0F,0x1F,0x1F,0x1E,0x3E,0x3E,0x3E,0x3E,0x7C,0x7C,0x7C,0x7C,0x7C,0x7C,0x7C,0x7C,0x7D,0x7D,0x7C,0x7E,0x3E,0x3E,0x3F,0x3F,0x3F,0x1F,0x1F,0x0F,0x0F,0x07,0x03,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	//关机
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0xFC,0xFC,0xFC,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0xE0,0xF0,0xF8,0xF8,0xFC,0xFC,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFC,0xFC,0xF8,0xF8,0xF0,0xE0,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xFC,0xFE,0xFF,0xFF,0xFF,0x7F,0x1F,0x0F,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x0F,0x1F,0x7F,0xFF,0xFF,0xFF,0xFE,0xFC,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x0F,0x3F,0x7F,0xFF,0xFF,0xFF,0xFE,0xF8,0xF0,0xE0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0xC0,0xE0,0xF0,0xF8,0xFE,0xFF,0xFF,0xFF,0x7F,0x1F,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x0F,0x0F,0x1F,0x1F,0x3F,0x3F,0x7F,0x7F,0x7F,0x7E,0x7E,0x7E,0x7E,0x7E,0x7E,0x7E,0x7F,0x7F,0x7F,0x3F,0x3F,0x1F,0x1F,0x0F,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

const uint8_t OLED_BMP_8x16[][16]=
{
	0x00,0x00,0x00,0xE0,0x10,0x08,0x04,0x04,
	0x00,0x00,0x00,0x07,0x08,0x10,0x20,0x20,  /*"进度条左",0*/

	0x04,0x04,0x08,0x10,0xE0,0x00,0x00,0x00,
	0x20,0x20,0x10,0x08,0x07,0x00,0x00,0x00,  /*"进度条右",1*/

	0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
	0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,  /*"进度条空",2*/

	0x04,0xE4,0xE4,0xE4,0xE4,0xE4,0xE4,0x04,
	0x20,0x27,0x27,0x27,0x27,0x27,0x27,0x20,  /*"进度条实",3*/
};

#endif
