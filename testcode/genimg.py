from PIL import Image
import sys

print("#ifndef _TEST_H")
print("#define _TEST_H")
print("#include <stdint.h>")
print("#define u8 uint8_t")

width = 100
height = 100
grid = 5
grid_width = width//grid
grid_height = height//grid

im = Image.new('RGB', (width, height), (255, 255, 255))
r_grid = Image.new('RGB', (grid_width, grid_height), (255, 0, 0))
b_grid = Image.new('RGB', (grid_width, grid_height), (0, 0, 0))

data = 238

for y in range(grid):
    for x in range(grid):
        if (y == 0 and (x == 0 or x == grid - 1)) or (y == grid - 1 and x == 0):
            im.paste(r_grid, (x*grid_width, y*grid_height))
        else:
            if (data % 2 == 0):
                im.paste(b_grid, (x*grid_width, y*grid_height))
            data //= 2

im = im.rotate(10, expand=1)

base = Image.new('RGB', (256, 256), (128, 128, 128));
base.paste(im, (110, 110))
base = base.resize((width, height))
base.show()
base.save('result.png')

print("#define IMAGE_WIDTH %d" % width)
print("#define IMAGE_HEIGHT %d" % height)
print("u8 im[%d][%d]=" % (width, height))

print('{')
for i in range(width):
    print('{', end='')
    for j in range(height):
        pixel = base.getpixel((j, i))
        if pixel[0]+pixel[1]+pixel[2] > 384:
            value =  2
        elif pixel[0] > 128:
            value = 1
        else:
            value = 0

        print(value, end='')
        if j < height - 1:
            print(',', end='')
    print('}', end='')
    if i < width - 1:
        print(',')
print('};')

print("#endif")