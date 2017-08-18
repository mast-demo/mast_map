#!/usr/bin/python
import os
import sys
print 'Arguments:', str(sys.argv)
import glob
imagefiles = sorted(glob.glob('*.jpg'))
posefiles = sorted(glob.glob('*.txt'))
print imagefiles[0]
print posefiles[0]
# earth radius in meters at equator
rad = 6378137.0
for i in range(0,len(imagefiles)):
  print imagefiles[i], ", ", posefiles[i]
  f = open(posefiles[i], "r")
  l = f.readlines()[5]
  x = float(l.split()[2])
  y = float(l.split()[3])
  lon = x / rad * 180.0 / 3.14159
  lat = y / rad * 180.0 / 3.14159
  print lat, ",", lon
  os.system("exiftool -GPSLongitude="+str(lon)+" -GPSLatitude="+str(lat)+" -GPSAltitude=0.0 "+imagefiles[i])
