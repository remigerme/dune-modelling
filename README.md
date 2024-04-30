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

## Run

Everytime you want to compile run this command inside `build` directory :

```shell
make -j8
./dune-modelling
```

Or if your terminal is located in the root directory of the project :

```shell
cd build && make -j8 && ./dune-modelling
```
