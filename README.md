# te-bonfire

A stamina-driven souls-like action-RPG *core* built on the
[Twilight Elysium engine](https://github.com/intricos777-dot/twilight-elysium).

> Original code only. This is a clean-room reimplementation of the *genre*,
> not a reproduction of any commercial game's assets.

## What's implemented

- Stamina budget combat: every swing costs stamina; rushing leaves you
  exhausted and open.
- Poise/guard state machine, HP/VIT/END/STR/DEX stat model.
- World progression: a chain of five bonfire zones (`world.h/cpp`);
  clearing a zone lights its bonfire and unbolts the next. Run state
  persists to `data/progress.json` (gitignored).
- First real boss loop (`boss.h/cpp`): the Ember Watcher gate - telegraph,
  strike, recover phases with guard-break rolls, stamina-gated offense.
- `--sim` headless mode used by CI: fights the Watcher, maps the result
  onto the zone chain and cross-checks hidden-boss unlock hints.
- Hidden-boss registry with authored unlock conditions (`hidden_boss.h`).

## Asset policy (sealed manifest)

This repository contains **no retail assets**. The game is playable as a
headless sim immediately; visual/audio assets are mounted from your own
legally-owned copies according to `data/manifest.json`:

```
assets/
  models/hero.glb
  models/knight.glb
  textures/fire.png
  audio/boss_fight.ogg
  audio/ambient.ogg
```

Nothing under `assets/` is tracked by git — it never enters this repo, and
it never enters anyone's fork.

## Build

```bash
cmake -S . -B build
cmake --build ./build -j$(nproc)
./build/te-bonfire --sim      # headless combat sim (CI)
./build/te-bonfire           # windowed run (engine-backed)
```

## License

Code: MIT (see LICENSE). Assets: never committed; rights stay with their
rightful owners.