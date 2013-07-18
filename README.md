# Plist Pad

An easy-to-use Property List / Plist file editor for Windows, OSX and Linux.

## Introduction

Plist Pad is a cross-platform Qt application which is intended to allow quick and easy editing of Property List / Plist files on platforms other than OSX. If you just want to use this application and don't need to worry about building from source, you can head over to the official site: <a href="http://www.johnwordsworth.com/plist-pad/">http://www.johnwordsworth.com/plist-pad/</a>.

## Warning

Please note that Plist Pad is currently in a very early stage of development. I’ve tested it on a dozen or so plist files of various sizes but <strong>you should assume that this application might mangle your Plist files</strong> until it has undergone more testing. It hopefully won’t (and shouldn’t), but if it does – don’t blame me, you were warned! I know it’s a faff, but you would be advised to save a backup of your plist file(s) before using PlistPad for the time being.

## Known Limitations

There are a few limitations with the current build of Plist Pad, the most notable are as follows:

* <strong>There is no undo feature... yet.</strong> You are advised not to hit the delete key when you have useful data selected.
* You can only open/save files in XML Plist format. I plan on adding support for binary Plist files, but it’s not there yet.

## Used Libraries

Plist Pad is built on the Qt Widget Library and uses images from the Open Icon Library.
