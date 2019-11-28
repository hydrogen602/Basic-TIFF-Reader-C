
# Organization of code

tiffFormat.h
 - all structs should be here
 - typedef for types used in tif
 - macro defs for image ids

tiffImage.c & .h
 - image checking code

tiffReader.c
 - read image file and find and load the tags in it

tiffTagStorage.c & .h
 - data tag
 - making & removing data tags
 - dealing with data tags
 
 tiffReaderHelper.c & .h
 - make sense of a read image
 - load pixels into memory

# Restrictions of this code
  
Only supports:
 - RGB
 - non-compressed images
 - files with one image in them
 - images organized as strips
 - 8 bit pixels

