# obnoxious framework ![alt text](https://i.imgur.com/ZXcfFYJ.png) ![alt text](https://i.imgur.com/RHlmKYL.png) ![alt text](https://i.imgur.com/1OKJ96Z.png) ![alt text](https://i.imgur.com/RPz95ve.png)
Open-source cheat for Counter-Strike: Global Offensive game.

## Menu
![alt text](https://i.imgur.com/QmH3wP5.png)
![alt text](https://i.imgur.com/jtTu1ou.png)
![alt text](https://i.imgur.com/tdhyS8H.png)
![alt text](https://i.imgur.com/L28klLw.png)
![alt text](https://i.imgur.com/TGTcDVY.png)
![alt text](https://i.imgur.com/2v7UHIj.png)
![alt text](https://i.imgur.com/VkFt8h7.png)
![alt text](https://i.imgur.com/3JJZYcG.png)
![alt text](https://i.imgur.com/8fxBVcO.png)
![alt text](https://i.imgur.com/9aIba3N.png)

## Getting started

### Prerequisites
Microsoft Visual Studio 2017 15.9.4 and Windows SDK 10.0.17134.0 (lower should work too but requires change in project settings) are required in order to compile obnoxious framework. If you don't have ones, you can download VS [here](https://visualstudio.microsoft.com/) (Windows SDK is installed during Visual Studio Setup).

### Cloning
The very first step in order to compile obnoxious framework is to clone this repo from GitHub to your local computer:
```
git clone https://github.com/Lumm1t/obnoxious-framework.git
```

### Compiling from source

When you have equiped a copy of source code, next step is opening **obnoxious framework.sln** in Microsoft Visual Studio 2017.

Then change build configuration to `Release | x86` and simply press **Build solution**.

If everything went right you should receive `obnoxious.dll`  binary file.

### Loading / injecting into game process

Open [DLL injector](https://en.wikipedia.org/wiki/DLL_injection) and inject `obnoxious.dll` into `csgo.exe` process.

Press `INSERT` key when injected and focused on CS:GO window.
