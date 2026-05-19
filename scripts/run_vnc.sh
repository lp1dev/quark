#!/usr/bin/bash
if [ "$#" -lt 1 ];then
    echo -n "Usage: $0 quark_project_directory\n\t(Should be accessible in the Docker container)"
else
    cd $1 && tigervncserver -xstartup /quark/quark -display :0 -verbose -localhost no -geometry 960x544
    tail -f /root/.vnc/*.log
fi
