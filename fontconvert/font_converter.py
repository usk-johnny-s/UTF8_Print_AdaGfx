from PIL import Image, ImageDraw, ImageFont
import sys
import math
import datetime
import copy

# Set chara_list filename
chara_list_filename = 'chara_list.txt'

# Set output filename
output_filename_data = '../src/UTF8_Core/UTF8_FontData.h'
output_filename_datatype = '../src/UTF8_Core/UTF8_FontDataType.h'

# Set Font filename and size
font_size = 8
font_filename = './misaki_ttf_2021-05-05/misaki_gothic.ttf'
#font_filename = './misaki_ttf_2021-05-05/misaki_gothic_2nd.ttf'
#font_filename = './misaki_ttf_2021-05-05/misaki_mincho.ttf'

byte_per_line = 16
word_per_line = 8
int_per_line = 8

def read_list_chara(chara_list_filename):
    # Read CharaList from utf-8 text file
    with open(chara_list_filename, 'r', encoding='utf-8') as f:
        in_lines = f.readlines()

    # Listing each unique character
    list_chara = []
    for in_line in in_lines:
        # Skipping line as a remarks, if the same character  continues twice at the beginning of a line
        if (in_line[0:1] == in_line[1:2]):
            continue
        for utf_chara in list(in_line):
            unicode = ord(utf_chara)
            if (unicode in [10,13]):
                continue
            if (0xffff < unicode):
                print('This converter not support over u+ffff [' + chr(unicode) + ']')
                exit
            if (not (unicode in list_chara)):
                list_chara.append(unicode)
    tofu_chara = list_chara[0]
    list_chara.sort()
    return list_chara,tofu_chara

def get_chara_dict_fbox_mbox_mask(list_chara,font_filename,font_size):
    # Open fontfile
    font = ImageFont.truetype(font_filename,size = font_size)
    chara_dict_fbox_mbox_mask = {}
    fbox_x_max = 0
    fbox_y_max = 0
    for chara in list_chara:
        # get box and mask
        fbox = font.getbbox(chr(chara))
        mask = font.getmask(chr(chara))
        mbox = mask.getbbox()
        mask_bitstream = ''
        if (mbox is not None):
            for iy in range(mbox[1],mbox[3]):
                for ix in range(mbox[0],mbox[2]):
                    pixel = mask.getpixel((ix,iy))
                    if (pixel != 0):
                        mask_bitstream += '1'
                    else:
                        mask_bitstream += '0'
        chara_dict_fbox_mbox_mask[chara] = [fbox,mbox,mask,mask_bitstream]
        if (fbox_x_max < fbox[2]):
            fbox_x_max = fbox[2]
        if (fbox_y_max < fbox[3]):
            fbox_y_max = fbox[3]
    return chara_dict_fbox_mbox_mask,fbox_x_max,fbox_y_max

def get_box_dict_numchara(chara_dict_fbox_mbox_mask):
    # create box dictionary
    box_dict_numchara = {}
    for chara in sorted(chara_dict_fbox_mbox_mask.keys()):
        fbox = chara_dict_fbox_mbox_mask[chara][0]
        mbox = chara_dict_fbox_mbox_mask[chara][1]
        if (fbox is None):
            fbox = (0,0,0,0)
        if (mbox is None):
            mbox = (0,0,0,0)
        if (fbox not in box_dict_numchara):
            box_dict_numchara[fbox] = 0
        box_dict_numchara[fbox] += 1
        if (mbox not in box_dict_numchara):
            box_dict_numchara[mbox] = 0
        box_dict_numchara[mbox] += 1
    return box_dict_numchara

