#!./venv/bin/python3

from PIL import Image, ImageDraw
import xml.etree.ElementTree as ET


def parse_log(filename):
    parse_result = {}

    tree = ET.parse(filename)
    root = tree.getroot()
    map = root.find('map')
    #parse_result["title"] = map.find('title').text
    parse_result["width"] = int(map.find("width").text)
    parse_result["height"] = int(map.find("height").text)
    parse_result["cellsize"] = int(map.find("cellsize").text)
    parse_result['start'] = (int(map.find("startx").text), int(map.find("starty").text))
    parse_result['finish'] = (int(map.find("finishx").text), int(map.find("finishy").text))

    parse_result['map'] = []
    for plain in map.find('grid'):
        parse_result['map'].append([int(i) for i in plain.text.split()])

    log = root.find('log')

    level = log.find('lplevel')
    if (level is not None):
        parse_result['section_path'] = False
        path = set()
        for node in level.iter('node'):
            path.add((int(node.get('x')), int(node.get('y'))))
        parse_result['path'] = path
    else:
        parse_result['section_path'] = True
        level = log.find('hplevel')

    level = log.find('viewed')
    closed = set()
    opened = set()
    if level is None:
        print("Can not find viewed section. Points visited by algorithm won't be shown for {}.".format(filename))
    else:
        for node in level.iter('node'):
            if (node.get('closed')):
                closed.add((int(node.get('x')), int(node.get('y'))))
            else:
                opened.add((int(node.get('x')), int(node.get('y'))))

    parse_result['closed_list'] = closed
    parse_result['opened_list'] = opened

    return parse_result

def illustrate(parsed_data, output_filename, output_format="PNG"):
    height = parsed_data['height']
    width = parsed_data['width']
    start = parsed_data['start']
    finish = parsed_data['finish']

    # Drawing result. Each cell has 2 pixel edge
    EMPTY_COLOR = (255, 255, 255)
    PATH_COLOR = (255, 255, 0)
    SNAG_COLOR = (0, 0, 0)
    START_COLOR = (0, 255, 0)
    FINISH_COLOR = (255, 0, 0)
    CLOSED_COLOR = (87, 72, 468)
    OPENED_COLOR = (18, 47, 170)

    im = Image.new("RGB", (2 * width, 2 * height), color=(255, 255, 255))

    dr = ImageDraw.Draw(im)
    for x in range(width):
        for y in range(height):
            if (x, y) == start:
                color = START_COLOR
            elif (x, y) == finish:
                color = FINISH_COLOR
            elif parsed_data['map'][y][x]:
                color = SNAG_COLOR
            elif (x, y) in parsed_data['path']:
                color = PATH_COLOR
            elif (x, y) in parsed_data['closed_list']:
                color = CLOSED_COLOR
            elif (x, y) in parsed_data['opened_list']:
                color = OPENED_COLOR
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
        m = re.match(r'(?P<number>\d+)\.?.*_log\.xml', filename)
        if m:
            number = m.group("number")
            #try:
            data = parse_log(dir_path + '/' + filename)
            illustrate(data, dir_path + '/' + str(number) + ".png")
            #except Exception:
                #print("File {} is not a correct log file. Can not make a picture".format(filename))
