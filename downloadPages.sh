#!/bin/bash

pastaParaSites=pages

arquivoIndices=links.txt

cacheManagerDir=cache-manager

cacheManagerBin=$cacheManagerDir/bin/cache-manager

cacheDb=./cache.db
cacheIndex=./cache.idx

rm -rf $pastaParaSites;

mkdir -p $pastaParaSites;

make -C $cacheManagerDir;

while read p; do
  filename=$(printf '%s' $p | md5sum | cut -d ' ' -f 1)
  echo "Baixando: $p em $filename";
  curl $p > "$pastaParaSites/$filename.html";
  $cacheManagerBin -d $cacheDb -t $cacheIndex -i -k $p -I "$pastaParaSites/$filename.html"

done < $arquivoIndices
