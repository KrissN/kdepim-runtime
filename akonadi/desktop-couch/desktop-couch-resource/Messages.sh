#! /usr/bin/env bash
$EXTRACTRC `find . -name \*.ui` `find . -name \*.kcfg` >> rc.cpp || exit 11
$XGETTEXT *.cpp -o $podir/akonadi_desktop-couch_resource.pot
