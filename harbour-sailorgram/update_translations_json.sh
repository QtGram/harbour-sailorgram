#!/bin/bash

url="http://www.transifex.com/api/2/project/sailorgram/languages/"
path="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
transifexrc="$HOME/.transifexrc"
tlogin=""
tpassword=""

if [ "$#" -ne 2 ]; then
    if [ -f $transifexrc ]; then
        echo "Using credentials from $transifexrc"
        tlogin=$(echo $(awk -F "=" '/username/ {print $2}' $transifexrc))
        tpassword=$(echo $(awk -F "=" '/password/ {print $2}' $transifexrc))
    else
        echo -e "\n    Usage: update_translations_json <user> <password>\n"
        exit
    fi
else
    tlogin=$1
    tpassword=$2
fi

echo "Getting $url"
curl -L --user $tlogin:$tpassword -X GET $url > $path/translations/translations.json
echo "Done!"
