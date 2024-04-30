# Edge-Fire-Detection

## Overview Of Project

The project involves creating an automatic fire detection system that can be deployed wirelessly. The concept uses the BeagleBone Black V4L2 and OpenCV libraries to take images using a Logitech Brio and send them to an ML model hosted on a Python backend to predict fires. The images are sent over the internet using curl by connecting the BeagleBone to an Ethernet cable and using a public IP tunneled to the local host using ngrok which runs the backend. The curl response is then sent back to the BeagleBone which uses a traffic light kernel module to show the risk of fire.

## Hardware Required

1. Beaglebone Black Board
2. Logitech Brio 101 Webcam
3. Breadboard
4. LED Traffic Light Module
5. Jumper Wires
6. USB To TTL Cable

## Schematic
![Schematics](https://github.com/SuperMusey/Edge-Fire-Detection/raw/main/Schematics.jpg)
## Software Modules

### Backend
The backend portion of the project consists of the ImageReceiver.py python file and the Machine Learning model.

#### Configuration 

In order to deploy the Flask backend we must first run the following command locally:
`python3 ImageReceiver.py`

Once the Flask server has been launched locally, we use [ngrok](https://ngrok.com/) in order to generate a URL which can be used to serve API requests. To do this we use the 
following command:
`ngrok http 5000`

We use port 5000 because that is the port the Flask backend works on our devices. The port specified may need to be changed depending on your system. Once you have the ngrok server deployed you will need to keep a note of the dynamic URL that is generated.

### Beaglebone
The beaglebone portion of the project consists of the image capture portion and the kernel module to control the traffic lights.

#### COnfiguration
In order to run the capture_image.cpp we utilize the imageUse.sh file. First of all ensure that the `make` command has been run to build all the binaries. To do this first of all:
1. `git clone` the repository
2. Naviate to the `Edge-Fire-Detection/beaglebone` directory and run `make`
3. Navigate to the `Edge-Fire-Detection/beaglebone/km` directory and run `make`
4. Set the `UPLOAD_URL` environment variable to be equal to the dynamic URL generated by ngrok. This can be done by running `export UPLOAD_URL=dynamicURL`.
5. Once the variable is set, navigate to the `Edge-Fire-Detection/beaglebone` directory and run the `sudo ./imageUse.sh` command.

The Beaglebone will now be in image capture mode and will be sending images to the Flask backend which will then be sending back it's fire prediction to the board. 
