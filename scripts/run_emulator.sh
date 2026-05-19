#!/usr/bin/bash
if ! pgrep pulseaudio;then
    pulseaudio&
fi  
DISPLAY=:0 i3&
sleep 3
SDL_AUDIODRIVER=pulseaudio DISPLAY=:0 Vita3K -F *.vpk