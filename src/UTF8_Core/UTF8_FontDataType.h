#ifndef _UTF8_FONT_DATA_TYPE_H
#define _UTF8_FONT_DATA_TYPE_H
//! @file ../src/UTF8_Core/UTF8_FontDataType.h
//! @brief Font DataType
//! @note font_filename="./misaki_ttf_2021-05-05/misaki_gothic.ttf",size=8
//! @note chara_list_filename="chara_list.txt"
//! @date 2023-09-12 01:33:01.587692

//! @typedef type_font_box_pos
//! @brief value type for font_box element (each X0,Y0,X1,Y1)
typedef uint8_t	type_font_box_pos;

//! @typedef type_font_num_entrylist
//! @brief value type for font_num_entrylist
typedef uint8_t	type_font_num_entrylist;

//! @typedef type_font_boxlist_id
//! @brief id type for font_boxlist
typedef uint8_t	type_font_boxlist_id;

//! @typedef type_font_glyphbit_pos
//! @brief bit pointer type for glyph_bitstream
typedef uint32_t	type_font_glyphbit_pos;

//! @typedef type_font_charatable_pos
//! @brief word pointer type for font_charatable
typedef uint16_t	type_font_charatable_pos;

#endif // _UTF8_FONT_DATA_TYPE_H

