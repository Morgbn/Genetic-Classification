#!/bin/sh

dir='data-test'

mkdir -p $dir

wget --help | grep -q '\--show-progress' && \
  WOPT="-q --show-progress -O" || WOPT="-O "

wget $WOPT $dir/Aventures-d-Alice-au-pays-des-merveilles.txt https://www.gutenberg.org/files/55456/55456-0.txt
wget $WOPT $dir/Contes-merveilleux-Tome-I.txt http://www.gutenberg.org/cache/epub/18244/pg18244.txt
wget $WOPT $dir/De-la-terre-a-la-lune.txt http://www.gutenberg.org/cache/epub/38674/pg38674.txt
wget $WOPT $dir/Fables-de-La-Fontaine-Tome-1.txt http://www.gutenberg.org/cache/epub/17941/pg17941.txt
wget $WOPT $dir/Feu-de-joie.txt https://www.gutenberg.org/files/55733/55733-0.txt
wget $WOPT $dir/La-Hyene-Enragee.txt http://www.gutenberg.org/cache/epub/57425/pg57425.txt
wget $WOPT $dir/L-art-pendant-la-guerre-1914-1918.txt https://www.gutenberg.org/files/49586/49586-0.txt
wget $WOPT $dir/Les-contemplations.txt http://www.gutenberg.org/cache/epub/29843/pg29843.txt
wget $WOPT $dir/Les-fleurs-animees-Tome-1.txt https://www.gutenberg.org/files/54972/54972-0.txt
wget $WOPT $dir/Les-Fleurs-du-Mal.txt http://www.gutenberg.org/cache/epub/6099/pg6099.txt
wget $WOPT $dir/Nouvelles-et-Contes-pour-la-jeunesse.txt http://www.gutenberg.org/cache/epub/14309/pg14309.txt
wget $WOPT $dir/Oeuvres-completes-de-Guy-de-Maupassant.txt https://www.gutenberg.org/files/45312/45312-0.txt
wget $WOPT $dir/Oeuvres-completes-de-Paul-Verlaine.txt http://www.gutenberg.org/cache/epub/15112/pg15112.txt
wget $WOPT $dir/Vingt-mille-lieues-sous-les-mers.txt https://www.gutenberg.org/files/54873/54873-0.txt

for file in $dir/*.txt; do python clean.py $file; done

echo "Données d'essai téléchargées!"
