# h2neo
h2neo: no more thirsty babies

Last updated: August 7th, 2020

The **h2neo** project is at the University of Washington (from EIH capstone 2019-20), solving neonatal dehydration in low-resource settings. This repository contains the CCS workspace for the flow rate monitor prototype. The code is written for **MSP430F5529** LaunchPad. This development board was chosen because of its number of GPIO input pins and the convenience of prototyping. In the future, we will move the MCU with other relevant components to our own PCB. In the future, it is possible that we need to look into lower power MCUs that run at 1.8V.

Each folder in this repo is a separate CCS project.

**TODO:** 

Actually set the _h2neo_monitor_ folder as master and organize this whole repository better. Currently there was only one person working on it until now...


## Outline of the repository
The folder _h2neo_monitor_ is used as the "master branch", meaning that all the verified and tested code gets merged into this CCS project. Each folder is a CCS project. Details of each project is outlined below:
### h2neo_monitor
This CCS project contains the most up to date version of the Flow Rate Monitor prototype. Code related to LCD screen, Nokia5110, connection using SPI is included in this project. 

### opticalSensor
This CCS project contains relevant code for the optical sensor module of the monitor. ADC12 is the key module used here. 
The sensors consist of two diodes, one infrared LED and photodiode. These diodes sit in a dark enclosure around the drip chamber. 
They are oriented straight across each other. The signal is always flat except at the presence of a drop, breaking the steady beam of light. The signal has a dip which is indicated as a drop. A delay function is used to limit multiple detection of the same drop.
The analog signal is inputed to the MCU using P6.0 (ADC12). See Section 28: ADC12_A in the [User Guide](https://www.ti.com/lit/ug/slau208q/slau208q.pdf) for more details.
The source files should have comments to help understand the steps taken in the code. 

### stopWatch
This CCS project contains relevant code for timing related modules of the monitor. TimerA is the key module used here, 
A timer is (stopped, then) started at every drop of IV liquid. This project initializes the relevant clocks and timers and implements a stopwatch that forms the basis of the flow rate computation code. See Section 17: Timer_A in the [User Guide](https://www.ti.com/lit/ug/slau208q/slau208q.pdf) for more details.

### switches
This CCS project contains relevant code for user interaction peripherals, such as the rotary encoder. Various GPIO pins are the key module used here. The peripherals are currently broken and need to be fixed by debouncing the circuit/code correctly. The rotary encoder should increment when turned CW and decrement when turned CCW. The push button on the rotary encoder should register as one button press every time it is pressed. The button is used as a confirmation button to set the desired flow rate into the monitor. See Section 12: Digital I/O Module in the [User Guide](https://www.ti.com/lit/ug/slau208q/slau208q.pdf) for more details.

## Resources
Refer to the resources below to get started! Feel free to add more links under here when you find something helpful.
### Working with MSP430
- Texas Instruments MSP430 [Workshop Series](https://training.ti.com/msp430-workshop-series)

### Github setup
#### Cloning this repo
See [link](https://docs.github.com/en/github/creating-cloning-and-archiving-repositories/cloning-a-repository#cloning-a-repository-using-the-command-line) above.

#### Check out a new branch
``` $ git checkout branchName ```

#### Keep your branch up to date
``` $ git status```
This command tells you if your remote workspace is up to date with the online repository. If you had updates to make use:
```$ git pull```

#### Saving your work
In order to save and push your commits:
```
$ git add .  // add all changes (you can select the changes you want to make as well)
$ git commit -m 'some commit message'
$ git push -u origin nameOfBranch
```

#### Merge conflicts
When saving your work into a shared repository, you might have two people trying to update the same branch at the same time. See [link](https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/resolving-a-merge-conflict-on-github) above to resolve this issue. 
