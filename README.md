# NeoWall Patch — Smooth Image Transitions

A patched version of **NeoWall v0.7.2** with additional GPU-based image transition effects.

This project keeps NeoWall's existing wallpaper engine and adds four additional image transitions:

* `zoom`
* `side_wipe`
* `circle`
* `radial`

The original transitions remain available:

* `fade`
* `slide_left`
* `slide_right`
* `glitch`
* `pixelate`

`none` can be used for an instant wallpaper change.

## Features

* GPU-accelerated wallpaper rendering
* Image wallpaper support
* Smooth image-to-image transitions
* 9 transition effects
* Random transition selection is easy to integrate into scripts
* No need to restart NeoWall between wallpaper changes
* `duration 0` can be used to disable automatic wallpaper cycling
* Works with X11 and Wayland environments supported by NeoWall
* Compatible with software rendering through Mesa LLVMpipe when hardware OpenGL is insufficient

## Added Transitions

### Zoom

A smooth zoom-out/zoom-in transition between the old and new wallpaper.

```text
transition zoom
```

### Side Wipe

The new wallpaper is revealed horizontally.

```text
transition side_wipe
```

### Circle

The new wallpaper expands outward from the center of the screen.

```text
transition circle
```

### Radial

The new wallpaper is revealed using a radial/angular sweep.

```text
transition radial
```

## Requirements

### General

* Linux
* X11 or Wayland
* OpenGL 3.3 capable renderer
* Meson
* Ninja
* GCC/Clang and standard build tools
* PNG/JPEG development libraries

NeoWall's upstream build system uses Meson and Ninja.

---

# Installation

## Arch Linux

Install the required build dependencies:

```bash
sudo pacman -S base-devel meson ninja wayland mesa libpng libjpeg-turbo \
    wayland-protocols libx11 libxrandr libxkbcommon
```

These are based on the upstream NeoWall Arch build requirements.

Clone this repository:

```bash
git clone https://github.com/jinu2004/neowall-patch.git
cd neowall-patch
```

Build:

```bash
meson setup build --buildtype=release
meson compile -C build
```

Install:

```bash
sudo install -Dm755 build/neowall /usr/bin/neowall
```

Verify:

```bash
neowall --version
```

## Installing the Custom Tag

The patched source can be checked out using:

```bash
git checkout v0.7.2-jinu-smooth
```

Then build:

```bash
meson setup build --buildtype=release
meson compile -C build
```

Install:

```bash
sudo install -Dm755 build/neowall /usr/bin/neowall
```

---

# Software Rendering

NeoWall requires a desktop OpenGL 3.3 context.

Some older Intel GPUs expose only OpenGL 2.1 through their hardware driver. In that situation, Mesa's LLVMpipe software renderer can provide the required OpenGL version.

Run NeoWall with:

```bash
LIBGL_ALWAYS_SOFTWARE=1 \
GALLIUM_DRIVER=llvmpipe \
neowall -f -v
```

With a configuration file:

```bash
LIBGL_ALWAYS_SOFTWARE=1 \
GALLIUM_DRIVER=llvmpipe \
neowall -f -v -c ~/.config/neowall/config.vibe
```

Look for output indicating that LLVMpipe is being used and that an OpenGL 3.3+ context was created.

## Example

```text
OpenGL renderer: llvmpipe
OpenGL version: 4.6
```

Software rendering is slower than hardware rendering, but it allows NeoWall to run on older hardware that cannot provide the required OpenGL version directly.

---

# Configuration

NeoWall uses:

```text
~/.config/neowall/config.vibe
```

A basic image configuration:

```text
default {
    path /home/user/Pictures/Wallpapers/
    mode fill
    duration 0
    shuffle false
    transition fade
    transition_duration 0.90
}
```

## Disable Automatic Wallpaper Cycling

Set:

```text
duration 0
```

This prevents NeoWall from automatically changing wallpapers.

This is useful when another program, such as an i3 theme switcher, controls wallpaper changes.

## Transition Duration

For example:

```text
transition_duration 0.90
```

Longer values produce slower transitions.

Examples:

```text
transition_duration 0.30
transition_duration 0.60
transition_duration 0.90
transition_duration 1.20
```

---

# Available Transitions

| Transition  | Configuration            |
| ----------- | ------------------------ |
| Fade        | `transition fade`        |
| Slide left  | `transition slide_left`  |
| Slide right | `transition slide_right` |
| Glitch      | `transition glitch`      |
| Pixelate    | `transition pixelate`    |
| Zoom        | `transition zoom`        |
| Side wipe   | `transition side_wipe`   |
| Circle      | `transition circle`      |
| Radial      | `transition radial`      |
| None        | `transition none`        |

