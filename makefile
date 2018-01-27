all: start-fbterm start-tmux bgterm

install: start-fbterm start-tmux bgterm
	cp start-fbterm ~/bin/
	cp start-tmux ~/bin/
	cp bgterm ~/bin/

uninstall:
	rm -f ~/bin/start-fbterm
	rm -f ~/bin/start-tmux
	rm -f ~/bin/bgterm

bgterm: bgterm.c

start-fbterm: start-fbterm.c

start-tmux: start-tmux.c
