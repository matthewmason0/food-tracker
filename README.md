# food-tracker
## Features
Food tracker is a utility that works with the [USDA Nutrient Database (NDB)](https://ndb.nal.usda.gov/ndb/). With it you can:
* Search for foods and view their nutritional info
* Save foods to a user file
* Load user files and view stats about saved foods
* Search for foods within user files and view their nutritional info

Here is a video demonstraton:

<a href="https://youtube.com/watch?feature=player_embedded&v=WKB0JUkksJg
" target="_blank"><img src="https://img.youtube.com/vi/WKB0JUkksJg/maxresdefault.jpg" 
alt="Demo Video" width="512" height="288" border="10" /></a>

## Setup
1. Clone or download this repository to your computer
2. Open a terminal and change into `food-tracker\`
3. Run `./setup.sh`. This will create the build directory and download the USDA database
4. Change into `build/` and run `make`. This will compile `food-tracker`

## Usage
From the `build/` directory, run `./food-tracker`
