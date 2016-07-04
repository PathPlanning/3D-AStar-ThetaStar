#!./py_env/bin/python3

from PIL import Image, ImageDraw


def illustrate(input_filename, output_filename, output_format="PNG"):
    # Parsing map and path
    path_file = open(input_filename)
    height = int(path_file.readline().rstrip())
    width = int(path_file.readline().rstrip())
    path_file.readline()
    matrix = []

    for i in range(height):
        matrix.append([int(j) for j in path_file.readline().rstrip().split()])

    path_file.readline()

    path = []
    for line in path_file.readlines():
        path.append(tuple([int(j) for j in line.rstrip().split()]))

    path_file.close()
    start = path[0]
    finish = path[-1]
    path = set(path)

    # Drawing result. Each cell has 2 pixel edge
    EMPTY_COLOR = (255, 255, 255)
    PATH_COLOR = (255, 255, 0)
    SNAG_COLOR = (0, 0, 0)
    START_COLOR = (0, 255, 0)
    FINISH_COLOR = (255, 0, 0)

    im = Image.new("RGB", (2 * width, 2 * height), color=(255, 255, 255))

    dr = ImageDraw.Draw(im)
    for x in range(height):
        for y in range(width):
            if ((x, y) == start):
                color = START_COLOR
            elif ((x, y) == finish):
                color = FINISH_COLOR
            elif (matrix[x][y]):
                color = SNAG_COLOR
            elif ((x, y) in path):
                color = PATH_COLOR
            else:
                color = EMPTY_COLOR

            dr.rectangle([(2 * x, 2 * y), (2 * x + 2, 2 * y + 2)], fill=color)

    del dr
    im.save(output_filename, output_format)


if __name__ == "__main__":
    from os import listdir
    import re

    dir_path = input("Please enter the path to the directory with calculated path files:  ")
    files = listdir(dir_path)
    for filename in files:
        m = re.match(r'(?P<number>\d+)\.?.*\.path\.txt', filename)
        if m:
            number = m.group("number")
            illustrate(dir_path + '/' + filename, dir_path + '/' + str(number) + ".png")
