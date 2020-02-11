#!/usr/bin/env sh

THREAD_BUILD_DIR=./apps/thread/

for directory in `find $THREAD_BUILD_DIR -maxdepth 1 -mindepth 1 -type d -not -name .svn`
do
  echo $directory
  cd $directory
  make clean
  if make
  then
    echo Success!
  else
    cd -
    exit 1
  fi
  cd -
done
