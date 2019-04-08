# food-tracker
## Features
Food tracker is a utility that works with the [USDA Nutrient Database (NDB)](https://ndb.nal.usda.gov/ndb/). With it you can:
* Search for foods and view their nutritional info
* Save foods to a user file
* Load user files and view stats about saved foods
* Search for foods within user files and view their nutritional info

Here is a video demonstraton:

<a href="https://www.youtube.com/watch?v=CvBNrDLogWE"><img src="https://img.youtube.com/vi/CvBNrDLogWE/maxresdefault.jpg" alt="Demo Video" width="512" height="288" border="10" /></a>

## Requirements
* ncurses
  ```
  sudo apt install libncurses5-dev
  ```

## Setup
1. Clone or download this repository to your computer
2. Open a terminal and change into `food-tracker\`
3. Run `./setupDatabase.sh`. This will download the USDA database to `build/`
4. Change into `build/` and run `cmake ..`. This will setup the directory for compiling
5. Finally, run `make` to compile `food-tracker`

## Usage
From the `build/` directory, run `./food-tracker`
