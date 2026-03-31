<div align="center">
    <h1 align="center" style="border: none; margin-bottom: none;">osu-stuff</h1>
    <p align="center">yet another osu! manager</p>
</div>

<p align="center">
  todo (image)
</p>

## what it does

- manage your lazer/stable collections (import, export, merge, etc...)
- download beatmaps (missing beatmaps from collections, from players, etc...)
- listen to beatmaps songs from your osu installation (radio tab)
- a lot more...

> [!NOTE]
> beatmaps downloaded with lazer mode on will be placed on your exports folder!

## download

**latest stable version**: [download here](https://github.com/mezleca/osu-stuff/releases/latest)

## building from source

### linux

**dependencies**

debian/ubuntu:
```bash
sudo apt-get update
sudo apt-get install -y \
  build-essential cmake ninja-build pkg-config \
  clang \
  qt6-base-dev qt6-declarative-dev qt6-tools-dev \
  libnotify-dev libxtst-dev libdbus-1-dev libfuse2 libsndfile1-dev
```

arch:
```bash
sudo pacman -S --noconfirm \
  qt6-base qt6-declarative qt6-tools \
  libnotify libxtst dbus fuse libsndfile
```

**build**
```bash
git clone https://github.com/mezleca/osu-stuff.git && cd osu-stuff
python ez init
python ez build
```

---

### windows (native)

**dependencies**
```powershell
choco install -y python ninja msys2

# open msys2 mingw64 shell and run:
pacman -S --noconfirm mingw-w64-x86_64-toolchain

# install qt6
pip install aqtinstall
aqt install-qt windows desktop 6.6.2 win64_mingw -O $env:USERPROFILE\\qt
```

**build**
```powershell
git clone https://github.com/mezleca/osu-stuff.git && cd osu-stuff
python ez init
python ez build
```

---

### cross-compile linux -> windows (mingw-w64)

**dependencies**

debian/ubuntu:
```bash
sudo apt-get install -y \
  mingw-w64 g++-mingw-w64-x86-64 binutils-mingw-w64 \
  zlib1g-dev
```

arch:
```bash
sudo pacman -S --noconfirm \
  mingw-w64-gcc mingw-w64-binutils mingw-w64-zlib
```

qt6 for cross-compile:
```bash
# debian/ubuntu
pip install --user aqtinstall

# arch
paru -S --noconfirm python-aqtinstall

# note: ~/qt is the default path used by the ez script
aqt install-qt linux desktop 6.6.2 gcc_64 -O ~/qt
aqt install-qt windows desktop 6.6.2 win64_mingw -O ~/qt
```

**build**
```bash
git clone https://github.com/mezleca/osu-stuff.git && cd osu-stuff
python ez init
python ez configure --platform=windows
python ez build
```

---

## contributing

bug reports and merge requests are always welcome. just make sure to include what you're adding or the issue you're reporting.

## credits

- [CollectionManager](https://github.com/Piotrekol/CollectionManager) — .osdb files
- [Osu!Collector](https://osucollector.com)
