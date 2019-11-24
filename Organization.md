
# Organization of code

tiffFormat.h
 - all structs should be here

tiffImage.c & .h
 - image and file struct
 - image checking code

tiffReader.c
 - read image and turn it to image and file struct data

tiffTagStorage.c & .h
 - data tag

# Restrictions of this code
  
Only supports:
 - RGB & GrayScale
 - non-compressed images
 - files with one image in them 

