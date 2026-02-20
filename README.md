#Start of a method of displaying L298 on wokwi.
The diagram.json must include the cihp as a part and the gitgub dependencies

 { "type": "chip-modulel298", "id": "chip1", "top": XX, "left": YY, "attrs": {} }

 "dependencies": { "chip-modulel298": "github:DarwinWasWrong/wokwi-l298module-chip@6.0.0" }


eg
{
  "version": 1,
  "author": "Darwin WasWrong",
  "editor": "wokwi",
  "parts": [
    { "type": "wokwi-arduino-uno", "id": "uno", "top": 125.4, "left": -0.6, "attrs": {} },
    { "type": "chip-modulel298", "id": "chip1", "top": 29.82, "left": 340.8, "attrs": {} }
  ],
  "connections": [
    [ "chip1:ENA", "uno:10", "green", [ "h0" ] ],
    [ "chip1:IN1", "uno:9", "green", [ "h-182.4" ] ],
    [ "chip1:IN2", "uno:8", "green", [ "h-172.8" ] ],
    [ "chip1:IN3", "uno:7", "green", [ "h-163.2" ] ],
    [ "chip1:IN4", "uno:6", "green", [ "h-144" ] ],
    [ "chip1:ENB", "uno:5", "green", [ "h-134.4" ] ]
  ],
  "dependencies": { "chip-modulel298": "github:DarwinWasWrong/wokwi-l298module-chip@6.0.0" }
}
