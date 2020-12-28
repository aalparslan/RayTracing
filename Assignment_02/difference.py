from cv2 import imread, imwrite
import os
import sys

if len(sys.argv) == 1 or len(sys.argv) > 3:
    print('Use:')
    print('\tpython3 difference.py image_1 # difference of image with the copy in ../ODTUCLASS/Sample Scenes/Outputs/')
    print('\tpython3 difference.py image_1 image_2 # difference of two images')

    print()
    print('Output is difference_image.jpg')

if len(sys.argv) == 2:

    im_name = sys.argv[1]

    img_org = imread('../ODTUCLASS/Sample Scenes/Outputs/'+im_name)
    img_pre = imread(im_name)


    diff = abs(img_org - img_pre)

    imwrite('difference_image.jpg', diff)

if len(sys.argv) == 3:

    im_name1 = sys.argv[1]
    im_name2 = sys.argv[2]

    img_org = imread(im_name1)
    img_pre = imread(im_name2)


    diff = abs(img_org - img_pre)

    imwrite('difference_image.jpg', diff)


print()
print('Output is difference_image.jpg')