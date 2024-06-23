##  SchwebeInsel

## Einführung
Unser Projekt präsentiert eine strukturierte Szene (verschiedene Rationsgruppe), in der eine schwebende Insel um einen EnergieObjekt(Lichtkörper, Erste Lichtquelle) rotiert. Auf der Insel befindet sich eine Windmühle, deren Flügel (transparente Objekt) sich ebenfalls drehen. Die Szene ist in einem Skybox platziert (Verwendung einer Umgebungstexture), wobei die Insel Texturen (InselStein ist insbesondere mit MixTexturen) und Phong-Beleuchtungseffekte verwenden. Zusätzlich haben wir Bloom-Effekte für den EnergieObjektund und Nebel-Effekte für die Inselstein implementiert. Durch benutzerdefinierte Tastaturbefehle können Benutzer die Kameraposition anpassen (Interaktion), um die Skybox aus verschiedenen Blickwinkeln zu betrachten. Hinzu kommt noch eine zweite Lichtquelle, ein weiches gelbes Licht aus der Ferne. 

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

## Tests für alle Funktion
make test_funk
./test_funk

## Steuerung
Die Interaktion mit dem Programm erfolgt über die Tastatur. Hier sind die spezifischen Kontrollen für die Kamerasteuerung, um die Skybox aus verschiedenen Perspektiven zu betrachten:

- `W`: Bewegen der Kamera nach vorne, um die Sicht nach vorne zu verlagern.
- `S`: Bewegen der Kamera nach hinten, um die Sicht nach hinten zu verlagern.
- `A`: Bewegen der Kamera nach oben, um die Sicht nach oben zu verlagern.
- `D`: Bewegen der Kamera nach unten, um die Sicht nach unten zu verlagern.
- `Q`: Bewegen der Kamera nach links, um die Sicht nach links zu verlagern.
- `E`: Bewegen der Kamera nach rechts, um die Sicht nach rechts zu verlagern.

## Quelle
1.Ganz Island-3D-Model:
https://www.cgtrader.com/items/729337/download-page
  
2.Bloom: https://www.youtube.com/watch?v=um9iCPUGyU4

3.Bloom: https://learnopengl.com/Advanced-Lighting/Bloom

4.Texture-Picture1, “Greenplatform”: https://www.textures.com/download/Grass0027/8962

5.Texture-Picture2, “Stone für Island”: https://ambientcg.com/view?id=Ground068

6.Texture-Umgebung: https://ambientcg.com/view?id=MorningSkyHDRI002A

7.Datei-"stb_image.h": https://github.com/nothings/stb/blob/master/stb_image.h

##  Name der beteiligen Gruppenmitglieder
Yi Zheng, Shuxin Liu, Siyuan Zhu



