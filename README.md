# FastLED

This is just a fun little project in C++ using Dave's [help](https://www.youtube.com/watch?v=HlBR_jOpZv0&list=PLF2KJ6Gy3cZ7ynsp8s4tnqEFmY15CKhmH&ab_channel=Dave%27sGarage). <br/>
In this LED Project I use the ESP32. <br/>
Right now it supports 10 different effects: Comets, Bouncing balls, Fire, Stars (like in christmas) [There are different configuration for some of the effects which sums up to 10 effects]. <br/>
There's WIFI support which allows you to login onto a local Asynchronous Web Server from your phone/PC and communicate with the chip and thus change the effects on-the-fly <br/>
without tweaking the source code (just notice that you have to change the username and password to yours in "./include/WiFiHandler" file. <br/>
Also you can send to chip to deep-sleep-mode if you want to turn it "off" and stop wasting CPU cycles. <br/>

<img width="1045" alt="Screenshot_1" src="https://user-images.githubusercontent.com/72410493/130970716-502336c5-2c26-43eb-8958-8e9507fdb511.png">

* I have no experience in UI design, thus the website is nothing more than some buttons and event listeners.

