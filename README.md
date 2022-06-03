# h2neo
h2neo: no more thirsty babies

Last updated: June 3rd, 2022

The **h2neo** project is at the University of Washington (from EIH capstone 2019-20), solving neonatal dehydration in low-resource settings. This repository contains the CCS and arduino workspace for the flow rate monitor prototype. The code is written for an Arduino MCU. This development board was chosen because of its number of GPIO input pins and the convenience of prototyping. In the future, we will move the MCU with other relevant components to our own PCB. In the future, it is possible that we need to look into lower power MCUs that run at 1.8V.

**TODO:** 

Actually set the _h2neo_monitor_ folder as master and organize this whole repository better. Currently there was only one person working on it until now...

## Outline of the repository
The folder _h2neo_monitor_ is used as the "master branch", meaning that all the verified and tested code gets merged into this CCS project. Each folder is a CCS project. Details of each project is outlined below:

### h2neo_monitor
This CCS project contains the most up to date version of the Flow Rate Monitor prototype. 

### opticalSensor

### stopWatch

### userInteraction

## Resources

### Github setup
#### Cloning this repo
See [link](https://docs.github.com/en/github/creating-cloning-and-archiving-repositories/cloning-a-repository#cloning-a-repository-using-the-command-line) above.

#### Check out a new branch
``` $ git checkout branchName ```

#### Keep your branch up to date
``` $ git status```
This command tells you if your remote workspace is up to date with the online repository. If you had updates to make use:
``` $ git pull```

#### Saving your work
In order to save and push your commits:
```
$ git add .  // add all changes (you can select the changes you want to make as well)
$ git commit -m 'some commit message'

```
#### (if first time using branch) Connect local branch to remote branch, then push:
``` $ git push --set-upstream origin branchName ```

#### (if local and remote branch are connected) Push changes:
``` $ git push ```

#### Create pull request on Github from branch into master:
Ask other team members to review and merge code

#### Merge conflicts
When saving your work into a shared repository, you might have two people trying to update the same branch at the same time. See [link](https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/resolving-a-merge-conflict-on-github) above to resolve this issue. 