def get_box_dict_bitstream(box_dict_numchara):
    # create box to bitstream dictionary
    id_list_box = []
    id_list_numchara = []
    box_dict_numchara_items = sorted(sorted(box_dict_numchara.items()), key=lambda x:x[1], reverse=True)
    # count chara by box for compaction
    for box_numchara in box_dict_numchara_items:
        id_list_box.append(box_numchara[0])
        id_list_numchara.append(box_numchara[1])
    id_max = len(id_list_numchara)
    id = 0
    bitstream_bitlen = []
    box_dict_bitstream = {}
    while True:
        # calculate entropy
        sum_numchara = 0
        for id_t in range(id,id_max):
            sum_numchara += id_list_numchara[id_t]
        entropy = -math.log2(id_list_numchara[id]/sum_numchara)
        if (entropy < 2):
            entropy = 0
        # convert entropy to bits
        bits = math.ceil(entropy)
        numcounts = 2 ** bits
        # generate bitstream
        for num in range(min(numcounts,id_max-id)):
            value_bitstream = ''
            if (bits > 0):
                value_bitstream = format(num,'0'+str(bits)+'b')[::-1]
            bitstream = '1'*(len(bitstream_bitlen))+'0'+value_bitstream
            box_dict_bitstream[id_list_box[id+num]] = bitstream
        bitstream_bitlen.append(bits)
        id += numcounts
        if id >= id_max:
            break
    return box_dict_bitstream,bitstream_bitlen,id_list_box

def get_boxid_to_bitstream(box,box_dict_bitstream):
    if (box is None):
        box = (0,0,0,0)
    box_bitstream = box_dict_bitstream[box]
    return box_bitstream

def get_fbox_mbox_mask_to_bitstream(chara,chara_dict_fbox_mbox_mask,box_dict_bitstream):
    fbox = chara_dict_fbox_mbox_mask[chara][0]
    mbox = chara_dict_fbox_mbox_mask[chara][1]
    mask = chara_dict_fbox_mbox_mask[chara][2]
    mask_bitstream = chara_dict_fbox_mbox_mask[chara][3]
    chara_bitstream = ''
    chara_bitstream += get_boxid_to_bitstream(fbox,box_dict_bitstream)
    chara_bitstream += get_boxid_to_bitstream(mbox,box_dict_bitstream)
    chara_bitstream += mask_bitstream
    return chara_bitstream

def get_chara_dict_bitstream(chara_dict_fbox_mbox_mask,box_dict_bitstream):
    chara_dict_bitstream = {}
    for chara in chara_dict_fbox_mbox_mask.keys():
        bitstream = get_fbox_mbox_mask_to_bitstream(chara,chara_dict_fbox_mbox_mask,box_dict_bitstream)
        chara_dict_bitstream[chara] = bitstream
    return chara_dict_bitstream

def get_list_bitstreamlen_list_chara(chara_dict_bitstream):
    sum_bitstreamlen = 0
    max_bitstreamlen = 0
    bitstreamlen_dict_list_chara = {}
    for chara in sorted(chara_dict_bitstream.keys()):
        bitstreamlen = len(chara_dict_bitstream[chara])
        sum_bitstreamlen += bitstreamlen
        if (max_bitstreamlen < bitstreamlen):
            max_bitstreamlen = bitstreamlen
        if (bitstreamlen not in bitstreamlen_dict_list_chara):
            bitstreamlen_dict_list_chara[bitstreamlen] = []
        bitstreamlen_dict_list_chara[bitstreamlen].append(chara)
    max_numchara = 0
    for list_chara in bitstreamlen_dict_list_chara.values():
        numchara = len(list_chara)
        if (max_numchara < numchara):
            max_numchara = numchara
    bits_glyphbitstreamlen = math.ceil(math.log2(max_bitstreamlen+1))
    bits_numchara = math.ceil(math.log2(max_numchara+1))
    entrycost = bits_glyphbitstreamlen + bits_numchara
    doing = True
    while (doing):
        doing = False
        list_bitstreamlen_list_chara = []
        for bitstreamlen in sorted(bitstreamlen_dict_list_chara.keys(), reverse=True):
            if (len(list_bitstreamlen_list_chara) > 0):
                mergecost = (list_bitstreamlen_list_chara[-1][0]-bitstreamlen)*len(bitstreamlen_dict_list_chara[bitstreamlen])
                if (mergecost <= entrycost):
                    list_bitstreamlen_list_chara[-1][1].extend(bitstreamlen_dict_list_chara[bitstreamlen])
                    numchara = len(list_bitstreamlen_list_chara[-1][1])
                    if (max_numchara < numchara):
                        max_numchara = numchara
                        bits_numchara_prev = bits_numchara
                        bits_numchara = math.ceil(math.log2(max_numchara+1))
                        if (bits_numchara_prev != bits_numchara):
                            doing = True
                    if (doing):
                        break
                    continue
            list_bitstreamlen_list_chara.append([bitstreamlen,bitstreamlen_dict_list_chara[bitstreamlen].copy()])
    for bitstreamlen_list_chara in list_bitstreamlen_list_chara:
        bitstreamlen_list_chara[1].sort()
    list_bitstreamlen_list_chara.reverse()
    return list_bitstreamlen_list_chara,bits_glyphbitstreamlen,bits_numchara

