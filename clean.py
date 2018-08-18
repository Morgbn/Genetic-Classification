#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
if len(sys.argv) < 1:
    print 'need a filename'
    exit(1)

f = open(sys.argv[1], "r");
txt = f.read()
f.close()

f = open(sys.argv[1], "w")

startHeader = 0
producedBy0 = 0
producedBy1 = 0
ready2print = 0
end = 0

for l in txt.split('\n'):
    if not l: continue
    if not end:
        if l[:12] == "*** START OF" \
        or l[:11] == "***START OF":                 startHeader = 1
        if startHeader and l[:11] == "Produced by": producedBy0 = 1
        elif producedBy0 and l == '\r':             producedBy1 = 1
        elif producedBy1 and l != '\r':             ready2print = 1
    if ready2print and l[:6] == "End of":           end = 1
    if ready2print and not end: f.write(l+'\n')

f.close()
