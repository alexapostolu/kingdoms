# Kingdoms

A single player kingdom building game. Build your kingdom and gain rewards attacking other kingdoms in the single player campaign.




https://github.com/user-attachments/assets/1ce0288b-d1b7-4e78-9dfa-6431f418afb5




If you want to get playing, checkout the [Play](#play) section on how to install (Windows only) and play instantly!

1. [Play (Windows only)](#play)
2. [Build Source](#build-source)
3. [About Game](#about-game)
4. [Other](#other)
4. [Technical Information](#technical-information)

### Play

Download the executable (`exe`) and the dynamic library `sdl.dll` from the Releases page. Extract the zip folder. Then click on the executable file and start playing!

### Build Source

Download, and then run the cmake.

### About Game

This game is supposed to be a replica of the mobile game [Kingdom of Zenia](https://www.facebook.com/ZeniaGame/). It's similar to early versions of Clash of Clans, but instead of your army attacking an enemy's base, it's your army attacking an enemy's army, with high emphasis on troop to troop interaction. It's also similar to the mobile game Battle Legions, however Battle Legions does not have a "base" gameplay feature. Kingdom of Zenia shares some commonality with the Rise of Kingdoms games, but comparing those two is like comparing Clash of Clans with Rise of Kingdoms. 

There were some issues with the game I think, as it had some balancing issues and the late game progression was not as exciting. However, no mobile game ever came close to the amount of fun I had playing Kingdom of Zenia. The graphic design, sound design, gameplay mechanics, and game design are amazing. I plan to recreate this game, and fix those issues by adding more user interactivity (more info below).

**Platforms**

I plan to release this game first for Windows as I am developing it on Windows. And then once I create a working release for it, then I will port it to Android on the Play Store. If that goes well, then I will set up a server and develop multiplayer features so you can battle other people.

**How to keep players engaged in a base building game**

Keeping players engaged is important. For a base building game that has minimal user interaction (the only user interaction that requires strategy is deploying troops when you attack, and even then it's minimal strategy and you only do it once per attack), players that have maxed out there base will eventually get bored. So Clash of Clans solved this issue by adding more troops and more Town Halls. I want to go a different route with Kingdom of Zenia. Instead of focusing on addition content, I will focus on the user interaction. I believe adding just a little bit more user interaction can keep a base building game entertaining just as well as adding more troops and more Town Halls.

The upside to focusing on user interaction is I don't have to spend hours on Blender modelling all the new troop and building designs haha. In addition, it is easier to maintain the original feel of the game. In Clash of Clans, a TH16 Root Rider, Electro Rider, and Pixel King attack feels a lot different than a TH5 Giant, Balloon and Wizard attack. Whether that is a good or bad thing I don't know, but for Kingdom of Zenia I want to keep the same feeling that a low level attack has compared to a high level attack. If there won't be as many troop options, then this will have to come from user interactivity.

I plan to do this by first allowing the user to select a group of troop and mobilize them elsewhere and change their target. I will also allow the user to control a special "hero" troop. In addition, for each battle, there will be strategic structures, for example a small tower with ground troops at the base and archers at the top. In this case, your groud troop must first defeat the enemy's group troops, before spending time climbing up the stairs to the enemy's archers. Meanwhile, your archers can target their archers instantly. This has some implications, first the single player AI must be advanced. Secondly, I need to get he right perspective to ensure the important fights are not blocked by structures (maybe allowing camera panning?). This also means a player's base is two parts. Non defensive and defensive. Defensive is the defending troops and structures their troop will fight on against an incoming attack. And the non defensive is everything else, resource collection, town hall, barracks, etc.

I will also consider having reasource buildings produce more loot to give the game more of an idle feel, as now you don't have to constantly attack other bases to gain loot.

### Other

Right now, the main user interaction is controlling a single troop in battle. I wonder if there is any other gameplay mechanics I can add that require user interaction.

I also wonder if I could combine the "spontaneity" and "open world" of Age of Empires with this base building game.

There's this upcoming third person Steam game called [Kingmakers](https://store.steampowered.com/app/2109770/Kingmakers/), it's not released but I think it's also army vs army, and you get to be a soldier in the battle, so that's tons of user interaction, and it seems like it'll be super fun.

### Technical Information

`C++20`, `CMake`, `Visual Studio`
[SDL 2.30.3](https://github.com/libsdl-org/SDL/releases/tag/release-2.30.3)
[SDL_ttf 2.22.0](https://github.com/libsdl-org/SDL_ttf/releases/tag/release-2.22.0)
[SDL_image 2.8.2](https://github.com/libsdl-org/SDL_image/releases/tag/release-2.8.2)
[SDL_FontCache](https://github.com/grimfang4/SDL_FontCache)
Models made with `Blender`
