#XBAT
A simple X window that displays the battery percentage in the form
of a pretty colored graph

##options
````
-w [int] : window width (includes border)
-bw [int]: border width (subtracted from total width)
-x [int] : x position (0 is left side of monitor)
-y [int] : y position (0 is top of monitor)
-bc #[hex] : hexadecimal HTML color ID
-bat [BAT1 / BAT0 ...] battery ID. Find yours in /sys/class/power_supply
````
