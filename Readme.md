# Difficulty estimator for a platformer level

## What is it ?

This project analyzes a level of the video game Super Mario Bros to calculate metrics to estimate the difficulty of this level. It is composed of two mini-projects: [Collision_data](https://github.com/mronzon/MarioDifficulty/tree/main/Collision_Data) and [Challenge](https://github.com/mronzon/MarioDifficulty/tree/main/ChallengeSimon).

`Collision_data` allows, by taking as argument the path to the .png of the level, to extract static elements, platforms, pipes and enemies and to save them in a json. 

`Challenge` allows, taking as argument the path to a json generated by `Collision_data`, to produce a difficulty metric.

## Realization

This project was realized in C++ language using the Visual Studio 2022 IDE. Moreover, we used the library [opencv](https://opencv.org/) for the recognition of images in an image and the library [jsoncons](https://github.com/danielaparker/jsoncons) for the passage from json to C++.

## Get Started

To use the two mini-projects, you need to install the opencv and jsoncons libraries using the vcpkg library manager. After that, please read the readme of the different projects to know their specificity.

## Author

- [Jules Linard](https://github.com/JulesLinard)
- [Mathis Ronzon](https://github.com/mronzon)

## Acknowledgment

This project is based on the work done by Simon NOMDEFAMILLE, who has already implemented a bass of `Collision_data` and coded `Challenge`.
Thanks also to UQAC who allow us to work on this subject and to Yannick Francillette and Hugo Tremblay who are our supervisors.

## Licence Info 
[MIT](https://github.com/mronzon/MarioDifficulty/blob/main/LICENSE)