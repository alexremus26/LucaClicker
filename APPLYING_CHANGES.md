# How to pull these selling changes into your clone

This branch contains the queued selling logic (one sale at a time per item) plus the updated display guard. Follow the steps below to bring the code into your own clone without losing your work.

## Quick summary of what's in this branch
- `src/GameManager.cpp/.h`: per-item selling workers that run one sale at a time and ignore extra "s" key presses while a sale is active or already queued.
- `src/Display.cpp`: UI call that starts a sale only if the guard allows it, otherwise shows the in-progress warning.

## Safest way: try it on a throwaway branch
1. In your existing clone, fetch the branch and create a local test branch:
   ```sh
   git fetch origin work
   git checkout -b try-sell-queue origin/work
   ```
2. Build and run from that branch:
   ```sh
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
   cmake --build build
   ./build/oop
   ```
3. If you like the behavior, merge it into your main branch:
   ```sh
   git checkout main             # or your primary branch
   git merge try-sell-queue
   ```

## If you only want the latest commits without keeping a test branch
1. Fetch the branch:
   ```sh
   git fetch origin work
   ```
2. Merge it directly into your current branch:
   ```sh
   git merge origin/work
   ```
3. Build and run to verify:
   ```sh
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
   cmake --build build
   ./build/oop
   ```

## If your clone has local edits
- Check what you changed: `git status`
- Stash them temporarily: `git stash push -m "save before merge"`
- Merge this branch (`git merge origin/work`), build, and test.
- Restore your edits after testing: `git stash pop`

## Manual copy instead of git merge
If you prefer to copy files instead of merging:
1. Back up your originals:
   ```sh
   cp src/GameManager.cpp src/GameManager.cpp.bak
   cp src/GameManager.h src/GameManager.h.bak
   cp src/Display.cpp src/Display.cpp.bak
   ```
2. Copy the versions from this branch into your clone (replace `/path/to` with your path):
   ```sh
   cp /path/to/this-branch/src/GameManager.cpp src/GameManager.cpp
   cp /path/to/this-branch/src/GameManager.h src/GameManager.h
   cp /path/to/this-branch/src/Display.cpp src/Display.cpp
   ```
3. Build and run using the commands above.

If anything goes wrong, restore from your backups or run `git reset --hard` to return to your previous commit.