def get_glyph_bitstream(list_bitstreamlen_list_chara,chara_dict_bitstream,tofu_chara):
    bitstream = ''
    tofu_pos = 0
    for bitstreamlen_list_chara in list_bitstreamlen_list_chara:
        bitstreamlen = bitstreamlen_list_chara[0]
        list_chara = bitstreamlen_list_chara[1]
        for chara in list_chara:
            chara_bitstream = chara_dict_bitstream[chara]
            chara_bitstreamlen = len(chara_bitstream)
            if (bitstreamlen > chara_bitstreamlen):
                chara_bitstream += '1'*(bitstreamlen-chara_bitstreamlen)
            if chara == tofu_chara:
                tofu_pos = len(bitstream)
            bitstream += chara_bitstream
    return bitstream,tofu_pos

def get_entry_bitstream(list_bitstreamlen_list_chara,bits_glyphbitstreamlen,bits_numchara):
    bitstream = ''
    for bitstreamlen_list_chara in list_bitstreamlen_list_chara:
        bitstreamlen = bitstreamlen_list_chara[0]
        numchara = len(bitstreamlen_list_chara[1])
        numchara_bitstream = format(numchara,'0'+str(bits_numchara)+'b')[::-1]
        bitstream += numchara_bitstream
        bitstreamlen_bitstream = format(bitstreamlen,'0'+str(bits_glyphbitstreamlen)+'b')[::-1]
        bitstream += bitstreamlen_bitstream
    return bitstream

def get_boxtable_bitstream(id_list_box):
    max_boxvalue = 0
    for box in id_list_box:
        for ix in range(4):
            if (max_boxvalue < box[ix]):
                max_boxvalue = box[ix]
    bits_boxvalue = math.ceil(math.log2(max_boxvalue+1))
    bitstream = ''
    for box in id_list_box:
        for ix in range(4):
            boxvalue = box[ix]
            boxvalue_bitstream = format(boxvalue,'0'+str(bits_boxvalue)+'b')[::-1]
            bitstream += boxvalue_bitstream
    return bitstream,bits_boxvalue

def get_charatable(list_bitstreamlen_list_chara):
    charatable = []
    for bitstreamlen_list_chara in list_bitstreamlen_list_chara:
        charatable.extend(bitstreamlen_list_chara[1])
    return charatable

def get_bytestream_from_bitstream(bitstream):
    bytestream = []
    while(len(bitstream)>=8):
        bytestream.append(int(bitstream[0:8][::-1],2))
        bitstream = bitstream[8:]
    if (len(bitstream) > 0):
        bytestream.append(int(bitstream[::-1],2))
        bitstream = bitstream[8:]
    return bytestream

def get_cppbytearray_from_bitstream(bitstream):
    cppbytearray = ''
    bytestream = get_bytestream_from_bitstream(bitstream)
    numcol = 0
    for byte in bytestream:
        if (numcol == 0):
            cppbytearray += '\t'
        else:
            cppbytearray += ','
            if (numcol > byte_per_line):
                cppbytearray += '\n\t'
                numcol = 0
            else:
                cppbytearray += ' '
        cppbytearray += '0x' + format(byte,'02X')
        numcol += 1
    if (numcol > 0):
        cppbytearray += '\n'
    return cppbytearray

def get_cppwordarray_from_wordlist(wordlist):
    cppwordarray = ''
    numcol = 0
    for word in wordlist:
        if (numcol == 0):
            cppwordarray += '\t'
        else:
            cppwordarray += ','
            if (numcol > word_per_line):
                cppwordarray += '\n\t'
                numcol = 0
            else:
                cppwordarray += ' '
        cppwordarray += '0x' + format(word,'04X')
        numcol += 1
    if (numcol > 0):
        cppwordarray += '\n'
    return cppwordarray

