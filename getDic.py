#!/usr/bin/env python
# -*- coding: utf-8 -*-

import re, sys
verbes = {}
total = 0

f = open(sys.argv[1])
txt = f.read().split('\n')
for l in txt:
    l = l.split('\t')
    if len(l) > 4:
        if l[3][0] == 'v' :
            if ('-' in l[1]) or ('\'' in l[1]):
                continue # passer verbe composé
            if l[4] == 'infi': continue # passer verbe à l'infinitif
            if not (l[2] in verbes): verbes[l[2]] = []
            if l[1] in l[2]: continue # doublons 343517 -> 321497
            verbes[l[2]].append(l[1])
            total += 1

for verbe in verbes:
    nverbes = len(verbes[verbe])
    print nverbes, verbe,
    for i in range(nverbes): print verbes[verbe][i],
    print
