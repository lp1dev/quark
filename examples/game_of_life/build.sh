PROJECT_NAME=`grep -Eo 'project\(.*\)' CMakeLists.txt | cut -d '(' -f 2 | cut -d ')' -f 1`
cmake .
make
zip -j -u $PROJECT_NAME.vpk ../../quark.js index.html ../../Sans.ttf style.css
