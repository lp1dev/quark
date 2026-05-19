#!/usr/bin/bash
if [ "$#" -lt 1 ];then
    echo -n "Usage: $0 quark_project_directory\n\t(Should be accessible in the Docker container)"
else
    cd $1 && bash build.sh && \
    tigervncserver -xstartup "/quark/scripts/run_emulator.sh" -display :0 -verbose -localhost no
    tail -f /root/.vnc/*.log
fi
