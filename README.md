# Forest-Walk Project
This repository contains the code for my submission to COMP 3931. There are two progams: the L-system editor and the forest simulator. Both have been developed for Linux, however the use of cross-platform libraries means that, in theory, this project should be compilable on Windows and Mac as well.
The UI for both programs was developed in QT 5.1. You should have this installed on your machine before building.

# L-system Editor (l-system-editor)
The L-system editor, as the name suggests, is a program which allows you to create and edit L-systems. To build the L-system editor, navigate to the root directory (/forest-walk/) of the project in a terminal. Then enter the following commands:
~~~~
cd Editor
qmake
make
~~~~
To run the editor, enter the command:
~~~~
./l-system-editor
~~~~

## Creating an L-system
When first running the editor, the first screen you will see is: 

![Screenshot]{Images/editor_UI.png}

## Editing L-systems
## Generating tree models
## Production language

# Forest Simulator (forest-walk)
The forest simulator (forest-walk) is a program which procedurally generates a forest.
To build the forest simulator, navigate to the root directory of the project in a terminal. Then enter the following commands:
~~~~
cd Core
qmake
./build.sh
~~~~
After entering the final command, a prompt with the text "gen stack code?" will be displayed. Just press enter to skip past it.
To run the simulator, enter the command:
~~~~
./forest-walk
~~~~

## Simulation
## Visualisation

Implementation for COMP 3931
