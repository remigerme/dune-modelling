# Dune modelling project

## Install

By default, if you use HTTPS :

```shell
git clone --recurse-submodules https://github.com/remigerme/dune-modelling.git
```

Or if you use SSH (use HTTPS if you don't know what it is) :

```shell
git clone --recurse-submodules git@github.com:remigerme/dune-modelling.git
```

## Setup

Run this command once :

```shell
mkdir -p build && cd build && cmake ..
```

If the command fails, make sure you have [cmake](https://cmake.org/download/) installed on your machine.

## Run

Everytime you want to compile run this command inside the `build` directory :

```shell
make -j8
./dune-modelling
```

Or if your terminal is located in the root directory of the project :

```shell
cd build && make -j8 && ./dune-modelling
```

If you create a new `.hpp` or `.cpp` file you need to run cmake again, so inside the `build` directory :

```shell
cmake ..
```
