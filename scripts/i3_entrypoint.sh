tigervncserver -xstartup i3 -display :0 -verbose -localhost no
DISPLAY=:0 pulseaudio&
tail -f /root/.vnc/*.log