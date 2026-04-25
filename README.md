# Sp26-DS-A-RPG-Game

Console RPG prototype with a traveling overworld loop, turn-based battles, inventory, loot, story beats, and save/load support.

## Build On macOS

```bash
./build.sh
```

## Run

```bash
./build/rpg-game
```

The repo also includes a `CMakeLists.txt` if you prefer CMake, but `build.sh` works with the Apple command line tools already installed on this Mac.

## Project Layout

- `source/app`: top-level program entry and main adventure loop wiring.
- `source/battle`: battle scenes, combat flow, and battle reward logic.
- `source/characters`: character stats, actions, and status-effect behavior.
- `source/inventory`: inventory data structures, catalog items, and inventory UI.
- `source/world`: overworld travel, dialogue, world map data, shops, and regional events.
- `source/persistence`: save/load serialization and migration logic.
- `source/data`: action database parsing and related data-loading code.
- `source/platform`: sound and shared terminal UI helpers.
- `source/third_party`: vendored dependencies kept separate from game code.
- `assets/data`: authored game data files such as action definitions.
- `runtime/saves`: runtime-generated save files written by the game.
