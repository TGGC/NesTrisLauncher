## Overview
A launcher for NES Tetris Emulators which want to transmit data to https://github.com/timotheeg/nestrischamps rooms. Creates all files necessary and automatically launches the emulators with correct connection settings and minimal manual interaction. Connection data can be passed in via cmd line. When the launcher is installed with an NSIS based installer, it will be registered to an URL protocol an can be launched directly from the browser.

## Getting Started

- download the installer and follow the setup
- start application
- choose emulator type
- locate the emulator executable and ROM you want to use
- save settings

## Establish connection to Nestrischamps room

- obtain connection info from http://nestrischamps.io/
- either use URL protocol to start application or start and put connection info manually into the settings
- hit "Launch Emulator"

### FCEUX
- after the emulator starts up, just hit connect
- the lua console will print a "Connected successfully!"

### Mesen
- to correct for a problem with the Mesen Working Dir, a copy of the emulator and is place in the documents folder
- you will automatically connect to the Nestrischamps room
- the lua console will print a "Connected successfully!"

### Bizhawk
- you will automatically connect to the Nestrischamps room
- the lua console will print a "Connected successfully!"

### Generic
- the generic setting can be used to launch any application/script you want
- it will pass 3 parameters, the path of the ROM the connection info and the folder where the lua scripts are located

## Easy Connect Info
The "Easy Connect Info" is meant to encapsulate all necessary information to connect to the websocket at nestetrischamps.io. It can contain a number of "key=value" pairs which are seperated by a "&". It usually looks something like "v=2&host=XXX&s=YYY", but can also be shortened. E.g. "v=1" sends to the players private room, if his DEFAULTSECRET is set up correctly.

### "v" -> URL Version
- "v=1" -> ws://nestrischamps.io/ws/room/producer
- "v=2" -> ws://nestrischamps.io/ws/room/u/{host}/producer

### "host" -> Twitch Username of the Host
Can be left out when not URL Version 2 is used.

### "s" -> Secret
The player needs to use his private secret to identify himself, can be found on https://nestrischamps.io/settings