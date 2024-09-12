# Image-Ination
## Project Description
This project is a simulation of clouds and water using OpenGL.

## Setup
### Dependencies
You can install the dependencies with the following instructions:

#### Linux
##### Debian Based Distributions
```shell
sudo apt install -y git g++ cmake make libglfw3-dev libxxf86vm-dev libxi-dev libglm-dev
```

##### Arch Based Distributions
```shell
sudo pacman -Sy git g++ cmake make
```

##### Fedora Based Distributions
```shell
sudo dnf install git g++ cmake make
```

#### Windows


#### Mac OS
You can install all dependencies using [Homebrew](https://brew.sh/).

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