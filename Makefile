

PRE:=$(shell ./mk-headers.sh)
PRE:=$(shell ./mk-shell.sh)
SRC:=$(shell find -type f -name "*.c" -or -name "*.cpp" -or -name "*.asm")
HDR:=$(shell find -type f -name "*.h" -or -name "*.hpp")
RES:=$(shell find -type f -name "*.png")
DIR:=$(shell find -L -mindepth 1 -type d -not -wholename "*/.*" -printf "-I %P ")




CC=g++
LIBS    = -lm   
CPPFLAGS= -Werror $(DIR)

# optim
CPPFLAGS+=-O3



.PHONY : all
all : main



.PHONY : run
run : main
	./$^ ../cornell.obj




OBS+=main.o



$(OBS) : $(SRC) $(HDR)


main : $(OBS)
	$(CC) $(CPPFLAGS) -o $@ $(OBS) $(LIBS)


#DYN+=draw_scene_gl.h
#draw_scene_gl.h : scenes/scene.obj obj2c.sh
#	./obj2c.sh scenes/scene.obj > $@


.PHONY : clean cl
clean cl :
	file * | awk '/ELF/ { gsub(/:.*/,"") ; print }' | xargs -r rm
	rm -fR deps.inc dyn



.PHONY : rebuild re
rebuild re : clean all

