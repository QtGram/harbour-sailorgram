#! /bin/sh

FILELIST=`find . -name "*.md"`
DEST_PATH="html"

mkdir $DEST_PATH
for MDFILE in $FILELIST
do
    DEST_FILE=`basename $MDFILE .md`
    DEST_DIR=`dirname $MDFILE`

    DIRECTORY="$DEST_PATH/$DEST_DIR/"
    if [ ! -d "$DIRECTORY" ]; then
        mkdir "$DIRECTORY"
    fi
    
    DST="$DIRECTORY/$DEST_FILE.html"
    SRC="$DEST_DIR/$DEST_FILE.md"

    grip --export "$SRC" "$DST"
    sed -i 's/.md">/.html">/g' "$DST"
done
