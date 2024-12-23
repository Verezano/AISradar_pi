#! /bin/bash

# This script refactors this plugin into a new plugin with a name of your choice.
# To rename the plugin to "catsarebest", run `bash make-new-plugin.sh catsarebest`.

newname="$1"
Newname="$(echo "${newname^}")"
NEWNAME="$(echo "${newname^^}")"

grep -rl testplugin . | grep -v .git | while read name; do
  sed -e "s+testplugin+$newname+g" -i "$name";
done

grep -rl Testplugin . | grep -v .git | while read name; do  
  sed -e "s+Testplugin+$Newname+g" -i "$name";
done 

grep -rl TESTPLUGIN . | grep -v .git | while read name; do  
  sed -e "s+TESTPLUGIN+$NEWNAME+g" -i "$name";
done 

find . -name "*testplugin*" | grep -v .git | while read name; do
  mv "$name" "$(echo "$name" | sed -e "s+testplugin+$newname+g")"
done

find . -name "*Testplugin*" | grep -v .git | while read name; do
  mv "$name" "$(echo "$name" | sed -e "s+Testplugin+$Newname+g")"
done
