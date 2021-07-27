# FastLED

This is just a fun little project in C++ using Dave's help: https://www.youtube.com/watch?v=HlBR_jOpZv0&list=PLF2KJ6Gy3cZ7ynsp8s4tnqEFmY15CKhmH&ab_channel=Dave%27sGarage.
In this LED Project I use the ESP32.
Right now it supports 10 different effects: Comets, Bouncing balls, Fire, Stars (like in christmas) [There are different configuration for some of the effects which sums up to 10 effects].
There's WIFI support which allows to login onto a local Asynchronous Server on your phone/PC and communicate with the chip and thus change the effects on-the-fly
without tweaked the source code.
Also you can send to chip to deep-sleep-mode if you want to turn it "off" and stop wasting CPU cycles.
