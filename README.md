# Project-Team-5
#### Bijie Qiu
#### Manvitha Challagundla
#### Raghvendra Dixit
#### Shilakha Dawar

## Smart House

Home Assistant allows you to control all your devices without storing any of your data in the cloud. Its an home automation hub where different devices with different protocols communicate with each other. This means you can link all your devices in just one place and build cool automations based on the state of all your devices.

There are different ways to install Home Assistant. You can run it on your computer or use a Raspberry Pi to act as an exclusive home automation hub

We have attempted to connect multiple devices and sensors 

The UI looks like as follows :
![screen shot 2017-12-11 at 6 44 53 pm](https://user-images.githubusercontent.com/32425619/33865151-45ac9a56-dea5-11e7-963e-31044be185cb.png)

Here user can control everything anywhere to make sure that one is completely aware of any activity going around at one’s home.
The UI shown above is admin controlled. The UI can be configured specifically for touch enabled devices which is as follows:

Dashboard intended for wallmount:
![ha](https://user-images.githubusercontent.com/20053808/33865930-c93e019a-dea8-11e7-8db1-7491e580455b.PNG) 

Homeassistant keeps the Logs and Data values for all the registered devices at home and the attached sensors:
![screen shot 2017-12-11 at 6 46 11 pm](https://user-images.githubusercontent.com/32425619/33865463-b4c5e55e-dea6-11e7-833b-1c9ecea96325.png)

## Architecture Diagram for the Samrt Home:
![architecture](https://user-images.githubusercontent.com/32425619/33865056-ce0b5640-dea4-11e7-8f9b-0f9b34b82080.png)

## Encryption:
* We used the Let's Encrypt open source service to get the SSL/TLS encryption for our Homeassistant URL.
* We have also disabled the brute froce possiblity by banning the IP to access the URL if it exceedes the allowed number of attempts for login.

## Cloud MQTT Server:
* We used the free cloud hosting (Cloud MQTT) to publish and subscribe messages between attached devices, sensors and Homeassistant.

Make your smart home happen today with the Home Assistant. Getting started has never been this easy.

