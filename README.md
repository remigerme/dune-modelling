# Dune modelling project

## Install

Please use SSH to clone this repository :

```shell
git clone --recurse-submodules git@github.com:remigerme/dune-modelling.git
```

Make sure you have [cmake](https://cmake.org/download/) installed on your machine (and all libs required by cgp such as `libglfw3` and stuff).

## Setup and run (using VSCode)

If you're using Linux, there is no setup. Just debug with the `Debug project with new files` configuration.

If you're using macOS, you need to replace `gdb` by `lldb` in `.vscode/launch.json`. Then debug.

## Setup and run (without VSCode)

Everytime you create a new file (and so the first time too), run this command - **inside the `build` directory** :

```shell
cmake ..
```

To compile and run the project, use this command - still inside the `build` directory :

```shell
make -j8
./dune-modelling
```

## Learn more about the project

A [blog article](https://url.remigerme.xyz/dune-modelling) will soon be available.
