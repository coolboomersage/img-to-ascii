# img-to-ascii
Program will take a .png or .jpg file and turn it into an ascii text file.  

With the 2 files included, helper.cpp will take a string of ascii characters and produce a lookup table in the form of a .txt file that main.cpp will then access. This is accomplished by putting the character on a white 32x32 canvas then taking the mean of all pixel brightness values /255 to get the values in [0-1].  

main.cpp is the file that will take inputIMG.png OR inputIMG.jpg with .png files taking priority if both are present. Then resizing the image defined by the defined SIZE on line 14 and keeeping the original aspect ratio and turning the image greyscale. This image is then saved as debugIMG.png. After that a thread is created using the pthreads lib per column of pixels and assigns each "pixel" in the ascii.txt file to an ascii character in lookup.txt by finding the closest value to the pixels brightness.
