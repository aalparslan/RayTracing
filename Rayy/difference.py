from cv2 import imread, imwrite
import os


im_name = 'horse_and_mug.ppm'

img_org = imread('../ODTUCLASS/Sample Scenes/Outputs/'+im_name)
img_pre = imread(im_name)


diff = abs(img_org - img_pre)

imwrite('difference_' + im_name.split('.')[0] + '_image.jpg', diff)




img_org = imread('bunny_float.ppm')
img_pre = imread('bunny_double.ppm')


diff = abs(img_org - img_pre)

imwrite('difference_bunny_float_double_image.jpg', diff)


img_org = imread('bunny.ppm')
img_pre = imread('bunny_double.ppm')


diff = abs(img_org - img_pre)

imwrite('difference_bunny_float_double_image2.jpg', diff)