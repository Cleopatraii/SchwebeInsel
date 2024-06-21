# SchwebeInsel

## Einführung
Unser Projekt präsentiert eine Szene, in der eine schwebende Insel um einen Energiekristall rotiert. Auf der Insel befindet sich eine Windmühle, deren Flügel sich ebenfalls drehen. Die Szene ist in einem Skybox platziert, wobei sowohl die Insel als auch der Kristall Texturen und Phong-Beleuchtungseffekte verwenden. Zusätzlich haben wir Bloom-Effekte für den Kristall und Nebel-Effekte für die Insel implementiert. Benutzer können die Kameraposition über die Tastatur anpassen und die Skybox aus verschiedenen Blickwinkeln betrachten.

## Voraussetzungen
Listet die Software und Bibliotheken auf, die zur Ausführung des Programms erforderlich sind.
- OpenGL
- GLFW
- GLEW
- stb_image

### Installation
git clone git@github.com:Cleopatraii/SchwebeInsel.git
cd ../SchwebeInsel

### Erstellung des Projekts
Mit einem Makefile: 
make

## Programm starten
./cgIsland

## Steuerung

Erläutere, wie man mit deinem Programm interagiert, sobald es läuft. Füge Tastatur- und Maussteuerungen hinzu. Zum Beispiel:

- `W` um vorwärts zu bewegen
- `S` um rückwärts zu bewegen
- Maus zum Umschauen
- `ESC` zum Beenden des Programms


### Windows
1. Installiere [CMake](https://cmake.org/download/) und füge es zum `PATH` hinzu.
2. Öffne die Eingabeaufforderung und navigiere zum Projektverzeichnis.
3. Führe die folgenden Befehle aus:
   ```sh
   mkdir build
   cd build
   cmake ..
   cmake --build .