The first five are from the original NeoWall transition system. The last four are added by this patch.

---

# NeoWall Commands

Start NeoWall:

```bash
neowall
```

Stop NeoWall:

```bash
neowall kill
```

Reload configuration:

```bash
neowall reload
```

Change to the next wallpaper:

```bash
neowall next
```

List wallpapers:

```bash
neowall list
```

Set a wallpaper by index:

```bash
neowall set 0
```

Show the current wallpaper:

```bash
neowall current
```

Pause wallpaper cycling:

```bash
neowall pause
```

Resume wallpaper cycling:

```bash
neowall resume
```

---

# Random Transitions

The transition can be randomized from a shell script.

Example:

```bash
transitions=(
    fade
    slide_left
    slide_right
    glitch
    pixelate
    zoom
    side_wipe
    circle
    radial
)

transition="${transitions[RANDOM % ${#transitions[@]}]}"
```

Then use the selected transition in the NeoWall configuration before changing the wallpaper.

This allows a theme switcher to randomly choose both:

1. the wallpaper
2. the transition effect

---

# i3 Theme Switcher

This patch works particularly well with an i3 theme-switching workflow.

A wallpaper helper can:

1. Find wallpapers belonging to the selected theme.
2. Select a random wallpaper.
3. Select a random transition.
4. Update the NeoWall configuration.
5. Reload NeoWall.
6. Set the selected wallpaper.

Example transition pool:

```bash
transitions=(
    fade
    slide_left
    slide_right
    glitch
    pixelate
    zoom
    side_wipe
    circle
    radial
)
```

The important part is that NeoWall does **not** need to be restarted for every wallpaper change.

---

# Persistent Wallpaper Queue

For a theme switcher using a persistent queue, wallpapers can be maintained in a directory such as:

```text
~/.cache/neowall/theme-queue/
```

The queue can contain the wallpapers from all themes.

For example:

```text
catppuccin-mocha--wallpaperflare.jpg
everforest--mountains.jpg
gruvbox-dark--wallpaper.jpg
nord--firewatch.jpg
tokyo-night--wallpaper.png
```

NeoWall can then address them by their zero-based queue index:

```bash
neowall list
```

Example:

```text
[0] catppuccin-mocha--wallpaperflare.jpg
[1] everforest--mountains.jpg
[2] gruvbox-dark--wallpaper.jpg
```

Then:

```bash
neowall set 1
```

---

# Building From Source

Clean build:

```bash
rm -rf build
meson setup build --buildtype=release
meson compile -C build
```

The resulting executable is:

```text
build/neowall
```

Test it without installing:

```bash
LIBGL_ALWAYS_SOFTWARE=1 \
GALLIUM_DRIVER=llvmpipe \
./build/neowall -f -v -c ~/.config/neowall/config.vibe
```

If the test succeeds, install:

```bash
sudo install -Dm755 build/neowall /usr/bin/neowall
```

---

# Development

Check the working tree:

```bash
git status
```

Build:

```bash
meson compile -C build
```

Check the latest commits:

```bash
git log --oneline -5
```

The custom transition implementation is located at:

```text
src/transitions/smooth.c
```

The additional transition types are registered in:

```text
src/transitions/transitions.c
src/config/config.c
```

The shared smooth-transition shader program is initialized and cleaned up in:

```text
src/render/render.c
```

The transition enum is defined in:

```text
include/neowall/output/output.h
```

The source file is included in:

```text
meson.build
```

---

# Git Version

This repository is based on:

```text
NeoWall v0.7.2
```

Custom patch tag:

```text
v0.7.2-jinu-smooth
```

The upstream version number is retained because this is a patch based on NeoWall v0.7.2 rather than an independently versioned NeoWall release.

---

# Troubleshooting

## OpenGL 3.3 Context Error

If you see:

```text
Failed to create OpenGL 3.3 context
```

try:

```bash
LIBGL_ALWAYS_SOFTWARE=1 \
GALLIUM_DRIVER=llvmpipe \
neowall -f -v
```

## NeoWall Does Not Change Wallpaper

Check the queue:

```bash
neowall list
```

Then manually test:

```bash
neowall set 0
```

Check the current wallpaper:

```bash
neowall current
```

## Test a Specific Transition

Edit:

```bash
nano ~/.config/neowall/config.vibe
```

Set, for example:

```text
transition circle
```

Then reload and advance:

```bash
neowall reload
neowall next
```

---

# Credits

This repository is a modification of NeoWall.

Original project:

https://github.com/1ay1/neowall

Original author and project maintainers retain their respective rights under the original project's license.

See the upstream repository for the original documentation, architecture, shader system, and project information.

# License

See the `LICENSE` file included in this repository.
