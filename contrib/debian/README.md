
Debian
====================
This directory contains files used to package safedeald/safedeal-qt
for Debian-based Linux systems. If you compile safedeald/safedeal-qt yourself, there are some useful files here.

## safedeal: URI support ##


safedeal-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install safedeal-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your safedeal-qt binary to `/usr/bin`
and the `../../share/pixmaps/safedeal128.png` to `/usr/share/pixmaps`

safedeal-qt.protocol (KDE)

