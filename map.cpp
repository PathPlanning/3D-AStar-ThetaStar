#include "map.h"

#include <climits>
#include <stdexcept>


std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

Map::Map() {
    height = -1;
    width = -1;
    start_i = -1;
    start_j = -1;
    start_h = -1;
    goal_i = -1;
    goal_j = -1;
    goal_h = -1;
    min_altitude_limit = 0;
    max_altitude_limit = INT_MAX;
    Grid = NULL;
}

Map::~Map() {
    if (Grid) {
        for (int i = 0; i < height; i++)
            delete[] Grid[i];
        delete[] Grid;
    }
}

bool Map::CellIsTraversable(int i, int j) const {
    return Grid[i][j] == CN_GC_NOOBS;
}

bool Map::CellIsTraversable(int i, int j, int h) const {
    return !CellIsObstacle(i, j, h);
}

bool Map::CellIsObstacle(int i, int j) const {
    return (Grid[i][j] != CN_GC_NOOBS);
}

bool Map::CellIsObstacle(int i, int j, int h) const {
    return h < Grid[i][j];
}

bool Map::CellOnGrid(int i, int j) const {
    return (i < height && i >= 0 && j < width && j >= 0);
}

bool Map::CellOnGrid(int i, int j, int height) const {
    return height >= min_altitude_limit && height <= max_altitude_limit && CellOnGrid(i, j);
}