def get_cppintarray_from_intlist(intlist):
    cppintarray = ''
    numcol = 0
    for int_val in intlist:
        if (numcol == 0):
            cppintarray += '\t'
        else:
            cppintarray += ','
            if (numcol > int_per_line):
                cppintarray += '\n\t'
                numcol = 0
            else:
                cppintarray += ' '
        cppintarray += format(int_val,'d')
        numcol += 1
    if (numcol > 0):
        cppintarray += '\n'
    return cppintarray

def get_cpptype(maxval):
    if (maxval < 2**8):
        return 'uint8_t'
    elif (maxval < 2**16):
        return 'uint16_t'
    elif (maxval < 2**32):
        return 'uint32_t'
    return 'uint64_t'

def get_c_datatype_header(font_filename,font_size,chara_list_filename,output_filename_datatype):
    c_code = ''
    c_code += '#ifndef _UTF8_FONT_DATA_TYPE_H\n'
    c_code += '#define _UTF8_FONT_DATA_TYPE_H\n'
    c_code += '//! @file '+output_filename_datatype+'\n'
    c_code += '//! @brief Font DataType\n'
    c_code += '//! @note font_filename="'+font_filename+'",size='+format(font_size,'d')+'\n'
    c_code += '//! @note chara_list_filename="'+chara_list_filename+'"\n'
    c_code += '//! @date '+str(datetime.datetime.now())+'\n'
    c_code += '\n'
    return c_code

def get_c_datatype_boxtable(bits_boxvalue):
    c_code = ''
    boxvalue = (2**bits_boxvalue)-1
    c_type = get_cpptype(boxvalue)
    c_code += '//! @typedef type_font_box_pos\n'
    c_code += '//! @brief value type for font_box element (each X0,Y0,X1,Y1)\n'
    c_code += 'typedef '+c_type+'\ttype_font_box_pos;\n'
    c_code += '\n'
    return c_code

def get_c_datatype_entrytable(num_entrylist):
    c_code = ''
    c_type = get_cpptype(num_entrylist)
    c_code += '//! @typedef type_font_num_entrylist\n'
    c_code += '//! @brief value type for font_num_entrylist\n'
    c_code += 'typedef '+c_type+'\ttype_font_num_entrylist;\n'
    c_code += '\n'
    return c_code

def get_c_datatype_charatable(charatable):
    c_code = ''
    numchara = len(charatable)
    c_type = get_cpptype(numchara)
    c_code += '//! @typedef type_font_charatable_pos\n'
    c_code += '//! @brief word pointer type for font_charatable\n'
    c_code += 'typedef '+c_type+'\ttype_font_charatable_pos;\n'
    c_code += '\n'
    return c_code

def get_c_datatype_boxid_bitlen(id_list_box):
    c_code = ''
    boxlist_id = len(id_list_box)
    c_type = get_cpptype(boxlist_id)
    c_code += '//! @typedef type_font_boxlist_id\n'
    c_code += '//! @brief id type for font_boxlist\n'
    c_code += 'typedef '+c_type+'\ttype_font_boxlist_id;\n'
    c_code += '\n'
    return c_code

def get_c_datatype_glyph(glyph_bitstream):
    c_code = ''
    bitstreamlen = len(glyph_bitstream)
    c_type = get_cpptype(bitstreamlen)
    c_code += '//! @typedef type_font_glyphbit_pos\n'
    c_code += '//! @brief bit pointer type for glyph_bitstream\n'
    c_code += 'typedef '+c_type+'\ttype_font_glyphbit_pos;\n'
    c_code += '\n'
    return c_code

def get_c_datatype_footer():
    c_code = ''
    c_code += '#endif // _UTF8_FONT_DATA_TYPE_H\n'
    c_code += '\n'
    return c_code

def get_c_header(font_filename,font_size,chara_list_filename,output_filename_data,fbox_x_max,fbox_y_max):
    c_code = ''
    c_code += '#ifndef _UTF8_FONT_DATA_H\n'
    c_code += '#define _UTF8_FONT_DATA_H\n'
    c_code += '//! @file '+output_filename_data+'\n'
    c_code += '//! @brief Font Data\n'
    c_code += '//! @note font_filename="'+font_filename+'",size='+format(font_size,'d')+'\n'
    c_code += '//! @note chara_list_filename="'+chara_list_filename+'"\n'
    c_code += '//! @date '+str(datetime.datetime.now())+'\n'
    c_code += '\n'
    c_code += '//! @def FONT_BOX_X_MAX\n'
    c_code += '//! @brief max width of font box\n'
    c_code += '#define FONT_BOX_X_MAX\t'+format(fbox_x_max,'d')+'\n'
    c_code += '\n'
    c_code += '//! @def FONT_BOX_Y_MAX\n'
    c_code += '//! @brief max height of font box\n'
    c_code += '#define FONT_BOX_Y_MAX\t'+format(fbox_y_max,'d')+'\n'
    c_code += '\n'
    return c_code

