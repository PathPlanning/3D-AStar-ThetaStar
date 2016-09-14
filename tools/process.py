#!./venv/bin/python3

import multiprocessing
import subprocess
import fractions
import xml.etree.ElementTree as ET

import re
from PIL import Image, ImageDraw, ImageFont

SETTINGS = {
    'max_treads': 3,
    'EMPTY_COLOR': (255, 255, 255),
    'PATH_COLOR': (255, 255, 0),
    'OBSTACLE_COLOR': (0, 0, 0),
    'START_COLOR': (0, 255, 0),
    'FINISH_COLOR': (255, 0, 0),
    'CLOSED_COLOR': (87, 72, 468),
    'OPENED_COLOR': (123, 63, 0),
    'TEXT_COLOR': (0, 0, 0),
    'TITLE': {
        'MARGIN_TOP': 2,
        'MARGIN_BOTTOM': 2,
        'BOX_HEIGHT': 30,
        'SEP_LINE': True,
        'SEP_HEIGHT': 4,
        'SEP_COLOR': (123, 0, 28)
    }
}


def parse_log(filename, shell=False):
    parse_result = {}

    tree = ET.parse(filename)
    root = tree.getroot()
    map = root.find('map')
    try:
        parse_result["title"] = map.find('title').text
    except AttributeError:
        parse_result["title"] = None
    parse_result["width"] = int(map.find("width").text)
    parse_result["height"] = int(map.find("height").text)
    try:
        parse_result["cellsize"] = int(map.find("cellsize").text)
    except AttributeError:
        print("Couldn't find size of cell (attribute <cellsize>) in {}. Would be ignored.".format(filename))
    parse_result['start'] = (int(map.find("startx").text), int(map.find("starty").text))
    parse_result['finish'] = (int(map.find("finishx").text), int(map.find("finishy").text))

    parse_result['map'] = []
    for plain in map.find('grid'):
        parse_result['map'].append([int(i) for i in plain.text.split()])

    algo_name = root.find('algorithm').find('searchtype').text.lower()
    if algo_name in {'bfs', 'jp_search', 'dijkstra', 'astar'}:
        any_angle_search = False
    else:
        any_angle_search = True

    log = root.find('log')

    level = log.find('lplevel')
    # For all paths 'lppath' section is used
    if (True or not any_angle_search and level is not None):
        parse_result['section_path'] = False
        path = set()
        for node in level.iter('node'):
            path.add((int(node.get('x')), int(node.get('y'))))
        parse_result['path'] = path
    else:
        path = []
        parse_result['section_path'] = True
        level = log.find('hplevel')
        section = level.find('section')
        path.append((int(section.get('start.x')), int(section.get('start.y'))))
        for section in level.iter('section'):
            path.append((int(section.get('finish.x')), int(section.get('finish.y'))))
        parse_result['path'] = path

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


def illustrate(parsed_data, output_filename, output_format="PNG", scale=2):
    scale = int(scale)
    scale = 1 if scale == 0 else scale
    height = parsed_data['height']
    width = parsed_data['width']
    start = parsed_data['start']
    finish = parsed_data['finish']

    text_zone_height = 0

    im = Image.new("RGB", (scale * width, scale * (height + text_zone_height)), color=SETTINGS['EMPTY_COLOR'])

    dr = ImageDraw.Draw(im)

    for x in range(width):
        for y in range(height):
            if (x, y) in parsed_data['opened_list']:
                color = SETTINGS['OPENED_COLOR']
            elif (x, y) in parsed_data['closed_list']:
                color = SETTINGS['CLOSED_COLOR']
            elif (x, y) == start:
                color = SETTINGS["START_COLOR"]
            elif (x, y) == finish:
                color = SETTINGS["FINISH_COLOR"]
            elif not parsed_data['section_path'] and (x, y) in parsed_data['path']:
                color = SETTINGS['PATH_COLOR']
            elif parsed_data['map'][y][x]:
                color = SETTINGS['OBSTACLE_COLOR']
            else:
                color = SETTINGS['EMPTY_COLOR']

            dr.rectangle([(scale * x, scale * y), (scale * x + scale, scale * y + scale)], fill=color)

    if parsed_data['section_path']:
        dr.line(list(map(lambda elem: (scale * elem[0], scale * elem[1]), parsed_data['path'])),
                fill=SETTINGS['PATH_COLOR'], width=scale)
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


def make_path_and_picture(exec_filename, input_filename, log_filename, picture_filename, scale=2, picture_format='PNG'):
    code = subprocess.run([exec_filename, input_filename],
                          stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT).returncode
    if code == 0:
        data = parse_log(log_filename)
        illustrate(data, picture_filename, picture_format, scale)
        print("{} processed successfully".format(input_filename))
    else:
        print("Error has occurred during searching path for {}.\
        Program has finished with exit code {}".format(input_filename, code))


if __name__ == "__main__":
    import argparse
    from os import listdir, cpu_count, path

    parser = argparse.ArgumentParser()
    parser.add_argument("--exe", required=True, help="Путь к исполняемому файлу проекта")
    parser.add_argument("--test", required=True, help="Путь к папке с заданиями в формате XML или единичному файлу")
    parser.add_argument("--scale", required=False, type=int,
                        help="Масштаб карты, относительно заданного в файле", default=2)
    args = parser.parse_args()
    exec_path = path.abspath(args.exe)
    if not path.isfile(exec_path):
        print("Incorrect path to the executable")
        exit(1)
    dir_path = path.abspath(args.test)
    if path.isdir(dir_path):
        files = listdir(dir_path)
    elif path.isfile(dir_path):
        files = [dir_path]
        dir_path = path.dirname(dir_path)
    else:
        print("Incorrect path to the test")
        exit(1)

    tasks = {}
    for filename in files:
        m = re.match(r'(?P<name>.+)(?<!_log)\.xml', filename)
        if m:
            tasks[path.join(dir_path, filename)] = (get_log_output_filename(path.join(dir_path, filename)),
                                                    path.join(dir_path, m.group('name') + '.plain.png'))

    with multiprocessing.Pool(min(SETTINGS['max_treads'], cpu_count())) as pool:
        for inp_path, val in tasks.items():
            pool.apply_async(make_path_and_picture, [exec_path, inp_path, val[0], val[1], args.scale])
        pool.close()
        pool.join()
