/**
 * File              : colors.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 14.05.2023
 * Last Modified Date: 03.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef COLORS_H
#define COLORS_H

#define _COLOR_STR(x, y) "</" #x ">" y "<!" #x ">"
#define COLOR_STR(x, y) _COLOR_STR(x, y)

#define _COLOR_N(x) "</" #x ">"
#define COLOR_N(x) _COLOR_N(x)

#define COLOR_WHITE_ON_WHITE      1
#define COLOR_WHITE_ON_RED        2
#define COLOR_WHITE_ON_GREEN      3
#define COLOR_WHITE_ON_YELLOW     4
#define COLOR_WHITE_ON_BLUE       5
#define COLOR_WHITE_ON_MAGENTA    6
#define COLOR_WHITE_ON_CYAN       7
#define COLOR_WHITE_ON_BLACK      8

#define COLOR_RED_ON_WHITE        9
#define COLOR_RED_ON_RED          10
#define COLOR_RED_ON_GREEN        11
#define COLOR_RED_ON_YELLOW       12
#define COLOR_RED_ON_BLUE         13
#define COLOR_RED_ON_MAGENTA      14
#define COLOR_RED_ON_CYAN         15
#define COLOR_RED_ON_BLACK        16

#define COLOR_GREEN_ON_WHITE      17
#define COLOR_GREEN_ON_RED        18
#define COLOR_GREEN_ON_GREEN      19
#define COLOR_GREEN_ON_YELLOW     20
#define COLOR_GREEN_ON_BLUE       21
#define COLOR_GREEN_ON_MAGENTA    22
#define COLOR_GREEN_ON_CYAN       23
#define COLOR_GREEN_ON_BLACK      24

#define COLOR_YELLOW_ON_WHITE     25
#define COLOR_YELLOW_ON_RED       26
#define COLOR_YELLOW_ON_GREEN     27
#define COLOR_YELLOW_ON_YELLOW    28
#define COLOR_YELLOW_ON_BLUE      29
#define COLOR_YELLOW_ON_MAGENTA   30
#define COLOR_YELLOW_ON_CYAN      31
#define COLOR_YELLOW_ON_BLACK     32

#define COLOR_BLUE_ON_WHITE       33
#define COLOR_BLUE_ON_RED         34
#define COLOR_BLUE_ON_GREEN       35
#define COLOR_BLUE_ON_YELLOW      36
#define COLOR_BLUE_ON_BLUE        37
#define COLOR_BLUE_ON_MAGENTA     38
#define COLOR_BLUE_ON_CYAN        39
#define COLOR_BLUE_ON_BLACK       40

#define COLOR_MAGENTA_ON_WHITE    41
#define COLOR_MAGENTA_ON_RED      42
#define COLOR_MAGENTA_ON_GREEN    43
#define COLOR_MAGENTA_ON_YELLOW   44
#define COLOR_MAGENTA_ON_BLUE     45
#define COLOR_MAGENTA_ON_MAGENTA  46
#define COLOR_MAGENTA_ON_CYAN     47
#define COLOR_MAGENTA_ON_BLACK    48

#define COLOR_CYAN_ON_WHITE       49
#define COLOR_CYAN_ON_RED         50
#define COLOR_CYAN_ON_GREEN       51
#define COLOR_CYAN_ON_YELLOW      52
#define COLOR_CYAN_ON_BLUE        53
#define COLOR_CYAN_ON_MAGENTA     54
#define COLOR_CYAN_ON_CYAN        55
#define COLOR_CYAN_ON_BLACK       56
	
#define COLOR_BLACK_ON_WHITE      57
#define COLOR_BLACK_ON_RED        58
#define COLOR_BLACK_ON_GREEN      59
#define COLOR_BLACK_ON_YELLOW     60
#define COLOR_BLACK_ON_BLUE       61
#define COLOR_BLACK_ON_MAGENTA    62
#define COLOR_BLACK_ON_CYAN       63
#define COLOR_BLACK_ON_BLACK      64

#endif /* ifndef COLORS_H */			
