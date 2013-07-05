#!/bin/bash

#rm -f miceamaze-latest.tar.gz
#wget http://www.miceamaze.org/source-packages/miceamaze-latest.tar.gz
#rm -rf temp
#mkdir temp
#cd temp || exit 1
#tar zxvf ../miceamaze-latest.tar.gz
#VERSION=`grep -o "[0-9\\.]*" miceamaze-*/src/Version.h`
#cp ../miceamaze-latest.tar.gz ../miceamaze_${VERSION}.orig.tar.gz

uscan --destdir . --verbose --force-download
