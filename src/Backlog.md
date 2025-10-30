## Add Replay Option
<hr>

## Add an Icon - both console window and GLFW window
<hr>

## Split GameSession -> +GameSessionAux
<hr>

## Fix Cylinder Ray Tracing
<hr>

## GameModel::GameRules - move to public when used
<hr>

## Add hotkey Manager And Input Profiles
<hr>

## GameView.h - Remove utility/Defaults.h Dependence
<hr>

> Maybe change GameModel::MovePossibility::canGoTo storage from const Cell* to Coord

## Add Checks
> Cell::append  
> GameBoard::placeChecker  
> GameModel::isPlayerDefeated - prevent idle move cycles (not just tossing reserve around)
<hr>

## Transpose coordinates in a single place
> BoardEditor::createBoard* now uses transposed coordinates
<hr>

## Add App Icon
> Both .ico and .png, bake into the executable
<hr>

## Create Menu

- Local Game
  - Quick 1v1 hotseat
  - Quick 1v1 AI
  - Set up a game
- Online Game
  - Ladder (rated)
  - Create
  - Join
- Board Editor
- Settings
  - Defaults
    - Default color palette
    - Default game settings
    - Default player names
    - Default AI names
  - Camera movement: smooth/sharp
  - Zoom: smooth/sharp
  - Animations: on/off
  - Key bindings
  - Open Settings.ini
- Exit
  - Confirmation