# SchwebeInsel

## Einführung
Unser Projekt präsentiert eine Szene, in der eine schwebende Insel um einen Energiekristall rotiert. Auf der Insel befindet sich eine Windmühle, deren Flügel sich ebenfalls drehen. Die Szene ist in einem Skybox platziert, wobei sowohl die Insel als auch der Kristall Texturen und Phong-Beleuchtungseffekte verwenden. Zusätzlich haben wir Bloom-Effekte für den Kristall und Nebel-Effekte für die Insel implementiert. Durch benutzerdefinierte Tastaturbefehle können Benutzer die Kameraposition anpassen, um die Skybox aus verschiedenen Blickwinkeln zu betrachten.

## Voraussetzungen
Listet die Software und Bibliotheken auf, die zur Ausführung des Programms erforderlich sind.
- C Compiler
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
Die Interaktion mit dem Programm erfolgt über die Tastatur. Hier sind die spezifischen Kontrollen für die Kamerasteuerung, um die Skybox aus verschiedenen Perspektiven zu betrachten:

- `W`: Bewegen der Kamera nach vorne, um die Sicht nach vorne zu verlagern.
- `S`: Bewegen der Kamera nach hinten, um die Sicht nach hinten zu verlagern.
- `A`: Bewegen der Kamera nach oben, um die Sicht nach oben zu verlagern.
- `D`: Bewegen der Kamera nach unten, um die Sicht nach unten zu verlagern.
- `Q`: Bewegen der Kamera nach links, um die Sicht nach links zu verlagern.
- `E`: Bewegen der Kamera nach rechts, um die Sicht nach rechts zu verlagern.




