from os import listdir
import re

list_file = open("task_list.txt", 'w')

files = listdir('./')

for filename in files:
    if re.match(r'\d+\.xml', filename):
        list_file.write(filename + '\n')
list_file.close()
