#!/bin/sh
# AUTO-GENERATED FILE, DO NOT EDIT!
if [ -f $1.org ]; then
  sed -e 's!^D:/cygwin/lib!/usr/lib!ig;s! D:/cygwin/lib! /usr/lib!ig;s!^D:/cygwin/bin!/usr/bin!ig;s! D:/cygwin/bin! /usr/bin!ig;s!^D:/cygwin/!/!ig;s! D:/cygwin/! /!ig;s!^M:!/cygdrive/m!ig;s! M:! /cygdrive/m!ig;s!^L:!/cygdrive/l!ig;s! L:! /cygdrive/l!ig;s!^K:!/cygdrive/k!ig;s! K:! /cygdrive/k!ig;s!^J:!/cygdrive/j!ig;s! J:! /cygdrive/j!ig;s!^I:!/cygdrive/i!ig;s! I:! /cygdrive/i!ig;s!^H:!/cygdrive/h!ig;s! H:! /cygdrive/h!ig;s!^G:!/cygdrive/g!ig;s! G:! /cygdrive/g!ig;s!^F:!/cygdrive/f!ig;s! F:! /cygdrive/f!ig;s!^E:!/cygdrive/e!ig;s! E:! /cygdrive/e!ig;s!^D:!/cygdrive/d!ig;s! D:! /cygdrive/d!ig;s!^C:!/cygdrive/c!ig;s! C:! /cygdrive/c!ig;' $1.org > $1 && rm -f $1.org
fi