def get_c_boxtable(boxtable_bitstream,bits_boxvalue,num_boxtable):
    c_code = ''
    c_code += '//! @def FONT_NUM_BOXTABLE\n'
    c_code += '//! @brief length of font_entrylist\n'
    c_code += '#define FONT_NUM_BOXTABLE\t'+format(num_boxtable,'d')+'\n'
    c_code += '\n'
    c_code += '//! @def FONT_BITS_BOXVALUE\n'
    c_code += '//! @brief Bitwidth of each BoxValue(X0,Y0,X1,Y1) in font_boxtable_bitstream\n'
    c_code += '#define FONT_BITS_BOXVALUE\t'+format(bits_boxvalue,'d')+'\n'
    c_code += '\n'
    c_code += '//! @var font_boxtable_bitstream\n'
    c_code += '//! @brief List of Box(X0,Y0,X1,Y1) as bitstream\n'
    c_code += 'static const uint8_t font_boxtable_bitstream [] PROGMEM = {\n'
    c_code += get_cppbytearray_from_bitstream(boxtable_bitstream)
    c_code += '};\n'
    c_code += '\n'
    return c_code

def get_c_entrytable(entry_bitstream,bits_glyphbitstreamlen,bits_numchara,num_entrylist):
    c_code = ''
    c_code += '//! @def FONT_NUM_ENTRYLIST\n'
    c_code += '//! @brief length of font_entrylist\n'
    c_code += '#define FONT_NUM_ENTRYLIST\t'+format(num_entrylist,'d')+'\n'
    c_code += '\n'
    c_code += '//! @def FONT_BITS_NUMCHARA\n'
    c_code += '//! @brief Bitstream length of numchara in font_entrylist_bitstream\n'
    c_code += '#define FONT_BITS_NUMCHARA\t'+format(bits_numchara,'d')+'\n'
    c_code += '\n'
    c_code += '//! @def FONT_BITS_GLYPTHLENGTH\n'
    c_code += '//! @brief Bitstream length of glypth_length in font_entrylist_bitstream\n'
    c_code += '#define FONT_BITS_GLYPTHLENGTH\t'+format(bits_glyphbitstreamlen,'d')+'\n'
    c_code += '\n'
    c_code += '//! @var font_entrylist_bitstream\n'
    c_code += '//! @brief List of numchara, glypth_length as bitstream\n'
    c_code += 'static const uint8_t font_entrylist_bitstream [] PROGMEM = {\n'
    c_code += get_cppbytearray_from_bitstream(entry_bitstream)
    c_code += '};\n'
    c_code += '\n'
    return c_code

def get_c_charatable(charatable):
    c_code = ''
    c_code += '//! @var font_charatable\n'
    c_code += '//! @brief List of UTF Character Code (16bit)\n'
    c_code += 'static const uint16_t font_charatable [] PROGMEM = {\n'
    c_code += get_cppwordarray_from_wordlist(charatable)
    c_code += '};\n'
    c_code += '\n'
    return c_code

def get_c_tofupos(tofu_pos):
    c_code = ''
    c_code += '//! @var font_tofu_glyphbit_pos\n'
    c_code += '//! @brief glyphbit pointer as tofu_character\n'
    c_code += 'static const type_font_glyphbit_pos font_tofu_glyphbit_pos = 0x'+format(tofu_pos,'X')+';\n'
    c_code += '\n'
    return c_code

