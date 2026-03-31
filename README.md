<div align="center">
    <h1 align="center" style="border: none; margin-bottom: none;">osu-stuff</h1>
    <p align="center">yet another osu! manager</p>
</div>

<p align="center">
  todo (image)
</p>

## wtf is osu-stuff

a simple and useful app that gives you freedom to:

- manage your lazer/stable collections (import, export, merge, etc...)
- download beatmaps (missing beatmaps from collections, from players, etc...)
- listen to beatmaps songs from your osu installation (radio tab)
- a lot more...

> [!NOTE]
> beatmaps downloaded with lazer mode on will be placed on your exports folder!

## wanna help osu-stuff development?

- bug reports and merge requests are always welcome :)
- just make sure to include what you're adding or the issue you're reporting.

## download

- **latest stable version**: [download here](https://github.com/mezleca/osu-stuff/releases/latest)

## dependencies

### linux (debian/ubuntu)

```bash
sudo apt-get update
sudo apt-get install -y \
  build-essential cmake ninja-build pkg-config \
  clang \
  qt6-base-dev qt6-declarative-dev qt6-tools-dev \
  libnotify-dev libxtst-dev libdbus-1-dev libfuse2 libsndfile1-dev
```

### linux (arch)

```bash
sudo pacman -S --noconfirm \
  qt6-base qt6-declarative qt6-tools \
  libnotify libxtst dbus fuse libsndfile
```

### cross compile to windows (mingw-w64)

#### debian/ubuntu

```bash
sudo apt-get install -y \
  mingw-w64 g++-mingw-w64-x86-64 binutils-mingw-w64 \
  zlib1g-dev
```

#### arch

```bash
sudo pacman -S --noconfirm \
  mingw-w64-gcc mingw-w64-binutils mingw-w64-zlib mingw-w64-openssl
```

#### qt6 (mingw)

```bash
# arch
paru -S --noconfirm python-aqtinstall

# debian/ubuntu
python -m pip install --user aqtinstall

# note: the ~/qt path is used by the "ez" script, if you wanna install somewhere else, make sure to also modify "ez"
aqt install-qt linux desktop 6.6.2 gcc_64 -O ~/qt
aqt install-qt windows desktop 6.6.2 win64_mingw -O ~/qt
```

## build

### prerequisites

- [git](https://git-scm.com/downloads)
- [python](https://www.python.org/)
- cmake + ninja
- linux: clang
- windows / linux cross compile: mingw-w64

### steps

```bash
# clone repo
git clone https://github.com/mezleca/osu-stuff.git && cd osu-stuff

# linux build
python ez init
python ez build

# cross compile for windows (linux)
python -m pip install --user aqtinstall
~/.local/bin/aqt install-qt windows desktop 6.6.2 win64_mingw -O ~/qt
python ez configure --platform=windows
python ez build
```

## windows dependencies

```powershell
choco install -y python ninja mingw aqt
aqt install-qt windows desktop 6.6.2 win64_mingw -O $env:USERPROFILE\\qt
```

## credits

- [CollectionManager](https://github.com/Piotrekol/CollectionManager) .osdb files
- [Osu!Collector](https://osucollector.com)
