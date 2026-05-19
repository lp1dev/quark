#!/usr/bin/bash
if [ "$#" -lt 1 ];then
    echo -n "Usage: $0 quark_project_directory\n\t(Should be accessible in the Docker container)"
else
    cd $1 && cmake .
fi
