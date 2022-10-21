#!/bin/bash

echo "we are here"
pwd

cd ./src/Frontend/automation_ui

flutter build web -v --base-href /ui/

exitCode=$?
if [ $exitCode -ne 0 ] ; then
    echo "flutter give an error"
    exit $exitCode
fi

cd ./build/web

#cd ../../../../../bin/Linux/webpage/ui
echo "we are here"
pwd

cp -Rv * ../../../../../bin/Linux/webpage/ui