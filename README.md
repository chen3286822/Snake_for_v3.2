3D Snake Game(Developing)
=============
***
<i>(last updated 2014-10-08)</i>

Overview
--------
***
This project uses cocos2d-x v3.2 engine to build.

The purpose of building this project is just to test some 3D properties of the new engine version.

Gameplay
--------
***
The game just has one mode for now, the adverture mode. 
In this mode player plays the levels one by one.

In a level, player uses arrow keys to control the snake's moving in the windows platform, 
or just slides the screen to control it in the android version.

There are some functional items to increase the playability:

1.  <B>Block</B>, just to prevent the snake from moving across it.
2.  <B>Food</B>, will increase the body's length.
3.  <B>Transform Door</B>, always appears in pairs, snake crosses one and emerges from the other door.
4.  <B>Apple</B>, snake eats it will gain the points.
5.  <B>Star</B>, it will speed up the snake and increase the points acquisition rate.
6.  <B>Ball</B>, slow down the snake for a while.

Player should control the snake eat food and items to achieve the level's target. 
Remember, some items should be avoided.

Code Main Points
----------------
***
The main title, selecting level page and playing page implemented in different scenes will be easier to change.

The resources should be loaded in

    onEnterTransitionDidFinish()

as many as possible. Because this function will be called after last scene's resources released. 
It will reduce the crash risk by out of memory on mobile devies.

All the items are inherited from a base item class, 
so it is easily managed by an item factory. It will be convenient to control all the items' 'born' and 'death' and effect.

Every level has a specific profile, it is a json file. 
The game will load and parse it before player can play. 
The file predefines the victory target and some items like blocks, doors.

The game will save player's records in a sqlite database file. 
The game will also read a player's data when he\she begins to play.

The project uses TableView control to implement the selecting level scene. 
By adding some actions to the TableViewCell, the levels can slide into player's signt one by one.

Because the engine doesn's support 3D spirit rotate by a specific point,
the project uses a spawn action that contains a moving and a rotation action instead.

Problems Left
-------------
***
The level editor has not been written yet.

When the snake rotate, the body looks like breaking up.
