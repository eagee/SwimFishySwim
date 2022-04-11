SwimFishySwim
=============

Swim Fishy Swim is a now defunct Wii Homebrew project (back when everything was 4:3 and Unity3d was only on Macs). The project compiles to a rom capable of running in an SD card on the wii the XYZ toolchain.



Footage of the actual (admittedly rather odd but fun) gameplay can be found here:
[![Swim fishy swim screenshot](https://img.youtube.com/vi/giIlyl0D9iA/0.jpg)](https://youtu.be/giIlyl0D9iA)


SFS Games Design Document (GDD)
===============================
Below was once a media wiki document that served as the general design doc for the game. It was hacked somewhere in 2014 and I was only able to restore this version from internet archives:


### From Sfswiki

Jump to: [navigation](#column-one), [search](#searchInput)

* * *

Green Door Games, LLC Software Engineering Document

* * *

Functional, Requirement, Analysis and Game Design Document: **Swim Fishy Swim (tm)**

Contents
--------

*   [1 Document Contribution:](#Document_Contribution:)
*   [2 Document Revision:](#Document_Revision:)
*   [3 Introduction:](#Introduction:)
    *   [3.1 Background (why we are doing this):](#Background_.28why_we_are_doing_this.29:)
    *   [3.2 Scope:](#Scope:)
    *   [3.3 Audience:](#Audience:)
    *   [3.4 Conventions:](#Conventions:)
    *   [3.5 Game Story:](#Game_Story:)
*   [4 Swim Fishy Swim (SLTN001):](#Swim_Fishy_Swim_.28SLTN001.29:)
    *   [4.1 Solution Use Case Diagram:](#Solution_Use_Case_Diagram:)
    *   [4.2 Solution Actors:](#Solution_Actors:)
    *   [4.3 Solution Systems:](#Solution_Systems:)
*   [5 Title Screen/Main Menu (SYS1):](#Title_Screen.2FMain_Menu_.28SYS1.29:)
    *   [5.1 Use Case Diagram:](#Use_Case_Diagram:)
    *   [5.2 Requirements:](#Requirements:)
    *   [5.3 Actors:](#Actors:)
    *   [5.4 System Use Cases:](#System_Use_Cases:)
        *   [5.4.1 Player Views Title Screen (SYS1-UC001):](#Player_Views_Title_Screen_.28SYS1-UC001.29:)
        *   [5.4.2 Player Watches Story (SYS1-UC002):](#Player_Watches_Story_.28SYS1-UC002.29:)
        *   [5.4.3 Player Starts New Game (SYS1-UC003):](#Player_Starts_New_Game_.28SYS1-UC003.29:)
        *   [5.4.4 Player Continues Game (SYS1-UC004):](#Player_Continues_Game_.28SYS1-UC004.29:)
    *   [5.5 Mockups:](#Mockups:)
        *   [5.5.1 Title Screen:](#Title_Screen:)
        *   [5.5.2 Story (Storyboards in video format):](#Story_.28Storyboards_in_video_format.29:)
        *   [5.5.3 Tutorial Screens:](#Tutorial_Screens:)
*   [6 Game Progress / Level Details Screen (SYS2):](#Game_Progress_.2F_Level_Details_Screen_.28SYS2.29:)
    *   [6.1 Use Case Diagram:](#Use_Case_Diagram:_2)
    *   [6.2 Requirements:](#Requirements:_2)
    *   [6.3 Actors:](#Actors:_2)
    *   [6.4 System Use Cases:](#System_Use_Cases:_2)
        *   [6.4.1 Player views Game Progress (SYS2-UC001):](#Player_views_Game_Progress_.28SYS2-UC001.29:)
    *   [6.5 Mockups:](#Mockups:_2)
        *   [6.5.1 Progress Screen:](#Progress_Screen:)
*   [7 Towers (Flowers) and their Behaviors (SYS3):](#Towers_.28Flowers.29_and_their_Behaviors_.28SYS3.29:)
    *   [7.1 Use Case Diagram:](#Use_Case_Diagram:_3)
    *   [7.2 Requirements:](#Requirements:_3)
    *   [7.3 System Use Cases:](#System_Use_Cases:_3)
        *   [7.3.1 Windy Tower Interacts w/ Baddies: (SYS3-UC001)](#Windy_Tower_Interacts_w.2F_Baddies:_.28SYS3-UC001.29)
        *   [7.3.2 Thorny Tower interacts w/ Baddies (SYS3-UC002) :](#Thorny_Tower_interacts_w.2F_Baddies_.28SYS3-UC002.29_:)
        *   [7.3.3 Plompy Tower interacts w/ Baddies (SYS3-UC003) :](#Plompy_Tower_interacts_w.2F_Baddies_.28SYS3-UC003.29_:)
        *   [7.3.4 Ptewey Tower interacts w/ Baddies (SYS3-UC004) :](#Ptewey_Tower_interacts_w.2F_Baddies_.28SYS3-UC004.29_:)
    *   [7.4 Mockups:](#Mockups:_3)
        *   [7.4.1 Windy Towers:](#Windy_Towers:)
        *   [7.4.2 Thorny Towers (with Windy Towers):](#Thorny_Towers_.28with_Windy_Towers.29:)
        *   [7.4.3 Petewy Towers:](#Petewy_Towers:)
        *   [7.4.4 Plompy Towers:](#Plompy_Towers:)
*   [8 Baddie Behaviors (SYS4):](#Baddie_Behaviors_.28SYS4.29:)
    *   [8.1 Use Case Diagram:](#Use_Case_Diagram:_4)
    *   [8.2 Requirements:](#Requirements:_4)
    *   [8.3 System Use Cases:](#System_Use_Cases:_4)
    *   [8.4 Mockups:](#Mockups:_4)
*   [9 Weapons/Attacks and their Behaviors (SYS5):](#Weapons.2FAttacks_and_their_Behaviors_.28SYS5.29:)
    *   [9.1 Use Case Diagram:](#Use_Case_Diagram:_5)
    *   [9.2 Requirements:](#Requirements:_5)
    *   [9.3 System Use Cases:](#System_Use_Cases:_5)
        *   [9.3.1 Fires Swigen Cannon (SYS5-UC001):](#Fires_Swigen_Cannon_.28SYS5-UC001.29:)
        *   [9.3.2 Arms Dŵr Bomb (SYS5-UC002):](#Arms_D.C5.B5r_Bomb_.28SYS5-UC002.29:)
        *   [9.3.3 Triggers Bubble Wal (SYS5-UC003):](#Triggers_Bubble_Wal_.28SYS5-UC003.29:)
        *   [9.3.4 Releases Carterfi Fishy (SYS5-UC004):](#Releases_Carterfi_Fishy_.28SYS5-UC004.29:)
    *   [9.4 Mockups:](#Mockups:_5)
*   [10 Tower Building Phase (SYS6):](#Tower_Building_Phase_.28SYS6.29:)
    *   [10.1 SYS6 - Use Case Diagram:](#SYS6_-_Use_Case_Diagram:)
    *   [10.2 SYS6 - Requirements:](#SYS6_-_Requirements:)
    *   [10.3 SYS6: System Use Cases:](#SYS6:_System_Use_Cases:)
        *   [10.3.1 Collects Power-ups and Grows Flowers: (SYS6-UC001)](#Collects_Power-ups_and_Grows_Flowers:_.28SYS6-UC001.29)
        *   [10.3.2 Talks to Pickle, Chooses Weapon (SYS6-UC002):](#Talks_to_Pickle.2C_Chooses_Weapon_.28SYS6-UC002.29:)
        *   [10.3.3 Mockups:](#Mockups:_6)
        *   [10.3.4 Phase Overview:](#Phase_Overview:)
        *   [10.3.5 Hero, Choose your Weapon:](#Hero.2C_Choose_your_Weapon:)
*   [11 Tower Defense Phase (SYS7):](#Tower_Defense_Phase_.28SYS7.29:)
    *   [11.1 SYS7 - Use Case Diagram:](#SYS7_-_Use_Case_Diagram:)
    *   [11.2 SYS7 - Requirements:](#SYS7_-_Requirements:)
    *   [11.3 SYS7: System Use Cases:](#SYS7:_System_Use_Cases:)
        *   [11.3.1 Upgrades Towers as points are available (SYS7-UC001):](#Upgrades_Towers_as_points_are_available_.28SYS7-UC001.29:)
        *   [11.3.2 Attempts to stop or hamper baddies from reaching pickle (SYS7-UC002):](#Attempts_to_stop_or_hamper_baddies_from_reaching_pickle_.28SYS7-UC002.29:)
        *   [11.3.3 Mockups:](#Mockups:_7)
        *   [11.3.4 Phase Overview:](#Phase_Overview:_2)
*   [12 Level End Phase (SYS8):](#Level_End_Phase_.28SYS8.29:)
*   [13 DEPRECATED - Flower Upgrades Screen (SYS9):](#DEPRECATED_-_Flower_Upgrades_Screen_.28SYS9.29:)
*   [14 End of Wave Upgrades Screen (SYS10):](#End_of_Wave_Upgrades_Screen_.28SYS10.29:)
*   [15 Level 1-1 Pysgode River (Lands of Brine) (SYS11):](#Level_1-1_Pysgode_River_.28Lands_of_Brine.29_.28SYS11.29:)
*   [16 Level 1-2 Lake Briney (Lands of Brine) (SYS12):](#Level_1-2_Lake_Briney_.28Lands_of_Brine.29_.28SYS12.29:)
*   [17 Level 1-3 Cornichon Sound (Lands of Brine) (SYS13):](#Level_1-3_Cornichon_Sound_.28Lands_of_Brine.29_.28SYS13.29:)
*   [18 Level 2-1 Nibble Island (Sandy Salty Desert) (SYS14):](#Level_2-1_Nibble_Island_.28Sandy_Salty_Desert.29_.28SYS14.29:)
*   [19 Level 2-2 Barren Wind Isle (Sandy Salty Desert) (SYS15):](#Level_2-2_Barren_Wind_Isle_.28Sandy_Salty_Desert.29_.28SYS15.29:)
*   [20 Level 2-3 Little Pickle Rock (Sandy Salty Desert) (SYS16):](#Level_2-3_Little_Pickle_Rock_.28Sandy_Salty_Desert.29_.28SYS16.29:)
*   [21 Level 3-1 Scraggly Skiffle (Frozen Sepul Coral Wasteland) (SYS17):](#Level_3-1_Scraggly_Skiffle_.28Frozen_Sepul_Coral_Wasteland.29_.28SYS17.29:)
*   [22 Level 3-2 Ragged Range (Frozen Sepul Coral Wasteland) (SYS18):](#Level_3-2_Ragged_Range_.28Frozen_Sepul_Coral_Wasteland.29_.28SYS18.29:)
*   [23 Level 3-3 Mount Echinoderm (Frozen Sepul Coral Wasteland) (SYS19):](#Level_3-3_Mount_Echinoderm_.28Frozen_Sepul_Coral_Wasteland.29_.28SYS19.29:)
*   [24 Weapons/Attacks and their Behaviors (APPENDIX-A):](#Weapons.2FAttacks_and_their_Behaviors_.28APPENDIX-A.29:)
    *   [24.1 Swigen Cannon](#Swigen_Cannon)
        *   [24.1.1 Purchase/Upgrade](#Purchase.2FUpgrade)
        *   [24.1.2 Levels](#Levels)
        *   [24.1.3 Costs](#Costs)
    *   [24.2 Dŵr Bomb](#D.C5.B5r_Bomb)
        *   [24.2.1 Purchase/Upgrade](#Purchase.2FUpgrade_2)
        *   [24.2.2 Levels](#Levels_2)
        *   [24.2.3 Costs](#Costs_2)
    *   [24.3 Bubble Wal](#Bubble_Wal)
        *   [24.3.1 Purchase/Upgrade](#Purchase.2FUpgrade_3)
        *   [24.3.2 Levels](#Levels_3)
        *   [24.3.3 Costs](#Costs_3)
    *   [24.4 Carterfi Fishy](#Carterfi_Fishy)
        *   [24.4.1 Purchase/Upgrade](#Purchase.2FUpgrade_4)
        *   [24.4.2 Levels](#Levels_4)
        *   [24.4.3 Costs](#Costs_4)
*   [25 Baddies and their Behaviors (APPENDIX-B):](#Baddies_and_their_Behaviors_.28APPENDIX-B.29:)
    *   [25.1 L1 Baddies: (Levels 1-1 to 1-3)](#L1_Baddies:_.28Levels_1-1_to_1-3.29)
    *   [25.2 L1 Boss: (Level 1-3)](#L1_Boss:_.28Level_1-3.29)
    *   [25.3 L2 Baddies: (Levels 2-1 to 2-3)](#L2_Baddies:_.28Levels_2-1_to_2-3.29)
    *   [25.4 L2 Boss: (Level 2-3)](#L2_Boss:_.28Level_2-3.29)
    *   [25.5 L3 Baddies: (Levels 3-1 to 3-3)](#L3_Baddies:_.28Levels_3-1_to_3-3.29)
    *   [25.6 L3 Boss: (Level 3-3)](#L3_Boss:_.28Level_3-3.29)
*   [26 Towers Upgrade Options (APPENDIX-C):](#Towers_Upgrade_Options_.28APPENDIX-C.29:)

if (window.showTocToggle) { var tocShowText = "show"; var tocHideText = "hide"; showTocToggle(); }

Document Contribution:
----------------------

**Prepared By:** Eagan Rackley

**Title:** Game Designer and Software Engineer

**Company:** Green Door Games, LLC

Document Revision:
------------------

**Created on:** 10/3/2010

**Version:** 0.5

**Approved:** No

Introduction:
-------------

### Background (why we are doing this):

Possessing the skill set and experience in requisite parts to make what we believe could be great games, the Green Door Games team has decided to embark on a test of those skills. Specifically, to author and complete a video game, and (if possible) release it on one or more platforms.

Aside from a deep love of video games and the desire to create, the major motivation for this project stems from a "Desire to continue living". This motivation may seem confusing until one attempts any major home repair project with our lead developer. Therefore the specific metric for success of this project has been specified by his spouse as, "Make enough money to re-do the kitchen, and you'll never have to do a major home repair again."

All joking aside, the true goal of this and future projects is to create a company that breaks the norm of the industry.

The long term goal of this project is to create a company that focuses on fun, creativity, stability, social responsibility, and community involvement (e.g. we'd like our company to make enough money to allow us to do some volunteer work teaching inner city kids how to write software. The hope of this particular goal is that the encouragement of software development skills (not necessarily games, but games as a draw for young, bright minds) in the hopes that such a low overhead skill can lead to life and community changing experiences).

### Scope:

The scope of this document should be limited to high level design decisions associated with Swim Fishy Swim. This includes general level design, general character/behavior description, menus, and descriptions of overall system functionality. Any low level implementation decisions, sales and marketing decisions, or graphic presentation decisions are outside the scope of this document and should be specified in other parts of the wiki.

### Audience:

The audience for this document is generally going to be a technically savvy individual, most likely associated with Green Door Games, LLC as a company. The technical details of the document will mostly focus on how the product is used by the player as is common in a Use Case driven development methodology.

### Conventions:

This document lays out an overall design specification based on two primary analysis tools:

**The use case**: [http://en.wikipedia.org/wiki/Use\_case](https://web.archive.org/web/20120323054600/http://en.wikipedia.org/wiki/Use_case)

**The use case diagram**: [http://en.wikipedia.org/wiki/Use\_case\_diagram](https://web.archive.org/web/20120323054600/http://en.wikipedia.org/wiki/Use_case_diagram)

Generally, this type of "use case driven" methodology depends upon use cases to describe overall systems, and then depends on analysis of those use cases to break down a system design at the code level.

The use cases in the document will intentionally include as little design specification as possible. We will also be omitting use case elements other than steps unless that information is actually outside of what would be considered shared common knowledge (e.g. Trigger: "User Turns on Video Game System" is implied, everyone knows you have to do it - so it doesn't need to be written down).

This document will generally adhere to the following layout:

1.  The Solution (e.g. Swim Fishy Swim)
    1.  Solution Requirements
    2.  Solution Actors
    3.  Solution Use Cases
    4.  Systems that make up the solution _(there can be 1..n of these!)_
        1.  System Name (e.g. SYS1: Title Screen)
            1.  System Requirements
            2.  System Actors
            3.  System Use Cases
        2.  System Name (e.g. SYS2: Game Progress / Level Details Screen)
            1.  System Requirements
            2.  System Actors
            3.  System Use Cases
        3.  etc...

  
The breakout between each System allows the design to tackle small parts of the problem at a time. Use cases for the most part should be sufficient to describe game interactions, however map and character design may include mockup graphics as well.

  

### Game Story:

In lieu of making this document the focus of the game story, please view the storyboards associated with this video: [http://www.youtube.com/watch?v=qNLGHPQjGns](https://web.archive.org/web/20120323054600/http://www.youtube.com/watch?v=qNLGHPQjGns)

**Swim Fishy Swim (SLTN001):**
------------------------------

### Solution Use Case Diagram:

### Solution Actors:

A1 Player

Describes a single player interacting with the game, from the perspective of either a WiiMote or other input device (such as a keyboard)

*   Goals
    *   1\. To learn the story of the game, and accomplish missions laid out by game levels.
    *   2\. To have fun.
    *   3\. To avoid playing a game too difficult to be entertaining.
    *   4\. To avoid unnecessary cut scenes or text that interrupt gameplay.
    *   5\. To care about the characters in the game and the goal of the game in some capacity.

### Solution Systems:

The intent of this section is to provide a high level picture of the entire solution. Below is listed the various systems that make up Swim Fishy Swim, including a brief description for each.

**Title Screen/Main Menu (SYS1):**

Describes the first screen a player sees, and the possible interactions the player has with that screen.

**Game Progress / Level Details Screen (SYS2):**

Describes a progress screen used at the start of the game, when continuing, and between each level to indicate overall progress.

**Towers (Flowers) and their Behaviors (SYS3):**

Describes the tower system (the towers in the game), how they interact with baddies, and the ammo-speed system for each tower. Details on how towers are created will be covered in later sections of the GDD. This system covers their behavior once they exist on the game map.

**Baddies and their Behaviors (SYS4):**

Describes the baddies in the game, and how they interact with the environment. Details on when/where the baddies spawn will be covered in later sections of the GDD.

**Weapons/Attacks and their Behaviors (SYS5):**

Describes interactions of weapons with the environment, baddies, and the player. See Appendix A for more information about the weapons in the game (cost, when they can be purchased, etc.).

**Tower Building Phase (SYS6):**

Describes the tower system (the towers in the game), how they interact with baddies, and the ammo-speed system for each tower. Details on how towers are created will be covered in later sections of the GDD. This system covers their behavior once they exist on the game map.

**Tower Defense Phase (SYS7):**

Describes the defense phase when baddies attack and the player/towers defend the pickle against them, as well as the potential upgrade opportunities in this phase.

**Level End Phase (SYS8):**

Describes the process of identifying the next level and providing opportunities for major weapon upgrades.

**DEPRECATED - Flower Upgrades Screen (SYS9):**

Describes the options available when a player selects a flower for upgrade.

**End of Wave Upgrades Screen (SYS10):**

Describes the options available at the end of each wave (general tower improvements)

**Level 1-1 Pysgod River (Lands of Brine) (SYS11):**

Describes the layout of the level, list of baddies/numbers, and a description of the boss fight.

**Level 1-2 Lake Briney (Lands of Brine) (SYS12):**

Describes the layout of the level, list of baddies/numbers, and a description of the boss fight.

**Level 1-3 Cornichon Sound (Lands of Brine) (SYS13):**

Describes the layout of the level, list of baddies/numbers, and a description of the boss fight.

**Level 2-1 Nibble Island (Sandy Salty Desert) (SYS14):**

Describes the layout of the level, list of baddies/numbers, and a description of the boss fight.

**Level 2-2 Barren Wind Isle (Sandy Salty Desert) (SYS15):**

Describes the layout of the level, list of baddies/numbers, and a description of the boss fight.

**Level 2-3 Little Pickle Rock (Sandy Salty Desert) (SYS16):**

Describes the layout of the level, list of baddies/numbers, and a description of the boss fight.

**Level 3-1 Scraggly Skiffle (Frozen Sepul Coral Wasteland) (SYS17):**

Describes the layout of the level, list of baddies/numbers, and a description of the boss fight.

**Level 3-2 Ragged Range (Frozen Sepul Coral Wasteland) (SYS18):**

Describes the layout of the level, list of baddies/numbers, and a description of the boss fight.

**Level 3-3 Mount Echinoderm (Frozen Sepul Coral Wasteland) (SYS19):**

Describes the layout of the level, list of baddies/numbers, and a description of the boss fight.

Title Screen/Main Menu (SYS1):
------------------------------

Describes the first screen a player sees, and the possible interactions the player has with that screen.

### Use Case Diagram:

[![SLTN001-SYS1-UCD001.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/7/79/SLTN001-SYS1-UCD001.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:SLTN001-SYS1-UCD001.png)

### Requirements:

* * *

**SYS1-MAINMENU001:**

System shall display a title screen that gives the player a clear first impression of the game look/feel.

**Notes:**

**Applies to Use Case(s):**

*   Player Views Title Screen (SYS1-UC001)

**SYS1-MAINMENU002:**

If player does not interact with the title screen in a reasonable period of time the system will display the game story.

**Notes:**

**Applies to Use Case(s):**

*   Player Watches Story (SYS1-UC002)

**SYS1-MAINMENU003:**

System shall allow player to interact with title screen in a way that will allow them to select an option to start a new game.

**Notes:**

**Applies to Use Case(s):**

*   Player Starts New Game (SYS1-UC003)

**SYS1-MAINMENU004:**

System shall allow player to interact with title screen in a way that will allow them to continue playing a game from the last save point.

**Notes:** Only one save point will be available - which is overwritten every time a player saves.

**Applies to Use Case(s):**

*   Player Continues Game (SYS1-UC004)

**SYS1-MAINMENU005:**

System shall display the progress of the game before beginning actual game play.

**Notes:**

**Applies to Use Case(s):**

*   Views Game Progress (SYS1-UC005)

### Actors:

* * *

SYS1-A1 Player

Describes the player interacting with the game either by WiiMote, Keyboard, Mouse, or other controller.

*   Goals
    *   1\. To understand the story of the game
    *   1\. To Start a New game when desired
    *   2\. To Continue playing from a previously saved game when desired.

### System Use Cases:

* * *

Below are listed all of the use cases associated with this system and its' requirements.

#### Player Views Title Screen (SYS1-UC001):

1.  Player turns on System and waits for menu to display.
2.  Player clicks on icon for "Swim Fishy Swim" (icon is animated screen on Wii).
3.  "Swim Fishy Swim" title screen fades in.
    1.  Title screen shows sunset, and land going from ashen, to sandy, to green. (ashen on right, green on left)
    2.  Swim Fishy Swim logo scrolls in on top of screen.
    3.  Display Text: "Start" and "Continue" under logo.
    4.  A selection of baddie fish swim from right to left across screen.
4.  Player doesn't do a dang thing.
    1.  Goto: <<include>> (SYS1-UC002: Player Watches Story)
5.  (Alternate Path) Player clicks "Start".
    1.  Goto: <<include>> (SYS1-UC003: Player Starts New Game)
6.  (Alternate Path) Player clicks "Continue".
    1.  Goto: <<include>> (SYS1-UC004: Player Continues Game)

#### Player Watches Story (SYS1-UC002):

1.  Player views introduction story: [http://www.youtube.com/watch?v=qNLGHPQjGns](https://web.archive.org/web/20120323054600/http://www.youtube.com/watch?v=qNLGHPQjGns)
2.  (Alternative) Player presses a button/key. Return to title screen with main menu open.
    1.  Goto: <<include>> (SYS1-UC001: Player Views Title Screen)
3.  Introduction finishes without player interaction. Return to title screen with main menu not unopened.
    1.  Goto: <<include>> (SYS1-UC001: Player Views Title Screen)

#### Player Starts New Game (SYS1-UC003):

1.  System provides optional introductory tutorial (likely static images, player can quit at any time):
    1.  System explains what the goal of the game is (defend your pickle)
    2.  System explains the phases of the game and defenses at your disposal
        1.  System explains flower building phase (before baddies show up):
            1.  System explains chompy flower
            2.  System explains ptewey flower
            3.  System explains stunner flower
            4.  System explains windy flower
            5.  System explains how flowers get weaker over time
        2.  System explains pickle protecting phase (after baddies show up - no more building - choose from 1 of 4 weapons):
            1.  System explains bubble cannon
            2.  System explains bubble mines
            3.  System explains bubble ring
            4.  System explains bubble blaster
            5.  System explains physical attack
            6.  System explains that all defenses come from pickle power (magic) and that if it runs out it has to recharge before the defenses work again.
    3.  Goto: <<include>> (SYS1-UC005: Player Views Game Progress)

#### Player Continues Game (SYS1-UC004):

1.  System loads previous save point from file system
2.  Seriously... that's all that happens here.
    1.  Goto: <<include>> (SYS2-UC001: Player views Game Progress)

### Mockups:

This section provides visual mock ups of the in-game elements. Please note that these do not contain actual branding or design elements but simply exist to act as a placeholder to act as a general suggestion to a professional artist. (That's why most of them look like they were rendered by a 12 year old using a bacon wrapped mouse with MS Paint)

#### Title Screen:

[![SYS1-TitleScreen.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/1/14/SYS1-TitleScreen.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:SYS1-TitleScreen.png)

  

#### Story (Storyboards in video format):

[http://www.youtube.com/watch?v=qNLGHPQjGns](https://web.archive.org/web/20120323054600/http://www.youtube.com/watch?v=qNLGHPQjGns)

#### Tutorial Screens:

TODO: Mock up tutorial screens once remaining design is complete and we're confident that we've got game play elements that are worth keeping.

Game Progress / Level Details Screen (SYS2):
--------------------------------------------

Describes a screen where the player can see their current status and their progress in the game.

### Use Case Diagram:

[![SLTN001-SYS2-UCD001.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/b/b4/SLTN001-SYS2-UCD001.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:SLTN001-SYS2-UCD001.png)

### Requirements:

**SYS2-PROG001:**

System shall display current progress and points of player in the game between each level, and before starting a new or continuing an existing game.

**Notes:**

**Applies to Use Case(s):**

*   (SYS2-UC001) Player views Game Progress

### Actors:

* * *

SYS2-A1 Player

Describes the player interacting with the game either by WiiMote, Keyboard, Mouse, or other controller.

*   Goals

1.  To view current points
2.  To view progress in the game
3.  To view progress needed to complete game

### System Use Cases:

* * *

Below are listed all of the use cases associated with this system and it's requirements.

#### Player views Game Progress (SYS2-UC001):

1.  System displays player status on screen (Points Earned, and... TODO: Figure out what status information is relevant between levels!)
2.  System displays map of all levels from the lands of brine to sepul coral
3.  Systems displays icon at level player is currently on (e.g. First level if starting new game, otherwise last level saved)
4.  System displays name of level
5.  System loops playing music until a key/button is pressed

**Postcondition:** System is now ready for gameplay. The first part of each level is the tower building phase. Please review SYS5 (Tower Building Phase) for more details. After the tower building phase the game moves into SYS6 - Tower Defense Phase, followed by SYS7 - Level End Phase.

### Mockups:

This section provides visual mock ups of the in-game elements. Please note that these do not contain actual branding or design elements but simply exist to act as a placeholder to act as a general suggestion to a professional artist. (That's why most of them look like they were rendered by a 12 year old)

#### Progress Screen:

[![SYS2-Progress.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/7/71/SYS2-Progress.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:SYS2-Progress.png)

Towers (Flowers) and their Behaviors (SYS3):
--------------------------------------------

Describes the tower system (the towers in the game), how they interact with baddies, and the ammo-speed system for each tower. Details on how towers are created will be covered in later sections of the GDD. This system covers their behavior once they exist on the game map.

### Use Case Diagram:

[![SLTN001-SYS3-UCD001.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/e/e3/SLTN001-SYS3-UCD001.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:SLTN001-SYS3-UCD001.png)

### Requirements:

* * *

**SYS3-TOWER001:**

System shall support a tower that will push baddies relative to the direction the tower faces and the

position of the baddie relative to the tower.

**Applies to Use Case(s):**

*   Windy Tower Interacts w/ Baddies (SYS3-UC001)

**SYS3-TOWER002:**

System shall support a tower that will cause severe damage to any approaching enemy, but only in a very

small radius around the tower.

**Applies to Use Case(s):**

*   Thorny Tower interacts w/ Baddies (SYS3-UC002)

**SYS3-TOWER003:**

System shall support a tower that will launch projectiles in an arc (allowing baddies to slip under this arc). When

the projectiles connect with a baddie, the baddie will be knocked in the opposite direction from it's current trajectory, and take :medium damage.

**Applies to Use Case(s):**

*   Plompy Tower interacts w/ Baddies (SYS3-UC003)

**SYS3-TOWER004:**

System shall support a tower that will shoot rapid low damage projectiles at the nearest baddie.

**Applies to Use Case(s):**

*   Ptewey Tower interacts w/ Baddies (SYS3-UC004)

**SYS3-TOWER005:**

Projectile towers in the system will contain their own magic-meter which is diminished the more they are used.

When the magic-meter reaches 0, the tower will need to recharge before firing again.

**Applies to Use Case(s):**

*   Plompy Tower interacts w/ Baddies (SYS3-UC003)
*   Ptewey Tower interacts w/ Baddies (SYS3-UC004)

### System Use Cases:

* * *

Below are listed all of the use cases associated with this system and its' requirements.

#### Windy Tower Interacts w/ Baddies: (SYS3-UC001)

1.  Windy tower uses default standing animation.
2.  Windy tower evaluates which baddie is the nearest baddie.
3.  Windy tower adjusts its current angle to face the nearest baddie.
4.  If \[player or baddie collides with tower\]
    1.  Windy tower stops current animation.
    2.  Windy tower switches to swaying animation (think reeds swaying in the breeze, or a corn field someone is running through).
    3.  Windy tower doesn't affect baddies in range while swaying.
5.  If \[a baddie is within range\]
    1.  Windy Tower changes to attack animation (the flower petals spin like a fan)
    2.  Windy tower evaluates current velocity of baddie.
    3.  Windy tower changes the baddie velocity by half of the current velocity for the baddie multiplied by the angle of the tower.

#### Thorny Tower interacts w/ Baddies (SYS3-UC002) :

1.  Thorny tower uses default standing animation.
2.  Thorny tower evaluates which baddie is the nearest baddie.
3.  Thorny tower adjusts its current angle to face the nearest baddie.
4.  If \[player or baddie collides with tower\]
    1.  Thorny tower stops current animation.
    2.  Thorny tower switches to swaying animation (think reeds swaying in the breeze, or a corn field someone is running through).
    3.  If Thorny tower is swaying, then it can't attack.
5.  If \[a baddie is within range\]
    1.  Thorny Tower changes to attack animation (A stabbing motion)
    2.  If \[closest Baddie has health left\]
        1.  Thorny tower does heavy damage, offset by tower upgrades.
    3.  Thorny tower cannot attack again until the attack animation is complete.

#### Plompy Tower interacts w/ Baddies (SYS3-UC003) :

1.  Plompy tower uses default standing animation.
2.  Plompy tower evaluates which baddie is the nearest it's goal.
3.  Plompy tower adjusts its current angle to face the baddie nearest it's goal.
4.  If \[player or baddie collides with tower\]
    1.  Plompy tower stops current animation.
    2.  Plompy tower switches to swaying animation (think reeds swaying in the breeze, or a corn field someone is running through).
    3.  If Plompy tower is swaying, then it can't attack.
5.  If \[a baddie is within range\]
    1.  Plompy Tower changes to attack animation (like vomiting something up into the air)
        1.  Plompy triggers a projectile to toss in the air at a 45 degree vertical angle.
        2.  If \[the projectile hits a baddie\] the baddie is knocked backward and receives medium damage
    2.  Plompy tower cannot shoot again until the attack animation is complete.
    3.  Plompy tower reduces it's energy level each time it fires. As energy runs out, it shoots slower.
    4.  If \[Plompy tower energy level reaches 0\] Plompy can't fire until it recharges the minimum amount.

#### Ptewey Tower interacts w/ Baddies (SYS3-UC004) :

1.  Ptewey tower uses default standing animation.
2.  Ptewey tower evaluates which baddie is nearest it's goal.
3.  Ptewey tower adjusts its current angle to face the baddie nearest it's goal.
4.  If \[player or baddie collides with tower\]
    1.  Ptewey tower stops current animation.
    2.  Ptewey tower switches to swaying animation (think reeds swaying in the breeze, or a corn field someone is running through).
    3.  If Ptewey tower is swaying, then it can't attack.
5.  If \[a baddie is within range\]
    1.  Ptewey Tower changes to attack animation (like vomiting something up into the air)
        1.  Ptewey triggers a projectile targeting the baddie by angle.
        2.  If \[the projectile hits a baddie\] the baddie receives a low level of damage. No other effect.
    2.  Ptewey tower cannot shoot again until the attack animation is complete.
    3.  Ptewey tower reduces it's energy level each time it fires. As energy runs out, it shoots slower.
    4.  If \[Ptewey tower energy level reaches 0\] Ptewey can't fire until it recharges the minimum amount.

### Mockups:

This section provides visual mock ups of the in-game elements. Please note that these do not contain actual branding or design elements but simply exist to act as a placeholder to act as a general suggestion to a professional artist. (That's why most of them look like they were rendered by a 12 year old)

#### Windy Towers:

Windy towers affect the movement of the baddies they're pointing at based on the position of the baddie relative to the position of the tower. Windy towers are the least expensive tower to build, taking only 15% of the players magic bar.

[![SYS3 Towers Windy2.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/9/99/SYS3_Towers_Windy2.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:SYS3_Towers_Windy2.png)

#### Thorny Towers (with Windy Towers):

Thorny towers do a high level of damage at close range. They can be used in conjunction with Windy towers to create a sort of baddie fish boobie trap. Thorny towers are the 2nd least expensive tower, taking up 25% of the players magic bar.

[![SYS3 Towers Windy.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/b/b4/SYS3_Towers_Windy.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:SYS3_Towers_Windy.png)

#### Petewy Towers:

Petewy towers deploy a rapid fire bubble attack to damage enemies. If this was super mario brothers 2, this tower would be Mario. It's your general all purpose workhorse. This is the third most expensive tower, taking 35% of the players magic bar.

[![SYS3 Towers Ptewey.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/e/e7/SYS3_Towers_Ptewey.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:SYS3_Towers_Ptewey.png)

#### Plompy Towers:

Plompy towers are useful in that they both stun enemies. When they have full energy, a well placed plompy tower could keep an enemy in the same location for a while... except for how it would kill the enemy b/c they do a fair bit of damage. These are the most expensive tower, taking 45% of the players magic bar to create.

[![SYS3 Towers Plompy.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/2/2b/SYS3_Towers_Plompy.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:SYS3_Towers_Plompy.png)

Baddie Behaviors (SYS4):
------------------------

Describes the possible baddie behavior in the game, and how baddies interact with the environment. Details on when/where the baddies spawn will be covered in later sections of the GDD.

### Use Case Diagram:

[![SLTN001-SYS4-UCD001b.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/6/67/SLTN001-SYS4-UCD001b.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:SLTN001-SYS4-UCD001b.png)

### Requirements:

**SYS4-WEAP001:**

System shall support a small quick fire bubble weapon that does a small amount of damage for each eat (but can fire rapidly)

**Applies to Use Case(s):**

*   Fires Swigen Cannon (SYS4-UC001)

### System Use Cases:

* * *

Below are listed all of the use cases associated with this system and its' requirements.

### Mockups:

This section provides visual mock ups of the in-game elements. Please note that these do not contain actual branding or design elements but simply exist to act as a placeholder to act as a general suggestion to a professional artist. (That's why most of them look like they were rendered by a 12 year old)

Weapons/Attacks and their Behaviors (SYS5):
-------------------------------------------

Describes interactions of weapons with the environment, baddies, and the player. See Appendix A for more information about the weapons in the game (cost, when they can be purchased, etc.).

### Use Case Diagram:

[![SLTN001-SYS5-UCD001.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/0/03/SLTN001-SYS5-UCD001.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:SLTN001-SYS5-UCD001.png)

### Requirements:

**SYS5-WEAP001:**

System shall support a small quick fire bubble weapon that does a small amount of damage for each eat (but can fire rapidly)

**Applies to Use Case(s):**

*   Fires Swigen Cannon (SYS5-UC001)

**SYS5-WEAP002:**

System shall support a mine like weapon that creates bubble shrapnel that fires in random directions.

**Applies to Use Case(s):**

*   Arms Dŵr Bomb (SYS5-UC002)

**SYS5-WEAP003:**

System shall support a defensive tool that creates giant bubbles that deflect baddie movement for a time.

**Applies to Use Case(s):**

*   Triggers Bubble Wal (SYS5-UC002)

**SYS5-WEAP004:**

System shall support an offensive tool that creates a small fishy that will patroll the area where it is spawned until a baddie comes in range (at which time it will pursue the baddie)

**Applies to Use Case(s):**

*   Releases Carterfi Fishy (SYS5-UC002)

### System Use Cases:

* * *

Below are listed all of the use cases associated with this system and its' requirements.

#### Fires Swigen Cannon (SYS5-UC001):

Precondition: Player presses fire button (B on Wii).

1.  System checks to ensure that a bubble can be deployed based on cannon level (see: APPENDIX-A)
2.  \[Ammo available\] System fires a long range bubble shaped projectile through the air on an arc.
    1.  Projectile velocity and angle set upon firing
3.  REPEAT (until projectile life is <= 0):
    1.  Remove one from projectile life
    2.  Move projectile based on current velocity and angle
    3.  \[Projectile collides with wall\] Projectile changes angle to bounce off of wall.
    4.  \[Projectile collides with baddie\] Projectile removes health from baddie based on remaining life of projectile
        1.  Projectile life set to 0
    5.  \[Projectile lands in water\] Projectile velocity is slowed
        1.  Projectile bobs in water as it moves forward.

#### Arms Dŵr Bomb (SYS5-UC002):

Precondition: Player presses fire button (B on Wii), and has selected the Dŵr Bomb as the current weapon.

1.  System checks to ensure that a bubble mine can be deployed based on weapon level (see: APPENDIX-A)
2.  \[Ammo available\] System sets bubble/mine shaped projectile in water behind player.
3.  REPEAT (until projectile life is <= 0):
    1.  Remove one from projectile life
    2.  Mine stays in same place in water, and bobs up an down.
    3.  \[Projectile collides with baddie\] Projectile removes health from baddie based on remaining life of projectile
        1.  Baddie is tossed up into the air
        2.  Projectile life set to 0
    4.  \[Projectile life runs out without hitting baddie\] Projectile shrinks and vanishes.

#### Triggers Bubble Wal (SYS5-UC003):

Precondition: Player presses fire button (B on Wii), and has selected the Bubble Wal as the current weapon.

1.  System checks to ensure that a bubble wal can be deployed based on weapon level (see: APPENDIX-A)
2.  \[Ammo available\] System places small bubble wal in front of players location, which expands to block path.
3.  REPEAT (until projectile life is <= 0):
    1.  Remove one from projectile life
    2.  Wal stays in same place in water, and bobs up an down.
    3.  \[Baddie collides with Bubble Wal\] Remove one from projectile life
        1.  Baddie is tossed backward - away from Bubble Wal
    4.  \[Projectile Health <= 0\] Bubble Wal shrinks and disappears

#### Releases Carterfi Fishy (SYS5-UC004):

1.  System checks to ensure that a fishy can be deployed based on ammo level (see: APPENDIX-A)
2.  \[Ammo available\] System releases small fishy at player location
    1.  Projectile velocity and angle set upon firing
3.  REPEAT (until projectile life is <= 0):
    1.  Fish pathfinds toward the closest baddie and reroutes around map obstacles
    2.  \[Projectile collides with baddie\] Projectile removes health from baddie based on total life of projectile
        1.  Projectile life set to 0

### Mockups:

This section provides visual mock ups of the in-game elements. Please note that these do not contain actual branding or design elements but simply exist to act as a placeholder to act as a general suggestion to a professional artist. (That's why most of them look like they were rendered by a 12 year old)

Tower Building Phase (SYS6):
----------------------------

Describes the tower system (the towers in the game), how they interact with baddies, and the ammo-speed system for each tower. Details on how towers are created will be covered in later sections of the GDD. This system covers their behavior once they exist on the game map.

### SYS6 - Use Case Diagram:

[![SLTN001-SYS6-UCD001a.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/2/23/SLTN001-SYS6-UCD001a.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:SLTN001-SYS6-UCD001a.png)

### SYS6 - Requirements:

* * *

**SYS6-BUILDING001:**

System shall allow player a period of time (adjusted based on difficulty of level) to collect power ups, and build tower defenses before the level begins.

**Applies to Use Case(s):**

*   Collects Power-ups and Grows Flowers (SYS6-UC001)

**SYS6-BUILDING002:**

System shall allow player to speak with the Pickle to end the building phase early, and select a weapon.

**Applies to Use Case(s):**

*   Talks to pickle, chooses weapon. (SYS6-UC002)

### SYS6: System Use Cases:

* * *

Below are listed all of the use cases associated with this system and its' requirements.

#### Collects Power-ups and Grows Flowers: (SYS6-UC001)

1.  Player spawns in map on opposite side of map from Pickle.
2.  System indicates that it's the Tower Building (Flower Growing) phase.
3.  Player navigates maps collecting Magic Power-Ups (They give a bonus that increases current magic level)
4.  As Player has the magic level to grow each flower (15% Windy, 25% Thorny, 40% Lobby, 50% Ptewey) player presses Up,Down,Left, or Right to cause flower to be grown.
5.  System displays and decrements a timer value indicating how much time is left in the building phase.
6.  \[Timer runs out\] Building phase is over, even if player hasn't talked to pickle yet.
    1.  System indicates that building phase is over.
    2.  <<Include>> (SYS6-UC002) Talks to Pickle, Chooses Weapon.
    3.  System begins Tower Defense Phase
7.  \[Player is done building, and talks to Pickle\]
    1.  Pickle asks player if the player is done growing flowers
    2.  \[No\] System continues with Building Phase.
    3.  \[Yes\] <<Include>> (SYS6-UC002) Talks to Pickle, Chooses Weapon.
        1.  System begins Tower Defense Phase.

#### Talks to Pickle, Chooses Weapon (SYS6-UC002):

1.  \[Building timer not expired before player talks to pickle\] Dialog is displayed asking character whether he wishes to finish building.
    1.  \[Yes\] <<Goto>> (SYS6-UC001) Collects Power-ups and Grows Flowers Step 1
    2.  \[No\] Dialog is displayed showing one of four weapons that the player can choose from.
        1.  Player chooses a bubble weapon that shoots forward (quick fire/weak dmg) (bounce/fire rate available as upgrade).
        2.  (Alternative) Player chooses a bubble weapon that lays bubble-mines (very slow fire/stron dmg) (bounce/fire rate available as upgrade).
        3.  (Alternative) Player chooses a bubble weapon that creates huge bubble barriers (slow/no dmg) (barrier life available as upgrade).
        4.  (Alternative) Player chooses a bubble weapon that creates homing-fish that home in on and ram enemies (med fire / med damage).
2.  <<Return to>> (SYS6-UC001) Collects Power-ups and Grows Flowers

#### Mockups:

This section provides visual mock ups of the in-game elements. Please note that these do not contain actual branding or design elements but simply exist to act as a placeholder to act as a general suggestion to a professional artist. (That's why most of them look like they were rendered by a 12 year old)

#### Phase Overview:

Gives a visual overview of how this phase might be laid out on level one.

[![Tower building phase.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/7/7a/Tower_building_phase.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:Tower_building_phase.png)

#### Hero, Choose your Weapon:

Provides a visual indication of the potential weapon elements. (Maybe my worst mockup, dear artist, will you please correct these?)

[![SYS6 - Tower Building Weapons a.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/9/9f/SYS6_-_Tower_Building_Weapons_a.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:SYS6_-_Tower_Building_Weapons_a.png)

Tower Defense Phase (SYS7):
---------------------------

Describes the defense phase when baddies attack and the player/towers defend the pickle against them, as well as the potential upgrade opportunities in this phase.

### SYS7 - Use Case Diagram:

[![SLTN001-SYS7-UCD001.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/4/46/SLTN001-SYS7-UCD001.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:SLTN001-SYS7-UCD001.png)

### SYS7 - Requirements:

* * *

**SYS7-DEFEND001:**

System shall allow Player to upgrade towers based on points specified in APPENDIX-C.

**Applies to Use Case(s):**

*   Upgrades Towers as points are available (SYS7-UC001)

**SYS7-DEFEND002:**

System shall give Player a visual indication when a tower upgrade is available for the nearest tower.

**Applies to Use Case(s):**

*   Upgrades Towers as points are available (SYS7-UC001)

**SYS7-DEFEND003:**

System shall allow Player to navigate the game world, using physical and projectile attacks in conjunction with current tower defense to hamper or stop baddie progress.

**Applies to Use Case(s):**

*   Attempts to stop or hamper baddies from reaching pickle (SYS7-UC002)

### SYS7: System Use Cases:

* * *

Below are listed all of the use cases associated with this system and its' requirements.

#### Upgrades Towers as points are available (SYS7-UC001):

Describes process for displaying potential upgrades

1.  Player navigates map
2.  As player comes within firing range of towers
    1.  The nearest tower begins to glow on the map
    2.  \[Enough Upgrade Points Available, and tower has upgrades left\] A button appears on screen that will trigger upgrade specifying # of points required.
    3.  (Alternative) \[Enough Upgrade Points Not Available, and tower has upgrades left\] A translucent button appears on screen specifying # of points required.
3.  As player leaves firing range of any towers, they stop glowing and the button disappears.

#### Attempts to stop or hamper baddies from reaching pickle (SYS7-UC002):

High level description of player interaction (mostly links to other use cases)

1.  Player navigates map.
2.  As baddies slip through tower defenses player can attempt to stop them by:
    1.  Players fires Swigen Cannon by pressing B button
        1.  <<include>> Fires Swigen Cannon (SYS5-UC001)
    2.  Players arms Dŵr Bomb by pressing B button
        1.  <<include>> Arms Dŵr Bomb (SYS5-UC002)
    3.  Player triggers Bubble Wal by pressing B button
        1.  <<include>> Triggers Bubble Wal (SYS5-UC003)
    4.  Player releases Carterfi Fishy by pressing B button
        1.  <<include>> Releases Carterfi Fishy (SYS5-UC004)
    5.  Player triggers physical spin-attack by shaking Wiimote
        1.  Player shakes wiimote horizontally
        2.  \[Contact with baddie is made\] Baddie is pushed back in water at an angle from player
            1.  Baddie takes damage relative to how close they are to center

#### Mockups:

This section provides visual mock ups of the in-game elements. Please note that these do not contain actual branding or design elements but simply exist to act as a placeholder to act as a general suggestion to a professional artist. (That's why most of them look like they were rendered by a 12 year old)

#### Phase Overview:

Gives a visual overview of how this phase might be laid out on level one.

Level End Phase (SYS8):
-----------------------

Describes the process of identifying the next level and providing opportunities for major weapon upgrades.

TODO: Move this phase over from pen & paper documentation

DEPRECATED - Flower Upgrades Screen (SYS9):
-------------------------------------------

Describes the options available when a player selects a flower for upgrade.

TODO: Move this phase over from pen & paper documentation

End of Wave Upgrades Screen (SYS10):
------------------------------------

Describes the options available at the end of each wave (general tower improvements)

TODO: Move this phase over from pen & paper documentation

Level 1-1 Pysgode River (Lands of Brine) (SYS11):
-------------------------------------------------

[![Level 1-1a.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/9/95/Level_1-1a.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:Level_1-1a.png)

Level 1-2 Lake Briney (Lands of Brine) (SYS12):
-----------------------------------------------

Describes the layout of the level, list of baddies/numbers, and a description of the boss fight.

[![Level 1-2.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/1/1c/Level_1-2.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:Level_1-2.png)

Level 1-3 Cornichon Sound (Lands of Brine) (SYS13):
---------------------------------------------------

Describes the layout of the level, list of baddies/numbers, and a description of the boss fight.

[![Level 1-3.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/0/09/Level_1-3.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:Level_1-3.png)

Level 2-1 Nibble Island (Sandy Salty Desert) (SYS14):
-----------------------------------------------------

Describes the layout of the level, list of baddies/numbers, and a description of the boss fight.

TODO: Once we're sure levels 1-1 to 1-3 are tweaked then design the rest!

Level 2-2 Barren Wind Isle (Sandy Salty Desert) (SYS15):
--------------------------------------------------------

Describes the layout of the level, list of baddies/numbers, and a description of the boss fight.

TODO: Once we're sure levels 1-1 to 1-3 are tweaked then design the rest!

Level 2-3 Little Pickle Rock (Sandy Salty Desert) (SYS16):
----------------------------------------------------------

Describes the layout of the level, list of baddies/numbers, and a description of the boss fight.

TODO: Once we're sure levels 1-1 to 1-3 are tweaked then design the rest!

Level 3-1 Scraggly Skiffle (Frozen Sepul Coral Wasteland) (SYS17):
------------------------------------------------------------------

Describes the layout of the level, list of baddies/numbers, and a description of the boss fight.

TODO: Once we're sure levels 1-1 to 1-3 are tweaked then design the rest!

Level 3-2 Ragged Range (Frozen Sepul Coral Wasteland) (SYS18):
--------------------------------------------------------------

Describes the layout of the level, list of baddies/numbers, and a description of the boss fight.

TODO: Once we're sure levels 1-1 to 1-3 are tweaked then design the rest!

Level 3-3 Mount Echinoderm (Frozen Sepul Coral Wasteland) (SYS19):
------------------------------------------------------------------

Describes the layout of the level, list of baddies/numbers, and a description of the boss fight.

TODO: Once we're sure levels 1-1 to 1-3 are tweaked then design the rest!

Weapons/Attacks and their Behaviors (APPENDIX-A):
-------------------------------------------------

This section of the appendix covers details regarding weapons, attacks, their cost, and provides some small details about behavior.

### Swigen Cannon

When fired the Swigen Cannon will shoot a single bright bubble quickly at the angle the Player is moving. Swigen bullets will ricochet off of walls when encountered.

#### Purchase/Upgrade

The Swigen Cannon is available by default, and can be upgraded at the end of level menu.

#### Levels

Level 1

Level one will allow four shots per second.

Level 2

Level two will allow six shots per second.

Level 3

Level three will allow eight shots per second

Level 4

Level four will allow ten shots per second

#### Costs

Level1

0 Upgrade Points

Level2

1000 Upgrade Points

Level3

10000 Upgrade Points

Level4

30000 Upgrade Points

  

### Dŵr Bomb

When fired the Dŵr Bomb sits in the water for a period of 20 seconds, or until a baddie collides with it. When it explodes it shoots out multiple bubbles in multiple directions, inflicting heavy damage.

#### Purchase/Upgrade

The Dŵr Bomb can be upgraded at the end of level menu.

#### Levels

Level 1

Level one will allow only a single bomb at a time on the map.

Level 2

Level two will allow only two bombs at a time on the map.

Level 3

Level three will allow only three bombs at a time on the map.

Level 4

Level four will allow only four bombs at a time on the map.

#### Costs

Level1

3000 Upgrade Points

Level2

15000 Upgrade Points

Level3

40000 Upgrade Points

Level4

50000 Upgrade Points

  

### Bubble Wal

When fired Bubble Wal will create a huge bubble that will cause baddies to continually bounce off of it until the bubble breaks, or 40 seconds have passed since the last time it was hit.

#### Purchase/Upgrade

The Bubble Wal can be upgraded at the end of level menu.

#### Levels

Level 1

Level one will allow only a single Wal at a time on the map.

Level 2

Level two will allow only two Wals at a time on the map.

Level 3

Level three will allow only three Wals at a time on the map.

Level 4

Level four will allow only four Wals at a time on the map.

#### Costs

Level1

1500 Upgrade Points

Level2

15000 Upgrade Points

Level3

30000 Upgrade Points

Level4

45000 Upgrade Points

  

### Carterfi Fishy

When fired Carterfi Fishy will spawn a small fish that will patrol an area and home in on the first baddie that gets near.

#### Purchase/Upgrade

The Caterfi Fishy can be upgraded at the end of level menu.

#### Levels

Level 1

Level one will allow only a single fishy at a time on the map.

Level 2

Level two will allow only two fishys at a time on the map.

Level 3

Level three will allow only three fishys at a time on the map.

Level 4

Level four will allow only four fishys at a time on the map.

#### Costs

Level1

700 Upgrade Points

Level2

9000 Upgrade Points

Level3

18000 Upgrade Points

Level4

36000 Upgrade Points

Baddies and their Behaviors (APPENDIX-B):
-----------------------------------------

Provides details and visual suggestion for the baddies in the game:

### L1 Baddies: (Levels 1-1 to 1-3)

[![Bumbler L1.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/2/24/Bumbler_L1.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:Bumbler_L1.png) [![Swarmer L1.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/1/13/Swarmer_L1.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:Swarmer_L1.png) [![Eel L1.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/1/1b/Eel_L1.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:Eel_L1.png)

### L1 Boss: (Level 1-3)

[![Crab L1.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/c/c2/Crab_L1.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:Crab_L1.png)

### L2 Baddies: (Levels 2-1 to 2-3)

[![Bumbler L2.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/8/8c/Bumbler_L2.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:Bumbler_L2.png) [![Swarmer L2.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/8/87/Swarmer_L2.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:Swarmer_L2.png) [![Eel L2.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/2/2a/Eel_L2.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:Eel_L2.png)

### L2 Boss: (Level 2-3)

[![Crab L2.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/9/99/Crab_L2.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:Crab_L2.png)

### L3 Baddies: (Levels 3-1 to 3-3)

[![Bumbler L3.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/5/5a/Bumbler_L3.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:Bumbler_L3.png) [![Swarmer L3.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/7/7d/Swarmer_L3.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:Swarmer_L3.png) [![Eel L3.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/2/23/Eel_L3.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:Eel_L3.png)

### L3 Boss: (Level 3-3)

[![Crab L3.png](/web/20120323054600im_/http://www.greendoorgames.com/sfswiki/images/c/c0/Crab_L3.png)](/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=File:Crab_L3.png)

Towers Upgrade Options (APPENDIX-C):
------------------------------------

Provides details and visual suggestion for the towers in the game, and their available levels:

Retrieved from "[http://www.greendoorgames.com/sfswiki/index.php?title=SFS\_Games\_Design\_Document\_(GDD)](https://web.archive.org/web/20120323054600/http://www.greendoorgames.com/sfswiki/index.php?title=SFS_Games_Design_Document_(GDD))"
