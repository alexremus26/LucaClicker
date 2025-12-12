# Trying this branch in a disposable clone

Follow these steps to try the current work branch without touching your main copy of the repo.

1. **Clone into a throwaway folder**
   ```sh
   git clone <your-remote-url> LucaClicker-test
   cd LucaClicker-test
   ```

2. **Fetch this branch** (replace `work` if your remote uses a different branch name)
   ```sh
   git fetch origin work
   git checkout -b try-sell-queue origin/work
   ```

3. **Configure and build** in an isolated build directory
   ```sh
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
   cmake --build build
   ```

4. **Run from the build output**
   ```sh
   ./build/oop
   ```

If you make local tweaks while testing, run `git status` to see them. To reset the test clone back to the committed state, use:
```sh
git reset --hard HEAD
rm -rf build
```

## Updating your test clone with new changes

If you already have the `LucaClicker-test` folder from the steps above, you can pull the latest work onto it without recloning:

```sh
cd /path/to/LucaClicker-test
git checkout work          # or the branch you created earlier (e.g., try-sell-queue)
git fetch origin           # grab updated commits from the remote
git pull --ff-only origin work
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/oop
```

If you run into merge conflicts because you made edits in the test clone, run `git reset --hard origin/work` (or your branch name) to discard the local changes, then rebuild.
