#!/bin/bash

rsync -avP -e ssh --exclude '*~' --exclude '.svn' --delete * whiteh12,gcead@web.sourceforge.net:htdocs/
