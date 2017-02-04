Das Skript "burn.bat" und die Firmware-Datei "bumpy_2_1_firmware.hex" müssen in das
hardware\tools\avr\bin Verzeichnis im Installationsverzeichnis der Arduino-Software verschoben werden
(falls dies nicht bereits geschen ist):

z.B. C:\Program Files (x86)\Arduino\hardware\tools\avr\bin

Der zu programmierende Mikrocontroller muss auf dem Sockel des Programmers (Arduino UNO) platziert 
werden und via ISP (mySmartUSB light) mit dem PC verbunden werden.

--> siehe Abbildung_ISP.jpg

Nun kann das Skript "burn.bat" ausgeführt werden. Der Mikrocontroller ist dann mit der aktuellen
Firmware programmiert.