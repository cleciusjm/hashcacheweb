#!/bin/bash

pastaParaSites=pages

arquivoIndices=links.txt

rm -rf $pastaParaSites;

mkdir -p $pastaParaSites;

while read p; do
  filename=$(printf '%s' $p | md5sum | cut -d ' ' -f 1)
  echo "Baixando: $p em $filename";
  curl $p > "$pastaParaSites/$filename.html";
done < $arquivoIndices
