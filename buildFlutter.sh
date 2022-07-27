#!/bin/bash

echo "we are here"
pwd

cd ./src/Frontend/automation_ui

flutter build web -v --base-href /ui/
