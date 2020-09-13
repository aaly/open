# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#                                                                             #
#    Copyright (C) 2013 Chuan Ji <jichuan89@gmail.com>                        #
#                                                                             #
#    Licensed under the Apache License, Version 2.0 (the "License");          #
#    you may not use this file except in compliance with the License.         #
#    You may obtain a copy of the License at                                  #
#                                                                             #
#     http://www.apache.org/licenses/LICENSE-2.0                              #
#                                                                             #
#    Unless required by applicable law or agreed to in writing, software      #
#    distributed under the License is distributed on an "AS IS" BASIS,        #
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. #
#    See the License for the specific language governing permissions and      #
#    limitations under the License.                                           #
#                                                                             #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# xinitrc file for testing our window manager.

# 1. Start some programs to play with.
#xclock &
#xeyes &
sleep 2s && urxvt &
#sleep 5 && geany &
#sleep 7 && urxvt &
#sleep 3 && kate &
#sleep 5 && mpv '/home/dude/Downloads/Mina - Io vivrò senza te.mp4' &

#sleep 3s  && xcompmgr -CcfF -I-.010 -O-.03 -D1 -t-1 -l-3 -r4.2 -o.5 &
#sleep 4s  && xcompmgr -CcfF -I-.010 -O-.03 -D1 -t-1 -l-3 -r4.2 -o.5 &
#sleep 2 && xset m 100/1 4 &
#xterm &

##sleep 5s && rofi -show run -font "snap 10" -fg "#505050" -bg "#000000" -hlfg "#ffb964" -hlbg "#000000" -o 60 -opacity 80 &

#sleep 5 && killall xterm &
# 2cStart our window manager.
#exec /home/dude/build-Open-Desktop-Debug/Open
exec /home/dude/Open/Open
#exec awesome
#exec antiwm
#exec windowlab
#exec /home/dude/Open/test
