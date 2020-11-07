from cv2 import imread, imwrite

im_name = 'horse_and_mug.ppm'

img_org = imread('../ODTUCLASS/Sample Scenes/Outputs/'+im_name)
img_pre = imread(im_name)


diff = abs(img_org - img_pre)

imwrite('difference_image.jpg', diff)

print('AAA')