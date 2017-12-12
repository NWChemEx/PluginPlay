#!/usr/bin/env python3
#Author: Ben Pritchard
import sys
import re
import mpmath as mp 

mp.dps=250
mp.mp.dps = 250

if len(sys.argv) != 2:
   print("Usage: format_CIAAW.py ciaawfile")
   quit(1)


path = sys.argv[1]


atomre = re.compile(r'^(\d+) +(\w\w*) +(\w+) +\[?(\d+)\]?\*? +(.*) *$') 
isore = re.compile(r'^(\d+)\*? +(\[?\d.*.*\]?) *$')
brange = re.compile(r'^\[([\d\.]+),([\d\.]+)\].*$')
buncertain = re.compile(r'^([\d\.]+)\((\d+)\)[a-z]*$')
bnum = re.compile(r'^([\d\d]+)$')

atommassline = re.compile(r'^(\d+) +(\w\w*) +(\w+) +(.*) *$')

def NumberStr(n):
  # Replace spaces
  s = n.replace(' ', '')

  # remove "exactly" for the carbon mass
  s = s.replace('(exactly)', '')

  # if only a number, put it three times
  m = bnum.match(s)
  if m: 
    s = "{:<25} {:<25} {:<25}".format(m.group(1), m.group(1), m.group(1))  

  # if parentheses uncertainty...
  m = buncertain.match(s)
  if m:
    # tricky. duplicate the first part as a string
    s2 = m.group(1)

    # but replace with all zero
    s2 = re.sub(r'\d', '0', s2)

    # now replace last characters
    l = len(m.group(2))
    s2 = s2[:len(s2)-l] + m.group(2)

    # convert to a float
    serr = mp.mpf(s2)
    scenter = mp.mpf(m.group(1))

    s = "{:<25} {:<25} {:<25}".format(mp.nstr(scenter, 18), mp.nstr(scenter-serr, 18), mp.nstr(scenter+serr, 18))

  # Replace bracketed ranges with parentheses
  m = brange.match(s)
  if m:
    slow = mp.mpf(m.group(1))
    shigh = mp.mpf(m.group(2))
    smid = (shigh + slow)/mp.mpf("2.0")
    s = "{:<25} {:<25} {:<25}".format(mp.nstr(smid, 18), mp.nstr(slow, 18), mp.nstr(shigh, 18))

  # just a dash?
  if s == "-":
    s = "{:<25} {:<25} {:<25}".format(0, 0, 0)


  return s






# First 5 lines are comments
filelines = [ x.strip() for x in open(path).readlines() ]

curatom = None

for line in filelines:
    matomre = atomre.match(line)
    misore = isore.match(line)
    matommass = atommassline.match(line)
    if matomre:
        curatom = "{:<5}   {:<5}".format(matomre.group(1), matomre.group(2))
        print("{}   {:<6}   {:<25}".format(curatom, matomre.group(4), NumberStr(matomre.group(5))))
    elif misore:
        print("{}   {:<6}   {:<25}".format(curatom, misore.group(1), NumberStr(misore.group(2))))
    elif matommass:
        curatom = "{:<5}   {:<5}".format(matommass.group(1), matommass.group(2))
        print("{}   {:<25}".format(curatom, NumberStr(matommass.group(4))))
    else:
        print(line) # comment lines, etc
