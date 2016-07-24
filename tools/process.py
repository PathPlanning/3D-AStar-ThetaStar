#!./venv/bin/python3

import multiprocessing
import subprocess
import xml.etree.ElementTree as ET

import re
from PIL import Image, ImageDraw

SETTINGS = {
    'max_treads': 3,
    'EMPTY_COLOR': (255, 255, 255),
    'PATH_COLOR': (255, 255, 0),
    'OBSTACLE_COLOR': (0, 0, 0),
    'START_COLOR': (0, 255, 0),
    'FINISH_COLOR': (255, 0, 0),
    'CLOSED_COLOR': (87, 72, 468),
    'OPENED_COLOR': (123, 63, 0)
}


def parse_log(filename, shell=False):
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
        if shell:
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
    im = Image.new("RGB", (2 * width, 2 * height), color=SETTINGS['EMPTY_COLOR'])

    dr = ImageDraw.Draw(im)
    for x in range(width):
        for y in range(height):
            if (x, y) == start:
                color = SETTINGS['START_COLOR']
            elif (x, y) == finish:
                color = SETTINGS['FINISH_COLOR']
            elif parsed_data['map'][y][x]:
                color = SETTINGS['OBSTACLE_COLOR']
            elif (x, y) in parsed_data['path']:
                color = SETTINGS['PATH_COLOR']
            elif (x, y) in parsed_data['closed_list']:
                color = SETTINGS['CLOSED_COLOR']
            elif (x, y) in parsed_data['opened_list']:
                color = SETTINGS['OPENED_COLOR']
            else:
                color = SETTINGS['EMPTY_COLOR']

            dr.rectangle([(2 * x, 2 * y), (2 * x + 2, 2 * y + 2)], fill=color)

    del dr
    im.save(output_filename, output_format)


def get_log_output_filename(task_filename):
    tree = ET.parse(task_filename)
    root = tree.getroot()
    logpath = root.find('options').find('logpath')
    path = logpath.text
    if path is not None:
        return path
    m = re.match(r'(?P<name>.+)\.xml', task_filename)
    return m.group('name') + "_log.xml"


def make_path_and_picture(exec_filename, input_filename, log_filename, picture_filename, picture_format='PNG'):
    code = subprocess.run([exec_filename, input_filename],
                          stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT).returncode
    if code == 0:
        data = parse_log(log_filename)
        illustrate(data, picture_filename, picture_format)
        print("{} processed successfully".format(input_filename))
    else:
        print("Error has occurred during searching path for {}.\
        Program has finished with exit code {}".format(input_filename, code))


if __name__ == "__main__":
    from os import listdir, cpu_count, path

    exec_path = path.abspath(input("Введите путь к исполняемому файлу проекта:  "))
    dir_path = path.abspath(input("Введите путь к папке с заданиями в формате XML:  "))
    files = listdir(dir_path)

    tasks = {}
    for filename in files:
        m = re.match(r'(?P<number>\d+)\.xml', filename)
        if m:
            tasks[path.join(dir_path, filename)] = (get_log_output_filename(path.join(dir_path, filename)),
                                                    path.join(dir_path, m.group('number') + '.png'))

    with multiprocessing.Pool(min(SETTINGS['max_treads'], cpu_count())) as pool:
        for inp_path, val in tasks.items():
            pool.apply_async(make_path_and_picture, [exec_path, inp_path, val[0], val[1]])
        pool.close()
        pool.join()
