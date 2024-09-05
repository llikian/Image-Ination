# Image-Ination
## Project Description
This project is

## Setup
### Dependencies
You can install the depencies with the following instructions:

#### Linux
##### Debian-Based Distributions
```shell
sudo apt install git g++ cmake make
```

#### Windows


#### Mac OS
```shell
brew install git g++ cmake make
```

### Clone Repository
```shell
git clone https://github.com/llikian/Image-Ination.git
cd Image-Ination
```

### Build
To build you can simply run the `release.sh` script at the root of the project using:
```shell
bash release.sh
```

You can also manually build it using:
```shell
cmake -B build && \
cmake --build build -j
```

Then you can run it using:
```shell
bin/Image-Ination
```

## Credits
This project was done in the context of the teaching unit of [LIFPROJET](http://cazabetremy.fr/wiki/doku.php?id=projet:presentation#enseignants)
at [University Claude Bernard Lyon 1](https://www.univ-lyon1.fr/).