#!/bin/bash

downloadDir=pages

indexFile=links.txt

cacheManagerDir=cache-manager

cacheManagerBin=$cacheManagerDir/bin/cache-manager

cacheDb=./cache.db
cacheIndex=./cache.idx

rm -rf $downloadDir;

mkdir -p $downloadDir;

make -C $cacheManagerDir;

while read p; do
  filename=$(printf '%s' $p | md5sum | cut -d ' ' -f 1)
  echo "Baixando: $p em $filename";
  curl $p > "$downloadDir/$filename.html";
  $cacheManagerBin -d $cacheDb -t $cacheIndex -i -k $p -I "$downloadDir/$filename.html"

done < $indexFile
