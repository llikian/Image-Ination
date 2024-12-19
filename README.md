# Image-Ination
## Project Description
This project is a simulation of clouds, water and a terrain using OpenGL.
The terrain was done by [Killian](https://github.com/llikian).
The water was done  by [Minas](https://github.com/Minas02).
Thet clouds were done by [Raphaël](https://github.com/zbeubizbeub).

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
sudo pacman -Sy git gcc cmake make glfw-x11 libxxf86vm libxi glm
```

##### Fedora Based Distributions
```shell
sudo dnf install -y git gcc-c++ cmake make glfw-devel libXxf86vm-devel libXi-devel glm-devel
```

#### Mac OS
You can install all dependencies using [Homebrew](https://brew.sh/).

```shell
brew install git cmake make glfw glm
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

Graphics are handled with [OpenGL](https://www.opengl.org/), using the [GLAD](https://github.com/Dav1dde/glad) implementation.

Window management and event handling are done with [GLFW](https://www.glfw.org/).

Image loading is done with [stb_image](https://github.com/nothings/stb).

Mathematics functionnality, such as vectors and matrices were handled by [GLM](https://github.com/g-truc/glm).