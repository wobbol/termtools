all: start-fbterm start-tmux

install: start-fbterm start-tmux
	cp start-fbterm ~/bin/
	cp start-tmux ~/bin/

uninstall:
	rm -f ~/bin/start-fbterm
	rm -f ~/bin/start-tmux

start-fbterm: start-fbterm.c

start-tmux: start-tmux.c
