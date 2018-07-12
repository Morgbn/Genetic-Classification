#!/usr/bin/env python
# -*- coding: utf-8 -*-

# def rmFrPlural(word):
#     word = list(word)
#     if (len(word) > 4) :
#         if word[-1]=='x':
#             if word[-2]=='u' and word[-3]=='a':
#                 word[-2]='l'
#             word = word[:-1]
#     if word[-1] == 's': word = word[:-1]
#     return ''.join(word)
#
# def rmFrFem(word):
#     word = list(word)
#     if (len(word) > 0 and word[-1]=='r'):
#         word = word[:-1]
#     if (len(word) > 0 and word[-1]=='e'):
#         word = word[:-1]
#     if (len(word) > 1 and word[-1]=='\xa9' and word[-2] == '\xc3'): # é
#         word = word[:-2]
#     if (len(word) > 1 and word[-1] == word[-2]):
#         word = word[:-1]
#     return ''.join(word)

import re
verbes = {}
total = 0
f = open("lexique-dicollecte-fr-v6.2.dic")
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


# while txt[0][0:2] != 'id':
#     txt = txt[1:] # passer entête
# txt = txt[1:]
# for l in txt:
#     l = l.split('\t')
#     if len(l) < 2: continue
#     if not re.search('\d+|/|\'|-', l[1]): # pas de chiffre, pas de /, ni de ' ou de -
#         if (l[2] == "Ω" or l[2] == "ℓ" or l[2] == "Ω"): continue;
#         if (l[1] == l[2]): print "%s 0" % l[1] # les mêmes
#         elif (l[1] == rmFrFem(rmFrPlural(l[2]))): print "%s 0" % l[1] # le même ms au pluriel et/ou féminin
#         else: print "%s %s" % (l[1], l[2])
