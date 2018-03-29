
## Useful configurations

Refer  to [this](https://www.shortcutfoo.com/blog/top-50-vim-configuration-options/)

`vim ~/.vimrc`

	# new lines inherit the indentation of previous lines
	set autoindent 
	# convert tabs to spaces
	set expandtab
	# when shifting lines, round the indentation to the nearest multiple of shiftwidth
	set shiftround
	# when shifting, indent using four spaces
	set shiftwdith=4
	# insert tabstop number of spaces when the tab key is pressed
	set smarttab
	# indent using four spaces
	set tabstop=4
	
	# enable search highlighting
	set hlsearch
	# ignore case when searching
	set ignorecase
	# automatically switch search case-sensitive when search query contains an uppercase letter
	set smartcase
	
	# enable syntax highlighting
	syntax enable
	
	# show line numbers on the sidebar
	set number
	# set the window title, reflecting the file currently being edited
	set title