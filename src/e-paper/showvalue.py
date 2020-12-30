#!/usr/bin/python3
# -*- coding:utf-8 -*-

import sys
import os
import logging
logging.basicConfig(level=logging.DEBUG)

print(__file__)

picdir = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'pic')
libdir = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'lib')
logging.debug(libdir)
if os.path.exists(libdir):
    sys.path.append(libdir)
    logging.debug('libdir set')

from waveshare_epd import epd2in13b_V3
import time
from PIL import Image,ImageDraw,ImageFont
import traceback
import urllib.request
import datetime

try:
    logging.info("epd2in9b V3 Demo")
    epd = epd2in13b_V3.EPD()
    logging.info("init and Clear")
    while True:
       epd.init()
#      epd.Clear()
#      time.sleep(1)

       w = epd.width
       h = epd.height
       logging.debug("width " + str(w) + " height " + str(h) )

       font18 = ImageFont.truetype(os.path.join(picdir, 'Font.ttc'), 18)
       blackimage = Image.new('1', (epd.height, epd.width), 255)
       ryimage = Image.new ('1', (epd.height, epd.width), 255)
       draw = ImageDraw.Draw(blackimage)
       drawry = ImageDraw.Draw(ryimage)
       logging.info("image created")
       try:
          contents = urllib.request.urlopen("http://192.168.2.49:8080/info/version").read().decode('utf-8')
       except Exception:
          logging.exception('http error')
          draw.text((0, 0), "Fehler", font=font18, fill=0, align='left')
       else:
          draw.text((0, 0), contents, font=font18, fill=0, align='left')
       rpi_time = datetime.datetime.now().replace(microsecond=0)
       zeitstring = str(str.encode('Gemessen %02d:%02d:%02d' % (int(rpi_time.strftime('%H')), int(rpi_time.strftime('%M')), int(rpi_time.strftime('%S')))), "utf-8")
       draw.text((0, 20), zeitstring, font=font18, fill=0, algin='left')
#      drawry.arc((140, 50, 190, 100), 0, 360, fill = 0)
       logging.info("tranfer data")
       epd.display(epd.getbuffer(blackimage), epd.getbuffer(ryimage))

       logging.info("Goto Sleep...")
       epd.sleep()
       time.sleep(10)

    epd.Dev_exit()
except IOError as e:
    logging.info(e)

except KeyboardInterrupt:
    logging.info("ctrl + c:")
    epd.Dev_exit()
    exit()
