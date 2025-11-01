## Replay (Start Over): Release VAO/VBO Memory on gameSession Recreation
<hr>

## Split GameSession -> +GameSessionAux (structures+enums)
<hr>

## Fix Cylinder Ray Tracing
<hr>

## GameModel::GameRules - move to public when used
<hr>

## Add hotkey Manager And Input Profiles
<hr>

## GameView.h - Remove utility/Defaults.h Dependence
<hr>

## Add Checks
> Cell::append  
> GameBoard::placeChecker  
> GameModel::isPlayerDefeated - prevent idle move cycles (not just tossing reserve around)
<hr>

## Transpose coordinates in a single place
> BoardEditor::createBoard* now uses transposed coordinates
<hr>

## Add Replay System
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