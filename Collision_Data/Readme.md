# Detection de collisions dans un niveau

This project consists in creating a json file containing all the static or dynamic elements that compose a level of Super Mario Bros. The elements that we take into account are all the elements that can interact with the player, be it enemies or blocks. The power ups are not detected because we decided to ignore them to focus on the motor difficulty of the level and the enemies.


## Get started

It is necessary that both libraries `jsoncons` and `opencv` are well installed on your computer. Then you have to give as argument to the program the absolute path of the folder that contains all the elements it will have to use. This folder contains :
- a folder containing the level to analyse. The image of the level has to be renamed to `level.png`.
- a folder named `Sprite` which contains all the sprites of the different elements to detect in the level. It is composed of several subfolders that allow to split the different sprites in several categories.

In this project the resources folder is the one we use to run the program.

When the program is finished, it will create a json file names `level.json` that is saved in the same folder that contains `level.png`.

## How it works

The program uses the opencv method `matchTemplate` to find all the similarities between the level and a sprite. We save the places where the similarities are the highest in the json file. Moreover, when the elements allow it, we try to group them in the largest rectangle possible. To do this, we first group them horizontally and then vertically.

Moreover, it also allows you to detect pipe inputs and outputs. However, you have to link them in the json file by hand.

The platforms are automatically detected and linked.

In order to make our program work we had to make some modifications on the levels. However, all these modifications were only made on the scenery elements and we did not touch the important elements of the level. This means that we have for example removed the black background on some levels, or we have removed the pipes allowing to teleport to another world at the end of level 1-2 for example.