# Tonex One Controller: An open-source controller and display interface for the IK Multimedia Tonex One guitar pedal
This project uses a low-cost embedded controller (Espressif ESP32-S3) to form a bridge to the IK Multimedia Tonex One guitar pedal (which does not have native Midi capability.)

# Midi Command list

| Name | MIDI CC# | Value |
| ------------ | ---------- | --------------------- |
| Delay Position | 1 | Pre Amp: 0, Post Amp: 127 |
| Delay: Power | 2 | On: 127, Off: 0 |
| Delay Type | 3 | Digital: 0, Tape: 1 |
| Digital Delay: Sync | 4 | On: 127, Off: 0 |
| Digital Delay: Time | 5 | 0-127 |
| Digital Delay: Feedback | 6 | 0-127 |
| Digital Delay: Mode | 7 | Normal: 0, Ping-Pong: 64|
| Digital Delay: Mix | 8 | 0-127 |
| Tuner | 9 | Not Supported |
| Tap Tempo | 10 | Value Not used |
| Expression Pedal | 11 | Not Supported |
| Preset On | 12 | Not supported |
| Gate Position | 13 | First: 0, Post Amp: 127 |
| Gate Power | 14 | On: 127, Off: 0 |
| Gate Threshold | 15 | 0-127 |
| Gate Release | 16 | 0-127 |
| Gate Depth | 17 | 0-127 |
| Compressor Power | 18 | On: 127, Off: 0 |
| Compressor Threshold | 19 | 0-127 |
| Compressor Gain | 20 | 0-127 |
| Compressor Attack | 21 | 0-127 |
| Compressor Position | 22 | First: 0, Post Amp: 127 |
| Bass EQ | 23 | 0-127 |
| Bass Hz | 24 | 0-127 |
| Mid EQ | 25 | 0-127|
| Mid Q | 26 | 0-127 |
| Mid Hz | 27 | 0-127 |
| Treble EQ | 28 | 0-127 |
| Treble Hz | 29 | 0-127 |
| EQ Position | 30 | 0-127 |
| Mod Position | 31 | Pre Amp: 0, Post Amp: 127 |
| Mod Power | 32 | On: 127, Off: 0 |
| Mod Type | 33 | Chorus: 0, Tremolo: 1, Phaser: 2, Flanger: 3, Rotary: 4 |
| Chorus Sync | 34 | On: 127, Off: 0 |
| Chorus Rate | 35 | 0-127 |
| Chorus Depth | 36 | 0-127 |
| Chorus Level | 37 | 0-127 |
| Tremolo Sync | 38 | On: 127, Off: 0 |
| Tremolo Rate | 39 | 0-127 |
| Tremolo Shape | 40 | 0-127 |
| Tremolo Spread | 41 | 0-127 |
| Tremolo Level | 42 | 0-127 |
| Phaser Sync | 43 | On: 127, Off: 0 |
| Phaser Rate | 44 | 0-127 |
| Phaser Depth | 45 | 0-127 |
| Phaser Level | 46 | 0-127 |
| Flanger Sync | 47 | On: 127, Off: 0 |
| Flanger Rate | 48 | 0-127 |
| Flanger Depth | 49 | 0-127 |
| Flanger Feedback | 50 | 0-127 |
| Flanger Level | 51 | 0-127 |
| Rotary Sync | 52 | On: 127, Off: 0 |
| Rotary Speed | 53 | 0-127 |
| Rotary Radius | 54 | 0-127 |
| Rotary Spread | 55 | 0-127 |
| Rotary Level | 56 | 0-127 |
| Spring Reverb 1 Time | 59 | 0-127 |
| Spring Reverb 1 Predelay | 60 | 0-127 |
| Spring Reverb 1 Color | 61 | 0-127 |
| Spring Reverb 1 Mix | 62 | 0-127 |
| Spring Reverb 2 Time | 63 | 0-127 |
| Spring Reverb 2 Predelay | 64 | 0-127 |
| Spring Reverb 2 Color | 65 | 0-127 |
| Spring Reverb 2 Mix | 66 | 0-127 |
| Spring Reverb 3 Time | 67 | 0-127 |
| Spring Reverb 3 Predelay | 68 | 0-127 |
| Spring Reverb 3 Color | 69 | 0-127 |
| Spring Reverb 3 Mix | 70 | 0-127 |
| Room Reverb Time | 71 | 0-127 |
| Room Reverb Predelay | 72 | 0-127 |
| Room Reverb Color | 73 | 0-127 |
| Room Reverb Mix | 74 | 0-127 |
| Plate Reverb Time | 76 | 0-127 |
| Plate Reverb Predelay | 77 | 0-127 |
| Plate Reverb Color | 78 | 0-127 |
| Plate Reverb Mix | 79 | 0-127 |
| Reverb Power | 75 | On: 127, Off: 0 |
| Spring Reverb 4 Time | 80 | 0-127 |
| Spring Reverb 4 Predelay | 81 | 0-127 |
| Spring Reverb 4 Color | 82 | 0-127 |
| Spring Reverb 4 Mix | 83 | 0-127 |
| Reverb Type | 85 | Spring 1-4: 0-3, Room: 4, Plate: 5 |
| Reverb Position | 84 | Post Amp: 0, Last: 127 |
| Preset Down | 86 | Value not used |
| Preset Up |  87 | Value not used |
| BPM | 88  | 0-127 |
| Bank Up | 89 | Not supported |
| Bank Down | 90 | Not supported |
| Tape Delay Sync | 91 | On: 127, Off: 0 |
| Tape Delay Time | 92 | 0-127 |
| Tape Delay Feedback | 93 | 0-127 |
| Tape Delay Mode | 94 | Normal: 0, Ping Pong: 64 |
| Tape Delay Mix | 95 | 0-127 |
| Global BPM | 99, 100 | 0-127 |
| Amp Model Gain | 102 | 0-127 |
| Amp Model Volume | 103 | 0-127 |
| Amp Model Mix | 104 | 0-127 |
| Presence | 106 | 0-127 |
| Depth | 107 | 0-127 |
| VIR Resonance | 108 | 0-127 |
| VIR Mic 1 | 109 | Condenser: 0, Dynamic: 1, Ribbon: 2 |
| VIR Mic 1 X | 110 | 0-127 |
| VIR Mic 1 Z | 111 | 0-127 |
| VIR Mic 2 | 112 | Condenser: 0, Dynamic: 1, Ribbon: 2 |
| VIR Mic 2 X | 113 | 0-127 |
| VIR Mic 2 Z | 114 | 0-127 |
| VIR Blend | 115 | 0-127 |
| Input Trim | 116 | 0-127 |
| Cab Sim Bypass | 117 | On: 127, Off: 0 |
| Global Tempo Source | 118 | Global: 127, Local: 0 |
| Global Tuning Reference | 119 | 0-127 |
| Select Preset | 127 | 0-19 |
