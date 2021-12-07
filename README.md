# PROGETTO RETI LOGICHE 2021

This project aims to implement an hardware component described in VHDL that, once receveid one image and some control signals as input returns the equalized version of the picture. The algorithm to use for the equalization has been provided with the specifics and the maximum dimension allowed for input images is 128 x 128 pixels.

# Equalization algorithm 
These are the steps of the algorithm that has been used for the equalization.
1. Research of the minimum value (MIN_PIXEL_VALUE) and maximum value (MAX_PIXEL_VALUE)
2. Calculation of delta value  
    DELTA_VALUE = MAX_PIXEL_VALUE - MIN_PIXEL_VALUE
4. Calculation of shift level  
    SHIFT_LEVEL = 8 - ⌊log2(DELTA_VALUE + 1)⌋
5. Tranformation of each pixel  
    TEMP_PIXEL = (CURRENT_PIXEL_VALUE − MIN_PIXEL_VALUE) << SHIFT_LEVEL  
    NEW_PIXEL_VALUE = min(255, TEMP_PIXEL)

# Content
In this repository you can find:
* The [vhdl code](10631762_10629918.vhd)
* The [latex code](doc.tex) of the documentation
* The [pdf](doc_reti_logiche.pdf) related to the latex code
* The [test generator](gen_tb.c)
