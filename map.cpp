#include "map.h"
#include "tinyxml2.h"

#include <climits>
#include <vector>
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
    altitude = 0;
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
    tinyxml2::XMLElement *root = 0, *map = 0, *element = 0, *mapnode;

    std::string value; //èìÿ òåãà
    std::stringstream stream; //ñîäåðäèìîå (òåêñò) òåãà

    bool hasGridMem = false, hasGrid = false, hasHeight = false, hasWidth = false, hasSTX = false, hasSTY = false;
    bool hasSTZ = false, hasFINX = false, hasFINY = false, hasFINZ = false, hasSTH = false, hasFINH = false;
    bool hasMAXALT = false, hasALTLIM = false;

    tinyxml2::XMLDocument doc;

    // Load XML File
    if (doc.LoadFile(FileName) != tinyxml2::XMLError::XML_SUCCESS) {
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

    //Èäåîëîãèÿ èíèöèàëèçàöèÿ "êàðòû":
    //Ïåðåáèðàåì ïîñëåäîâàòåëüíî âíóòðåííîñòè òåãà "êàðòà"
    //Åñëè âïåðâûå ïîïàëîñü êîððåêòíîå çíà÷åíèå ñòàðòà (õ, ó), ôèíèøà (õ, ó), âûñîòû è øèðèíû - çàïîíèìàåì èõ
    //Åñëè ñòàðò-õ, ôèíèø-õ ïîïàäàþòñÿ ðàíüøå øèðèíû - îøèáêà!
    //Åñëè ñòàðò-ó, ôèíèø-ó ïîïàäàþòñÿ ðàíüøå âûñîòû - îøèáêà!
    //Åñëè ïîïàäàþòñÿ äóáëèêàòû (òåãè ïîâòîðÿþòñÿ), ïîñëå òîãî êàê ìû óæå çàïîìíèëè çíà÷åíèÿ, - âûâîäèì Warning
    //ò.å. ÂÑÅÃÄÀ çàïîìèíàåòñÿ è èñïîëüçóåòñÿ ÒÎËÜÊÎ ïåðâîå (êîððåêòíî îïðåäåëåííîå) çíà÷åíèå ñòàðòà (õ, ó), ôèíèøà (õ, ó), âûñîòû, øèðèíû


    for (mapnode = map->FirstChildElement(); mapnode; mapnode = mapnode->NextSiblingElement()) {
        element = mapnode->ToElement(); //î÷åðåäíîé ýëåìåíò (òåã)
        value = mapnode->Value(); // èìÿ òåãà
        std::transform(value.begin(), value.end(), value.begin(), ::tolower); //"õîðîøåå èìÿ òåãà"

        stream.str(""); //î÷èùàåì
        stream.clear(); //áóôåð
        stream
                << element->GetText(); //è êëàäåì â íåãî ñîäåðæèìîå (òåêñò) òåãà (âíèìàíèå: â òåãå "ãðèä" íåò òåêñòà - òàì âëîæåííûå òåãè, òàì áóäåò NULL. Òàê ÷òî áåñïîêîèòñÿ íå íàäî, âñ¸ ïîä êîíòðîëåì!

        //1. Çàãîòîâêà ïîä ãðèä.
        if (!hasGridMem && hasHeight &&
            hasWidth) //â êàêîé-òî ìîìåíò îêàçàëîñü, ÷òî åñòü óæå âûñîòà è øèðèíà - ñîçäàåì ãðèä
        {
            Grid = new int *[height];
            for (int i = 0; i < height; i++)
                Grid[i] = new int[width];
            hasGridMem = true;
        }

        //2. Âûñîòà
        if (value == CNS_TAG_HEIGHT) {
            if (hasHeight) //Äóáëü. Âûñîòà óæå áûëà.
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_HEIGHT << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_HEIGHT << "' =" << height << "will be used."
                          << std::endl;
            } else//Âûñîòû åùå íå áûëî. Èëè áûëà - íî ïëîõàÿ. Â îáùåì - ïîêà íå èíèöèàëèçèðîâàëè...
            {
                if (!((stream >> height) && (height > 0)))//sstream >> height ïðåîáðàçîâûâàåò ñòðîêó â ÷èñëî (int).
                    //Åñëè íå ìîæåò ïðåîáàçîâàòü (ñòðîêà êîñÿ÷íàÿ) - çíà÷åíèå height îñòàåòñÿ ïðåäûäóùèì.
                    //Òî åñòü - ïî óìîë÷àíèþ (èç êîíñòðóêòîðà) 0.
                    //Òî æå ñàìîå êàñàåòñÿ è äðóãèõ ïðåîáðàçîâàíèé íèæå...
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
            //Çàêîí÷èëè ñ âûñîòîé


            //3. Øèðèíà
        else if (value == CNS_TAG_WIDTH) {
            if (hasWidth) //Äóáëü. Øèðèíà óæå áûëà.
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_WIDTH << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_WIDTH << "' =" << width << "will be used." << std::endl;
            } else //Øèðèíû åùå íå áûëî. Èëè áûëà - íî ïëîõàÿ. Â îáùåì - ïîêà íå èíèöèàëèçèðîâàëè...
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
            //Çàêîí÷èëè ñ øèðèíîé

        else if (value == CNS_TAG_MAXALT) {
            if (hasMAXALT) //Äóáëü. Ìàêñèìàëüíàÿ âûñîòà êàðòû óæå áûëà
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_MAXALT << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_MAXALT << "' =" << altitude << "will be used."
                          << std::endl;
            } else {
                if (!((stream >> altitude) && (altitude >= 0))) {
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
            if (hasALTLIM) //Äóáëü. Îãðàíè÷åíèÿ íà âûñîòó
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_ALTLIM << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_ALTLIM << "will be used." << std::endl;
            } else {
                hasALTLIM = true;
                if (!((element->QueryAttribute(CNS_TAG_ALTLIM_ATTR_MIN, &min_altitude_limit)) && (min_altitude_limit >= 0))) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_ALTLIM_ATTR_MIN << "' attribute of '"
                              << CNS_TAG_ALTLIM << "' tag encountered (or could not convert to integer)." << std::endl;
                    std::cout << "Value of '" << CNS_TAG_ALTLIM_ATTR_MIN << "' tag should be an integer AND >=0"
                              << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_ALTLIM_ATTR_MIN
                              << "' tag will be encountered later..." << std::endl;
                    hasALTLIM = false;
                }
                if (!((element->QueryAttribute(CNS_TAG_ALTLIM_ATTR_MAX, &max_altitude_limit)) &&
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

            //4. Ñòàðò-Èêñ
        else if (value == CNS_TAG_STX) {
            if (!hasWidth) //Òåã "ñòàðò-õ" âñòðåòèëñÿ ðàíüøå ÷åì "øèðèíà" - îøèáêà!
            {
                std::cout << "Error! '" << CNS_TAG_STX << "' tag encountered before '" << CNS_TAG_WIDTH << "' tag."
                          << std::endl;
                return false;
            }

            if (hasSTX) //Äóáëü. Ñòàðò-ÈÊÑ óæå áûë.
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_STX << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_STX << "' =" << start_j << "will be used." << std::endl;
            } else //Ñòàðò-ÈÊÑ åùå íå âñòðå÷àëñÿ (èëè âñòðå÷àëñÿ, íî êîðÿâûé), â îáùåì - íàäî ïûòàòüñÿ èíèöèàëèçèðîâàòü
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
            //Çàêîí÷èëè ñî Còàðò-Èêñ


            //5. Ñòàðò-Èãðåê
        else if (value == CNS_TAG_STY) {
            if (!hasHeight) //Òåã "ñòàðò-y" âñòðåòèëñÿ ðàíüøå ÷åì "âûñîòà" - îøèáêà!
            {
                std::cout << "Error! '" << CNS_TAG_STY << "' tag encountered before '" << CNS_TAG_HEIGHT << "' tag."
                          << std::endl;
                return false;
            }

            if (hasSTY) //Äóáëü. Ñòàðò-ÈÊÑ óæå áûë.
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_STY << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_STY << "' =" << start_i << "will be used." << std::endl;
            } else //Ñòàðò-Èãðåê åùå íå âñòðå÷àëñÿ (èëè âñòðå÷àëñÿ, íî êîðÿâûé), â îáùåì - íàäî ïûòàòüñÿ èíèöèàëèçèðîâàòü
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
            //Çàêîí÷èëè ñî Còàðò-Èãðåê

            // Start Z
        else if (value == CNS_TAG_STZ) {
            if (hasSTZ) //Äóáëü. Ñòàðò-çåò óæå áûë.
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_STZ << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_STZ << "' =" << start_h << "will be used." << std::endl;
            } else //Ñòàðò-çåò åùå íå âñòðå÷àëñÿ (èëè âñòðå÷àëñÿ, íî êîðÿâûé), â îáùåì - íàäî ïûòàòüñÿ èíèöèàëèçèðîâàòü
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

            //Ôèíèø-Èêñ
        else if (value == CNS_TAG_FINX) {
            if (!hasWidth) //Òåã "ôèíèø-èêñ" âñòðåòèëñÿ ðàíüøå ÷åì "øèðèíà" - îøèáêà!
            {
                std::cout << "Error! '" << CNS_TAG_FINX << "' tag encountered before '" << CNS_TAG_WIDTH << "' tag."
                          << std::endl;
                return false;
            }

            if (hasFINX) //Äóáëü. ôèíèø-èêñ óæå áûë.
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_FINX << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_FINX << "' =" << goal_j << "will be used." << std::endl;
            } else //Ôèíèø-èêñ åùå íå âñòðå÷àëñÿ (èëè âñòðå÷àëñÿ, íî êîðÿâûé), â îáùåì - íàäî ïûòàòüñÿ èíèöèàëèçèðîâàòü
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
            //Çàêîí÷èëè ñî Ôèíèø-Èêñ


            //Ôèíèø-Èãðåê
        else if (value == CNS_TAG_FINY) {
            if (!hasHeight) //Òåã "ôèíèø-èãðåê" âñòðåòèëñÿ ðàíüøå ÷åì "âûñîòà" - îøèáêà!
            {
                std::cout << "Error! '" << CNS_TAG_FINY << "' tag encountered before '" << CNS_TAG_HEIGHT << "' tag."
                          << std::endl;
                return false;
            }

            if (hasFINY) //Äóáëü. Ôèíèø-èãðåê óæå áûë.
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_FINY << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_FINY << "' =" << goal_i << "will be used." << std::endl;
            } else //Ôèíèø-Èãðåê åùå íå âñòðå÷àëñÿ (èëè âñòðå÷àëñÿ, íî êîðÿâûé), â îáùåì - íàäî ïûòàòüñÿ èíèöèàëèçèðîâàòü
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
            if (hasFINZ) //Äóáëü. Ôèíèø-èãðåê óæå áûë.
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_FINZ << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_FINZ << "' =" << goal_h << "will be used." << std::endl;
            } else //Ôèíèø-Èãðåê åùå íå âñòðå÷àëñÿ (èëè âñòðå÷àëñÿ, íî êîðÿâûé), â îáùåì - íàäî ïûòàòüñÿ èíèöèàëèçèðîâàòü
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
            //Çàêîí÷èëè ñ Ôèíèø-Çåò

            //Ãðèä
        else if (value == CNS_TAG_GRID) {
            hasGrid = true;
            if (!(hasHeight && hasWidth)) {
                std::cout << "Error! No '" << CNS_TAG_WIDTH << "' tag or '" << CNS_TAG_HEIGHT << "' tag before '"
                          << CNS_TAG_GRID << "'tag encountered!" << std::endl;
                return false;
            }
            element = mapnode->FirstChildElement(); //ïåðåõîäèì îò "ãðèäà" ê "ñòðîêàì ãðèäà"
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
                if (elems.size() > 0)//ïðîâåðêà íà òî, ÷òî òåã row íå ïóñòîé
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
    //Çàêîí÷èëè ñ ãðèäîì
    if (!hasGrid)//ïðîâåðêà íà òî, ÷òî â ôàéëå íåò òåãà grid
    {
        std::cout << "Error! There is no tag 'grid' in xml-file!\n";
        return false;
    }
    if (!(hasFINX && hasFINY && hasSTX &&
          hasSTY))//Ïðîâåðêà íà òî, ÷òî òàê è íå óäàëîñü ñ÷èòàòü íîðìàëüíûé ñòàðò è ôèíèø.
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
        std::cout << "Error! Goal cell is on forbidden altitude: " << goal_h
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
