                              Jeti README
Install:
	edit the Makefile, and shange prefix to your liking.
	make
	sudo make install

Uninstall:
	sudo make uninstall

basic usage:
	closing: 
		ctrl+c (if it still dosent close, hit random key, there was stuff left in the input que)
		or simply push the 'X' button in the corner of your terminal ( Jeti should be able to handle this ;) )

	navigation:
		* Use right and left arrows to switch between windows or simply click in the window you want to use.
		* pushing left arrow in left window will force you back one folder, pushing rightarrow in right window will do the same.
		* Use up and down arrows to navigate thru files, or simply just click the file you want to handle.
		* Press 'Enter' or double left click a catalog you want to enter, or file you want jeti to take action on (see the jeti,rc file: type actions, below.)
		* use scroll to scroll the page up and down.
		* Push 'Esc', if you want to unhiglight the marker
		* Push 'Esc' twise, if you want to get on top of page.

	selecting files:
		*navigate to the file you want to select, and hit 'space'.
		*or simply right click the file.

	copying files:
		select the files you want to copy, navigate the other window to the directory you want the files to be copied to, 
			* and hit 'c'
			* or push down the left mouse button in the window your files are located (don't let go yet.) drag the mouse to the destination window, and let go. 

	moving files:
		select the files you want to move, navigate the other window to the directory you want the files to be moved to, 
			* and hit 'm'
			* or push down the right mouse button in the window your files are located (don't let go yet.) drag the mouse to the destination window, and let go.
		
	deleting files:
		select the files you want to remove,
			* and hit 'DELETE'

	the command window:
		opening and closing:
				*open by hitting ':'
				*and closing by hitting 'Esc'
				*Enter will also close, but execute the current command on the way out.

		basic usage: 
			almost everything written in this will be past down to your shell (bash or sh or what ever. This is done tru a system() call)
			* simply type a command and hit 'Enter'.
			* navigate thru history of commands with up and down arrows.
			example: making a directory:
				open thru hitting ':'
				write "mkdir newcatalog"
				push 'Enter'
				update the directory by double clicking '.' dir

		special variables:
			file variable "$f":
				if you first, select a number of files, and open the command window. You can now use the variable "$f" which jeti will replace with the file 				path and name. The command will be executed once fore every file 				selected. 
					Example: showing selected pictures in a slide show.
						"feh -F $f; sleep 3s"

			directory variable "$d1 and $d2":
				$d1 is the workdirectory of the current window. a '$d' without the number will result in $d1
				$d2 is the destination directory. It takes its path from the passive window. (It is the same path shown in the command widows bottom)
				The command will be executed once fore every file selected.

the jeti.rc files:
	install:
		"cd $XDG_CONFIG_HOME ; >jeti.rc"
		or "cd $XDG_CONFIG_HOME ; mkdir jeti.d ; cd jeti.d ; >whatever.rc ; chmod +x whatever.rc"
		or "cd ~ ; > .jeti.rc"

	misc:
		terminal.name: <terminal>
			for executing of scripts and other binarys.

		logfile.path&prio: <path> <priority int>
			a directory where the logfile will be stored when using -l, if non set it will put it in your current directory.
			Note place this option in the top of your rc, if you want the full log output, or ells it will miss printing stuff untill this is called.

		dialogwindow.reactions: <int>
			if this is set, jeti will ask for password or if you are shure

			1 activate dialog window when no access
			2 activate dialog window on all (on every thing)
			4 when no access, just put sudo infront (nonpassword sudo)
			8 ask for password (su -c)
			16 star password typing
			32 dont show password typing at all

			example: dialogwindow.reactions: 25
			this will ask for root password when no write access and star it when writing

		window.showhiden: <bool>
			0 : don't show hidden files at start up (default)
			1 : show hidden files at start up

		window.startdirectory: <directory>
			cwd : same directory as jeti was launched.
			"/directory/" : starts at directory
			"/" : start at rot, (default)

		window.workdir.orientation: <binary int> <type>
			where <binary int>:
				1 : left centerd
				2 : right centerd
				3 : center centration
				0 : placement on the top border
				4 : placement on left border
				8 : placement on right border
				12 : placement on bottomborder
			where <type>
				mirror : this will make the right windows workdirectory a mirror of the left
				leaf : this shows only the leaf directory
				fullpath : this shows the full path of your workdriectory

		window.shortcut.orientation: <binary int> <type>
			where <binary int>:
				1 : left centerd
				2 : right centerd
				3 : center centration (this is currently under construction)
				0 : placement on the top border
				4 : placement on left border
				8 : placement on right border
				12 : placement on bottomborder
			where <type>
				mirror : this will make the right windows shortcuts a mirror of the left

		window.files.orientation: <binary int> <type>
			where <binary int>:
				0 : down -> up
				1 : left -> right
				2 : right -> left
				3 : up -> down
			where <type>
				mirror : this will make the right windows filemovement a mirror of the left

		window.animations: <boolean>
			where <boolean>
				0 or false : no animations
				1 or true : animations is activated
			

	tabs:
		window.tab.left: <tabtype> <optional predefined lenght>
			
		window.tab.right: <tabtype> <optional predefined lenght>

		window.tab.center: <tabtype> <optional predefined lenght>

		Available <tabtype> are:
			selected :
				prints a '*' if file is selected.
			filename :
				print file name.
			presentation :
				if id3tags are available print id3 tags: %artist - %title - %album %year
				else print file name without '.filetype'.
			filetype :
				prints the file type.
			mode :
				prints the file read/wright status.
			size :
				prints the file size in whole bites or kilo bites or Mega.. and so on.
			nothing :
				prints a blank space.

	Coloring:
		window.color.<filetype>: <int>
			colorates files

			 available <filetypes> are:
				background :
					background color
				katalog :
					catalogs.
				executeble :
					files in executable mode.
				link :
					links
				fifo :
					fifos
				char :
					chars
				block :
					blocks
				error :
					fishy files
				selected :
					selected files
				highlight :
					activated stuff, for example witch window that is active
				borders :
					the ncurses box's, the frames
				bordertext :
					text that is on the border, for example the workdirectory
				default :
					all other files

			available colors (<int>) are:
				-1 : none / default color of terminal text.
				0 : black
				1 : red
				2 : green
				3 : orange
				4 : blue
				5 : purple
				6 : turquoise 
				7 : white or bright gray
				and apparently so on and so on, depending on what your terminal can handle

	type actions:
		file.type: "<command>" <filetype>,<filetype>..,<filetype>
			where <command> is, is the command to be executed when any of the file types are entered. The command will be followed by the directory and file 			name.

			use ' ' for the none filetype

			Variable "$fn" can be used to send file name as a parameter.
			Example:  file.type: "lxterminal --title=$fn -e vim " c,h,rc,' ',txt
				to sett vim border tittle to file name, when entering a text file.

	Shortcuts:
		window.shortcut: '<char>' "<direcory or shortcut variable>"
			creates a shortcut button in the left upper corner of the file windows.

			available <shortcut variable> are:
				$hidden :
					shows hidden files (files with a dot in front of it)
				$noexe : 
					ignores the 'x'  mode (executable mode) of files
				$mute :
					turn off sound effects.
				$passiv :
					teleports you to the same directory as the passive window.
					note, the same thing will hapen if you click on the workdirectory.
				$home :
					teleports you to the home directory of the current user.
				$exec <command>:
					executes a console command. here you can also use special variables $f and $d, NOTE: $d is the directory of the activ window in this case (see commandwindow for more information).

	sound effects:
		sound.enter.<action>: "<file path>"
			plays a sound, thru aplay, when a action happends

			 available <action> are:
				singledot
				doubledot
				kat
				executeble
				scrollupp
				scrolldown
				stepupp
				stepdown
				selectfile
				copyfiles
				movefiles
				removefiles
				shortcut
				startup
				quit
				fileaction
flags:
	--help or -h:
		shows helpmenu.

	--mute or -m:
		turns sound off.

	--config or -c <file.rc>:
		use an alternative configuration file.

	--version or -v:
		print version

	--log or -l <int>:
		creates a logfile called jeti.log if something in the priority range happened during runtime.

		available Priority's (<int>) are:
			0: no logging   
			1: ERRORS
			2: entering a large process or function
			3: entering a smaller process or function, ex. get_this()
			4: bigger blocks inside process or function, ex. "printing window decorations..."
			5: intestines of ifs and stuff, ex "is inside endofname_if"
			6: frequent stuff like graphics, ex on every row
			7: even more frequent stuff, ex on every character or pixel
		
about:
	author: Harri Leino
	when: 2016-2017
	why: 
		4 reasons:
			1. I have always wanted one of these
			2. just for fun.
			3. I try to get better at programing in C
			4. to give something back to the open society.

	License:
		this program is distributed under the terms of the GNU General Public License
