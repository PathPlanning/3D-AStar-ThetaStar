# 3D-AStar-ThetaStar
Basic algorithms for grid-based 3D (map of heights) path finding.

Description
==========
This project contains implementations of the following algorithms:
>- Dijkstra's algorithm
>- A*
>- Theta*

Build and Launch
================
To build the project you can use QtCreator or CMake. Both .pro and CMakeLists.txt projects files are available in the repository. 

Your compiler must support C++ 11 standard to compile a project

Input and Output files
======================
Project uses specially structured xml files for getting task (map, search options, start and finish coordinates) and generating resulting path.

You can find examples of input and output files in _examples_ directory in repository.
####Input file should contain:
- Mandatory tag <b>\<map></b>. It describes the environment.
  * **\<height>** and **\<width>** - mandatory tags that defines size of the map. Origin is in the upper left corner. (0,0) - is upper left, (*width*-1, *height*-1) is lower right.
  * **\<maxaltitude>** - optional tag that defines the maximal height of obstacle on the map.
  * **\<altitudelimits min=a max=b/>** - optional tag that means that agent can't move higher than **b** and lower than **a**. **a** and **b** must be _ints_ **&ge; 0**.
  * **\<startx>**, **\<starty>** and **\<startz>** - mandatory tags that defines coordinates of the start location. it can pe interpreted as horizontal (X) and vertical (Y) offset from the upper left corner anh height (Z) from the ground level of start point. 0 &le; **startx** &le; **width** - 1 and 0 &le; **starty** &le; **height** - 1. If **\<altitudelimits min=a max=b/>** is specified **a** &le; **startz** &le; **b**.
  * **\<finishx>**, **\<finishy>** and **\<finishz>** - mandatory tags that defines coordinates of the goal location. The same limitations as for **\<startx>**, **\<starty>**, **\<startz>** tags.
  * **\<grid>** - mandatory tag that describes the square grid constituting the map. It consists of **\<row>** tags. Each **\<row>** contains a sequence of non negative ints separated by blanks. Each number defines height of obstacle on this cell. "0" means that cell is fully traversable.
  * **\<cellsize>** - optional tag that defines the size of one cell. One might add it to calculate scaled length of the path.
  * **\<title>**, **\<URL>**, **\<coordinates>**, etc - optional tags containing additional information on the map.
- Mandatory tag <b>\<algorithm></b>. It describes the parameters of the algorithm.
  * **\<searchtype>** - mandatory tag that defines the planner algorithm. Possible values - "astar", "theta", "bfs", "dijkstra".
  * **\<metrictype>** - defines the type of metric for heuristic function. Possible values - "euclidean", "diagonal", "manhattan", "chebyshev". Default value is "euclidean".
  * **\<hweight>** - defines the weight of heuristic function. Default value is "1".
  * **\<breakingties>** - defines the priority in OPEN list when nodes have the equal F-values. Possible values - "g-min", "g-max". Default value is "g-max".
  * **\<allowdiagonal>** - boolean tag that defines the possibility to make diagonal moves. Setting it to "false" restricts agent to make cardinal (horizonal, vertical) moves only. If Theta* algorithm is chosen, it will generate only cardinal successors during expansion of current node, but after resetting parent it will probably break this restriction. Default value is "true".
  * **\<cutcorners>** - boolean tag that defines the possibilty to make diagonal moves when one adjacent cell is untraversable. The tag is ignored if diagonal moves are not allowed. Default value is "false".
  * **\<allowsqueeze>** - boolean tag that defines the possibility to make diagonal moves when both adjacent cells are untraversable. The tag is ignored if cutting corners is not allowed. Default value is "false".
- Optional tag <b>\<options></b>. Options that are not related to search.
  * **\<loglevel>** - defines the level of detalization of log-file. Default value is "1". Possible values:
    * "0" or "none" - log-file is not created.
    * "0.5" or "tiny" - All the input data is copied to the log-file plus short **\<summary>** is appended. **\<summary>** contains info of the path length, number of steps, elapsed time, etc.
    * "1" or "short" - *0.5*-log plus **\<path>** is appended. It looks like **\<grid>** but cells forming the path are marked by "\*" instead of "0". The following tags are also appended: **\<hplevel>** and **\<lplevel>**. **\<lplevel>** is the sequence of coordinates of cells forming the path (in case Theta* planner is used, this sequence is formed at post-processing step by invoking sequantually line-of-sight procedure on path's segments). **\<hplevel>** is the sequence of sections forming the path (in case planner other from Theta* is used, sections are formed at post-processing step using naive procedure).
  * **\<logpath>** - defines the directory where the log-file should be written. If not specified directory of the input file is used. 
  * **\<logname>** - defines the name of log-file. If not specified the name of the log file is: "input file name"+"_log"+input file extension.
####Output file structure:
Output file contains the same information as input file.

Additionally it contains **\<log>** tag.

Depends on **loglevel** set in the input file **\<log>** tag may contain the next tags:

- **\<mapfilename>** - tag which defines the path to the source input file.
- **\<summary numberofsteps=(int) nodescreated=(int) length=(double) time=(double)>** - tag that contains summary of search results as tag atributes.
- **\<path>** - tag that contains the path top view. It the same structure as **\<grid>** but cells forming the path are marked by "\*".
- **\<lplevel>** - tag that defines point by point path representation. It contains the list of **\<node x=(int) y=(int) z=(int) number=(int) />** tags, where x, y, z - coordinates of the next cell in path, number - number of node in path.
- **\<hplevel>** - tag that defines section path representation. It contains the list of **\<section number=(int) start.x=(int) start.y=(int) start.z=(int) finish.x=(int) finish.y=(int) finish.z=(int) length=(double) />** tags, where start.x, start.y, start.z - coordinated of section beginning, finish.x, finish.y, finish.z - section ending, length - length of section