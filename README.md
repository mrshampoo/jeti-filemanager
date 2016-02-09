# jeti-filemanager
a linux totalcommander clone, made in ncurses

comply by executing ./comply

copy the jeti.rc to $HOME/.jeti.rc or $XDG_CONFIG_HOME/jeti.rc or $XDG_DATA_DIRS/jeti/jeti.rc

navigate with mouse or arrow-keys
right-clicking or hitting space on files, will mark them
double left-clicking or pushing enter, will enter a folder or execute a executable
left draging from one window to the next, or pushing 'c' will copy the marked files
right draging from one winddow to the next, or pushing 'm' will move the marked files
pushing 'delete' will remove marked files.
pushing 'Esc' will shut the program down. it is safe to push the 'X' in the corner for this do.
pushing ':' will bring upp a command window, ('Esc' will bring you back ) everething enterd here will be put in system(). $f will do be changed to every marked file (the enterd command will be executed once fore every file). arrow-up will give you history of the commands.
in the left corner of both windows are shourtcuts (if declared in the rc file), left-clilck them to enter. there are a few special shourtcuts here, se the jeti.rc fore more info.
you can probobly spot more stuff in the rc. a note on "presentation" vs "filetype" do, presentation will show you the id3tags on mp3s and stuff if sush exists.
