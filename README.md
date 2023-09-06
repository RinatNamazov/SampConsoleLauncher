# SampConsoleLauncher

Command-line launcher for SA-MP.

Usage:
```
samp_launcher
    --host (-h) 127.0.0.1 (required)
    --port (-p) 7777 (required)
    --nickname (-n) Rinat_Namazov (optional)
    --password (-z) strong1pass (optional)
    --game-path (-g) \"D:\\Games\\GTA_SA\\gta_sa.exe\" (optional)
```

If the game-path and/or nickname is not specified, the program will attempt to find them in the registry.

Alternatively, you can simply enter the server address in the format host:port and optionally a nickname after it:
```
samp_launcher samp.rinwares.com:7777 Rinat
```

## License
The source code is published under GPLv3, the license is available [here](LICENSE).
