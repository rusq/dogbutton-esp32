===================
  Dog Button, The  
===================

Press the button on your ESP32 and receive a telegram bot message.

After reading this text, you will understand that the alpha and omega,
the beginning and the end, most important thing, and a meaning of life

                          is a button.


Prerequisitage
--------------
You need:
1. A button (or M5AtomLite, it has a couple of those)
2. A telegram bot (get token from https://t.me/botfather)
3. Wifi SSID/Password
4. A neighbour's dog that barks.
5. OCD on listening to the dog barking.

Usage
-----

1. Create "include/secrets.h" by copying "include/secrets.h.sample", and update the values.
2. Build/Upload to the ESP32.
3. Press the button.
4. ...
5. Receive an message

Optional:

6. Export JSON from the bot chat
7. Get the timestamps
8. Record neighbour's dogs barking
9. Create a program that replays the barks with the time intervals taken from the origial timestamps
...
?. PROFIT!!!

Features
--------
If the button is not pressed for (roughly) 60 seconds, ESP32 is put to a light sleep mode.
Press the button to wake it up and send a message.

How to build the project
------------------------

Originally built for M5Atom-Lite S3, but should work with any ESP32,
as long as you have a button, or two.

Get the platformio: https://docs.platformio.org/en/latest/

If you don't have a button, get one, now.

License
-------

WTFPL.