# CLAS12 Data Analysis Tutorial

This repository contains a tutorial on how to analyze data from the CLAS12 experiment. The tutorial provides step-by-step instructions and code examples to guide you through the data analysis process using the CLAS12 software framework. (For beginners, see Step 3 in [Setup](#setup) for instructions on how to clone this repository)

## Table of Contents

- [Introduction](#introduction)
- [Setup](#setup)

## Introduction

The CLAS12 detector system measures the final state products of fixed target electron-nucleon scattering. It does so using a family of detectors, each with their own unique purpose. For my research, I have only use the *forward detector* (FD for short) which corresponds to the set of CLAS12 subsystems which cover electron scattering angles of 5 to 35 degrees off the beam lines. These subsystems are listed and briefly described in the image below. For more information about the detectors, please [read more here](https://www.sciencedirect.com/science/article/pii/S0168900220300243).

![Slice of the Detector System](utils/detector_slice.jpg)

In this repository, I will be showcasing how to analyze simple inclusive DIS events. This corresponds to the process *e+p --> e' + X* where *e* and *e'* are the initial and final state electrons, *p* is the initial proton (at rest), and *X* is the unmeasured hadronic state. We will be looking at real, collected CLAS12 data from the [RG-A experiment](https://clasweb.jlab.org/wiki/index.php/Run_Group_A#tab=Current_status) which ran in the years 2018 and 2019 with a polarized electron beam and unpolarized proton target. In addition, we will be looking at Monte Carlo simulations of these very same collisions.  The end goal is to be able to calculate the event kinematics (x, Q2, y, etc.) and particle kinematics (electron energy, electron scattering angle) for each collision and produce some figures.

This tutorial is will complete this task in two separate ways. First, the **python-only** route will utilize the python package `hipopy` ([written by Matthew McEneaney](https://pypi.org/project/hipopy/)) to read the collision data, then also use python to perform some postprocessing and plotting. Second, the much faster **C++/python** route will utilize the package `clas12root` ([see documentation](https://github.com/JeffersonLab/clas12root)) to read the collision data, then use python to perform the postprocessing and plotting.

Lastly, the data we will analyze is stored in files with a .hipo extension, typically referred to as "hipo files". They are compressed in a way to save on storage space and have, at least when you first start with CLAS, a convoluted way of being read. I am hoping this tutorial will introduce the basics and give you a foundation to build off of. 

## Setup

For this tutorial, I will be assuming you have connection to Jefferson Lab's ifarm and have your own directory in `/work/clas12/users/` or some equivalent. 

1. The first step is to obtain access to a Jupyter-Notebook via the web that links to your filesystem on the farm. This is ideal for programming long scripts, creating/testing analysis macros, and viewing plots. There will be some limitations to this which will we get to later, but for now, [visit here](https://scicomp.jlab.org/docs/JupyterHub) and follow the instructions for obtaining your kernel.
When creating your kernel, use the **CLAS12** notebook image. By default, the kernel opens your home directory on the farm at `/home/<USERNAME>/`. Since this directory has a limited storage space, I would reccommend you do all your programming in your `/work` directory or some equivalent. It may be helpful to provide a quick file link to your `/work` from your default `/home` directory, this way whenever you open Jupyter-Notebook on ifarm you can open your work directory with a single click. To set this up on ifarm, do `ln -s /path/to/work/directory /home/<USERNAME>/link_to_workspace`. This will create a link called "link_to_workspace" in your home directory that you can double click while using Jupyter to enter you work directory. 

2. Next, we need to install several python packages for our analysis. Since all our python analysis will be compiled within Jupyter-Notebook, we must make sure the python packages are installed there. On your notebook page, click the blue `+` button on the top left and create a Terminal. Then, in this terminal, install the following python packages by running...
```
pip install numpy matplotlib hipopy
```
The package `hipopy` will be used for simple reading of the CLAS12 data with python, albeit slowly. 

3. Now, you will want to clone this repository somewhere in your work directory. Using either the terminal on Jupyter or when ssh'd into ifarm from your home PC, go to your work directory and run...
```
git clone https://github.com/Gregtom3/clas12_tutorial/
```

4. As mentioned earlier, we will be using C++ at some point to read the collision data faster than python could ever hope to do. Historically, software development for CLAS was done mainly in Java. The consequence of this is that you will need to install the program `clas12root` which is a sort of go-between for processing in C++. Luckily, this program is able to be accessed directly on ifarm using `module`. To do so, you will need to start by editing you `~/.cshrc` in your home directory on ifarm (this can be done using the ssh terminal). Add the following lines...
```
source /group/clas12/packages/setup.csh
module load clas12/pro

set PATH=${PATH}:${CLAS12ROOT}/bin
set RCDB_HOME=${CLAS12ROOT}/rcdb
set CCDB_HOME=${CLAS12ROOT}/ccdb
source ${CCDB_HOME}/environment.csh
set QADB=${CLAS12ROOT}/clasqaDB
```
then close and reopen your terminal. This will use the `module` command to load several programs (which can all be viewed by typing `module list`), including clas12root, every time you open your terminal. Similar to how `root` can be used to compile a C++ program and run it, `clas12root` can be used to compile a C++ program *that may be using hipo-related code* and run it ([see documentation](https://github.com/JeffersonLab/clas12root)). 

To make sure the setup worked, you should be able to type `clas12root` and receive the ROOT terminal as expected.


---

Feel free to customize and modify the template according to your specific needs. Add additional sections, code examples, or instructions as necessary to make it comprehensive and useful for the CLAS12 data analysis tutorial.