bool Map::getMap(const char *FileName) {
    TiXmlElement *root = 0, *map = 0, *element = 0;
    TiXmlNode *mapnode = 0;

    std::string value; //имя тега
    std::stringstream stream; //содердимое (текст) тега

    bool hasGridMem = false, hasGrid = false, hasHeight = false, hasWidth = false, hasSTX = false, hasSTY = false;
    bool hasSTZ = false, hasFINX = false, hasFINY = false, hasFINZ = false, hasSTH = false, hasFINH = false;
    bool hasMAXALT = false, hasALTLIM = false;

    TiXmlDocument doc(FileName);

    // Load XML File
    if (!doc.LoadFile()) {
        std::cout << "Error opening XML file!" << std::endl;
        return false;
    }
    // Get ROOT element
    root = doc.FirstChildElement(CNS_TAG_ROOT);
    if (!root) {
        std::cout << "Error! No '" << CNS_TAG_ROOT << "' tag found in XML file!" << std::endl;
        return false;
    }

    // Get MAP element
    map = root->FirstChildElement(CNS_TAG_MAP);
    if (!map) {
        std::cout << "Error! No '" << CNS_TAG_MAP << "' tag found in XML file!" << std::endl;
        return false;
    }

    //Идеология инициализация "карты":
    //Перебираем последовательно внутренности тега "карта"
    //Если впервые попалось корректное значение старта (х, у), финиша (х, у), высоты и ширины - запонимаем их
    //Если старт-х, финиш-х попадаются раньше ширины - ошибка!
    //Если старт-у, финиш-у попадаются раньше высоты - ошибка!
    //Если попадаются дубликаты (теги повторяются), после того как мы уже запомнили значения, - выводим Warning
    //т.е. ВСЕГДА запоминается и используется ТОЛЬКО первое (корректно определенное) значение старта (х, у), финиша (х, у), высоты, ширины


    for (mapnode = map->FirstChild(); mapnode; mapnode = map->IterateChildren(mapnode)) {
        if (mapnode->Type() == TiXmlNode::TINYXML_COMMENT) { // Skipping comments
            continue;
        }
        element = mapnode->ToElement(); //очередной элемент (тег)
        value = mapnode->Value(); // имя тега
        std::transform(value.begin(), value.end(), value.begin(), ::tolower); //"хорошее имя тега"

        stream.str(""); //очищаем
        stream.clear(); //буфер
        stream
                << element->GetText(); //и кладем в него содержимое (текст) тега (внимание: в теге "грид" нет текста - там вложенные теги, там будет NULL. Так что беспокоится не надо, всё под контролем!

        //1. Заготовка под грид.
        if (!hasGridMem && hasHeight &&
            hasWidth) //в какой-то момент оказалось, что есть уже высота и ширина - создаем грид
        {
            Grid = new int *[height];
            for (int i = 0; i < height; i++)
                Grid[i] = new int[width];
            hasGridMem = true;
        }

        //2. Высота
        if (value == CNS_TAG_HEIGHT) {
            if (hasHeight) //Дубль. Высота уже была.
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_HEIGHT << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_HEIGHT << "' =" << height << "will be used."
                          << std::endl;
            } else//Высоты еще не было. Или была - но плохая. В общем - пока не инициализировали...
            {
                if (!((stream >> height) && (height > 0)))//sstream >> height преобразовывает строку в число (int).
                    //Если не может преобазовать (строка косячная) - значение height остается предыдущим.
                    //То есть - по умолчанию (из конструктора) 0.
                    //То же самое касается и других преобразований ниже...
                {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_HEIGHT
                              << "' tag encountered (or could not convert to integer)." << std::endl;
                    std::cout << "Value of '" << CNS_TAG_HEIGHT << "' tag should be an integer >=0" << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_HEIGHT
                              << "' tag will be encountered later..." << std::endl;
                } else
                    hasHeight = true;
            }
        }
            //Закончили с высотой


            //3. Ширина
        else if (value == CNS_TAG_WIDTH) {
            if (hasWidth) //Дубль. Ширина уже была.
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_WIDTH << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_WIDTH << "' =" << width << "will be used." << std::endl;
            } else //Ширины еще не было. Или была - но плохая. В общем - пока не инициализировали...
            {
                if (!((stream >> width) && (width > 0))) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_WIDTH
                              << "' tag encountered (or could not convert to integer)." << std::endl;
                    std::cout << "Value of '" << CNS_TAG_WIDTH << "' tag should be an integer AND >=0" << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_WIDTH
                              << "' tag will be encountered later..." << std::endl;

                } else
                    hasWidth = true;
            }
        }
            //Закончили с шириной

        else if (value == CNS_TAG_MAXALT) {
            if (hasMAXALT) //Дубль. Максимальная высота карты уже была
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_MAXALT << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_MAXALT << "' =" << altitude << "will be used."
                          << std::endl;
            } else {
                if (!((stream >> altitude) && (altitude > 0))) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_MAXALT
                              << "' tag encountered (or could not convert to integer)." << std::endl;
                    std::cout << "Value of '" << CNS_TAG_MAXALT << "' tag should be an integer AND >=0" << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_MAXALT
                              << "' tag will be encountered later..." << std::endl;

                } else {
                    hasMAXALT = true;
                    if (!hasALTLIM) {
                        max_altitude_limit = altitude;
                    }
                }
            }
        } else if (value == CNS_TAG_ALTLIM) {
            if (hasALTLIM) //Дубль. Ограничения на высоту
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_ALTLIM << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_ALTLIM << "will be used." << std::endl;
            } else {
                hasALTLIM = true;
                if (!((element->Attribute(CNS_TAG_ALTLIM_ATTR_MIN, &min_altitude_limit)) && (min_altitude_limit > 0))) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_ALTLIM_ATTR_MIN << "' attribute of '"
                              << CNS_TAG_ALTLIM << "' tag encountered (or could not convert to integer)." << std::endl;
                    std::cout << "Value of '" << CNS_TAG_ALTLIM_ATTR_MIN << "' tag should be an integer AND >=0"
                              << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_ALTLIM_ATTR_MIN
                              << "' tag will be encountered later..." << std::endl;
                    hasALTLIM = false;
                }
                if (!((element->Attribute(CNS_TAG_ALTLIM_ATTR_MAX, &max_altitude_limit)) &&
                      (max_altitude_limit >= min_altitude_limit))) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_ALTLIM_ATTR_MAX << "' attribute of '"
                              << CNS_TAG_ALTLIM << "' tag encountered (or could not convert to integer)." << std::endl;
                    std::cout << "Value of '" << CNS_TAG_ALTLIM_ATTR_MAX
                              << "' tag should be an integer AND be not less than attribute '"
                              << CNS_TAG_ALTLIM_ATTR_MIN << "'" << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_ALTLIM_ATTR_MIN
                              << "' tag will be encountered later..." << std::endl;
                    hasALTLIM = false;
                }
            }
        }

            //4. Старт-Икс
        else if (value == CNS_TAG_STX) {
            if (!hasWidth) //Тег "старт-х" встретился раньше чем "ширина" - ошибка!
            {
                std::cout << "Error! '" << CNS_TAG_STX << "' tag encountered before '" << CNS_TAG_WIDTH << "' tag."
                          << std::endl;
                return false;
            }

            if (hasSTX) //Дубль. Старт-ИКС уже был.
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_STX << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_STX << "' =" << start_j << "will be used." << std::endl;
            } else //Старт-ИКС еще не встречался (или встречался, но корявый), в общем - надо пытаться инициализировать
            {
                if (!(stream >> start_j && start_j >= 0 && start_j < width)) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_STX
                              << "' tag encountered (or could not convert to integer)" << std::endl;
                    std::cout << "Value of '" << CNS_TAG_STX << "' tag should be an integer AND >=0 AND < '"
                              << CNS_TAG_WIDTH << "' value, which is " << width << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_STX
                              << "' tag will be encountered later..." << std::endl;
                } else
                    hasSTX = true;
            }
        }
            //Закончили со Cтарт-Икс


            //5. Старт-Игрек
        else if (value == CNS_TAG_STY) {
            if (!hasHeight) //Тег "старт-y" встретился раньше чем "высота" - ошибка!
            {
                std::cout << "Error! '" << CNS_TAG_STY << "' tag encountered before '" << CNS_TAG_HEIGHT << "' tag."
                          << std::endl;
                return false;
            }

            if (hasSTY) //Дубль. Старт-ИКС уже был.
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_STY << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_STY << "' =" << start_i << "will be used." << std::endl;
            } else //Старт-Игрек еще не встречался (или встречался, но корявый), в общем - надо пытаться инициализировать
            {
                if (!(stream >> start_i && start_i >= 0 && start_i < height)) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_STY
                              << "' tag encountered (or could not convert to integer)" << std::endl;
                    std::cout << "Value of '" << CNS_TAG_STY << "' tag should be an integer AND >=0 AND < '"
                              << CNS_TAG_HEIGHT << "' value, which is " << height << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_STY
                              << "' tag will be encountered later..." << std::endl;
                } else
                    hasSTY = true;
            }
        }
            //Закончили со Cтарт-Игрек

            // Start Z
        else if (value == CNS_TAG_STZ) {
            if (hasSTZ) //Дубль. Старт-зет уже был.
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_STZ << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_STZ << "' =" << start_h << "will be used." << std::endl;
            } else //Старт-зет еще не встречался (или встречался, но корявый), в общем - надо пытаться инициализировать
            {
                if (!(stream >> start_h && start_h >= 0)) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_STZ
                              << "' tag encountered (or could not convert to integer)" << std::endl;
                    std::cout << "Value of '" << CNS_TAG_STZ << "' tag should be an integer AND >=0 '"
                              << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_STZ
                              << "' tag will be encountered later..." << std::endl;
                } else
                    hasSTZ = true;
            }
        }

            //Финиш-Икс
        else if (value == CNS_TAG_FINX) {
            if (!hasWidth) //Тег "финиш-икс" встретился раньше чем "ширина" - ошибка!
            {
                std::cout << "Error! '" << CNS_TAG_FINX << "' tag encountered before '" << CNS_TAG_WIDTH << "' tag."
                          << std::endl;
                return false;
            }

            if (hasFINX) //Дубль. финиш-икс уже был.
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_FINX << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_FINX << "' =" << goal_j << "will be used." << std::endl;
            } else //Финиш-икс еще не встречался (или встречался, но корявый), в общем - надо пытаться инициализировать
            {
                if (!(stream >> goal_j && goal_j >= 0 && goal_j < width)) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_FINX
                              << "' tag encountered (or could not convert to integer)" << std::endl;
                    std::cout << "Value of '" << CNS_TAG_FINX << "' tag should be an integer AND >=0 AND < '"
                              << CNS_TAG_WIDTH << "' value, which is " << width << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_FINX
                              << "' tag will be encountered later..." << std::endl;
                } else
                    hasFINX = true;
            }
        }
            //Закончили со Финиш-Икс


            //Финиш-Игрек
        else if (value == CNS_TAG_FINY) {
            if (!hasHeight) //Тег "финиш-игрек" встретился раньше чем "высота" - ошибка!
            {
                std::cout << "Error! '" << CNS_TAG_FINY << "' tag encountered before '" << CNS_TAG_HEIGHT << "' tag."
                          << std::endl;
                return false;
            }

            if (hasFINY) //Дубль. Финиш-игрек уже был.
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_FINY << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_FINY << "' =" << goal_i << "will be used." << std::endl;
            } else //Финиш-Игрек еще не встречался (или встречался, но корявый), в общем - надо пытаться инициализировать
            {
                if (!(stream >> goal_i && goal_i >= 0 && goal_i < height)) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_FINY
                              << "' tag encountered (or could not convert to integer)" << std::endl;
                    std::cout << "Value of '" << CNS_TAG_FINY << "' tag should be an integer AND >=0 AND < '"
                              << CNS_TAG_HEIGHT << "' value, which is " << height << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_FINY
                              << "' tag will be encountered later..." << std::endl;
                } else
                    hasFINY = true;
            }
        } else if (value == CNS_TAG_FINZ) { // Finish Z
            if (hasFINZ) //Дубль. Финиш-игрек уже был.
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_FINZ << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_FINZ << "' =" << goal_h << "will be used." << std::endl;
            } else //Финиш-Игрек еще не встречался (или встречался, но корявый), в общем - надо пытаться инициализировать
            {
                if (!(stream >> goal_h && goal_h >= 0)) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_FINZ
                              << "' tag encountered (or could not convert to integer)" << std::endl;
                    std::cout << "Value of '" << CNS_TAG_FINZ << "' tag should be an integer AND >=0 '"
                              << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_FINZ
                              << "' tag will be encountered later..." << std::endl;
                } else
                    hasFINZ = true;
            }
        }
            //Закончили с Финиш-Зет

            //Грид
        else if (value == CNS_TAG_GRID) {
            hasGrid = true;
            if (!(hasHeight && hasWidth)) {
                std::cout << "Error! No '" << CNS_TAG_WIDTH << "' tag or '" << CNS_TAG_HEIGHT << "' tag before '"
                          << CNS_TAG_GRID << "'tag encountered!" << std::endl;
                return false;
            }
            element = mapnode->FirstChildElement(); //переходим от "грида" к "строкам грида"
            int val, grid_j;
            for (int grid_i = 0; grid_i < height; ++grid_i) {
                if (!element) {
                    std::cout << "Error! Not enough '" << CNS_TAG_ROW << "' tags inside '" << CNS_TAG_GRID << "' tag."
                              << std::endl;
                    std::cout << "Number of '" << CNS_TAG_ROW
                              << "' tags should be equal (or greater) than the value of '" << CNS_TAG_HEIGHT
                              << "' tag which is " << height << std::endl;
                    return false;
                }

                std::string str = element->GetText();
                std::vector<std::string> elems = split(str, ' ');
                grid_j = 0;
                if (elems.size() > 0)//проверка на то, что тег row не пустой
                    for (; grid_j < width && grid_j < elems.size(); ++grid_j) {
                        stream.str("");
                        stream.clear();
                        stream << elems[grid_j];
                        stream >> val;
                        Grid[grid_i][grid_j] = val;
                    }

                if (grid_j != width) {
                    std::cout << "Invalid value on " << CNS_TAG_GRID << " in the " << grid_i + 1 << " " << CNS_TAG_ROW
                              << std::endl;
                    return false;
                }

                element = element->NextSiblingElement();
            }
        }
    }
    //Закончили с гридом
    if (!hasGrid)//проверка на то, что в файле нет тега grid
    {
        std::cout << "Error! There is no tag 'grid' in xml-file!\n";
        return false;
    }
    if (!(hasFINX && hasFINY && hasSTX &&
          hasSTY))//Проверка на то, что так и не удалось считать нормальный старт и финиш.
        return false;

    if (!(hasFINZ && hasSTZ)) {
        start_h = goal_h = min_altitude_limit = max_altitude_limit = 0;
        std::cout << "Warning! Couldn't find altitude of start or finish. Map will be interpreted as a 2D.\n";
    }

    if (Grid[start_i][start_j] > start_h) {
        std::cout << "Error! Start cell is not traversable (cell's is on a altitude " << start_h
                  << ", but obstacle has height " << Grid[start_i][start_j] << ")!" << std::endl;
        return false;
    }

    if (Grid[goal_i][goal_j] > goal_h) {
        std::cout << "Error! Goal cell is not traversable (cell's is on a altitude " << goal_h
                  << ", but obstacle has height " << Grid[goal_i][goal_j] << ")!" << std::endl;
        return false;
    }

    if (start_h < min_altitude_limit || start_h > max_altitude_limit) {
        std::cout << "Error! Start cell is on forbidden altitude: " << start_h
                  << ", but allowed only alltitude between " << min_altitude_limit << " and " << max_altitude_limit
                  << ". Please change start cell's altitude or altitude limits." << std::endl;
        return false;
    }

    if (goal_h < min_altitude_limit || goal_h > max_altitude_limit) {
        std::cout << "Error! Goal cell is on forbidden altitude: " << start_h
                  << ", but allowed only alltitude between " << min_altitude_limit << " and " << max_altitude_limit
                  << ". Please change goal cell's altitude or altitude limits." << std::endl;
        return false;
    }

    return true;
}

int Map::getValue(int i, int j) const {
    if (i < 0 || i >= height)
        throw std::out_of_range("i coordinate is out of grid");

    if (j < 0 || j >= width)
        throw std::out_of_range("j coordinate is out of grid");

    return Grid[i][j];
}