def get_c_boxid_bitlen(boxid_bitlen,id_list_box):
    c_code = ''
    boxlist_id = len(id_list_box)
    boxlist_id_bitlen = math.ceil(math.log2(boxlist_id+1))
    c_type = get_cpptype(boxlist_id_bitlen)
    c_code += '//! @var font_boxlist_id_bitlen\n'
    c_code += '//! @brief Bitstream length of boxlist_id in font_glyph_bitstream\n'
    c_code += 'static const '+c_type+' font_boxlist_id_bitlen [] PROGMEM = {\n'
    c_code += get_cppintarray_from_intlist(boxid_bitlen)
    c_code += '};\n'
    c_code += '\n'
    return c_code

def get_c_glyph(glyph_bitstream):
    c_code = ''
    c_code += '//! @var font_glyph_bitstream\n'
    c_code += '//! @brief Glypth(fbox_id,mbox_id,mask) as bitstream\n'
    c_code += 'static const uint8_t font_glyph_bitstream [] PROGMEM = {\n'
    c_code += get_cppbytearray_from_bitstream(glyph_bitstream)
    c_code += '};\n'
    c_code += '\n'
    return c_code

def get_c_footer():
    c_code = ''
    c_code += '#endif // _UTF8_FONT_DATA_H\n'
    c_code += '\n'
    return c_code


#Get box and mask of listed character from Fonts
list_chara,tofu_chara = read_list_chara(chara_list_filename)
chara_dict_fbox_mbox_mask,fbox_x_max,fbox_y_max = get_chara_dict_fbox_mbox_mask(list_chara, font_filename,font_size)

#Compaction and generate bitstream and table
box_dict_numchara = get_box_dict_numchara(chara_dict_fbox_mbox_mask)
box_dict_bitstream,boxid_bitlen,id_list_box = get_box_dict_bitstream(box_dict_numchara)
chara_dict_bitstream = get_chara_dict_bitstream(chara_dict_fbox_mbox_mask,box_dict_bitstream)
list_bitstreamlen_list_chara,bits_glyphbitstreamlen,bits_numchara = get_list_bitstreamlen_list_chara(chara_dict_bitstream)
boxtable_bitstream,bits_boxvalue = get_boxtable_bitstream(id_list_box)
entry_bitstream = get_entry_bitstream(list_bitstreamlen_list_chara,bits_glyphbitstreamlen,bits_numchara)
charatable = get_charatable(list_bitstreamlen_list_chara)
glyph_bitstream,tofu_pos = get_glyph_bitstream(list_bitstreamlen_list_chara,chara_dict_bitstream,tofu_chara)

#Generate C source code
c_datatype_header = get_c_datatype_header(font_filename,font_size,chara_list_filename,output_filename_datatype)
c_datatype_boxtable = get_c_datatype_boxtable(bits_boxvalue)
c_datatype_entrytable = get_c_datatype_entrytable(len(list_bitstreamlen_list_chara))
c_datatype_charatable = get_c_datatype_charatable(charatable)
c_datatype_boxid_bitlen = get_c_datatype_boxid_bitlen(id_list_box)
c_datatype_glyph = get_c_datatype_glyph(glyph_bitstream)
c_datatype_footer = get_c_datatype_footer()
c_header = get_c_header(font_filename,font_size,chara_list_filename,output_filename_data,fbox_x_max,fbox_y_max)
c_boxtable = get_c_boxtable(boxtable_bitstream,bits_boxvalue,len(id_list_box))
c_entrytable = get_c_entrytable(entry_bitstream,bits_glyphbitstreamlen,bits_numchara,len(list_bitstreamlen_list_chara))
c_charatable = get_c_charatable(charatable)
c_tofupos = get_c_tofupos(tofu_pos)
c_boxid_bitlen = get_c_boxid_bitlen(boxid_bitlen,id_list_box)
c_glyph = get_c_glyph(glyph_bitstream)
c_footer = get_c_footer()

with open(output_filename_datatype, 'w', encoding='utf-8') as f:
    f.write(c_datatype_header)
    f.write(c_datatype_boxtable)
    f.write(c_datatype_entrytable)
    f.write(c_datatype_boxid_bitlen)
    f.write(c_datatype_glyph)
    f.write(c_datatype_charatable)
    f.write(c_datatype_footer)

with open(output_filename_data, 'w', encoding='utf-8') as f:
    f.write(c_header)
    f.write(c_boxtable)
    f.write(c_entrytable)
    f.write(c_boxid_bitlen)
    f.write(c_glyph)
    f.write(c_charatable)
    f.write(c_tofupos)
    f.write(c_footer)
