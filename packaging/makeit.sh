#!/bin/bash

VERSION=`head -n 1 debian/changelog | grep -o "[0-9\\.]*" | head -n1`
echo $VERSION

rm -rf miceamaze-${VERSION}
tar zxvf miceamaze_${VERSION}.orig.tar.gz || exit 1
cp -R debian miceamaze-${VERSION}/ || exit 1
cd miceamaze-${VERSION} || exit 1

debuild -us -uc

