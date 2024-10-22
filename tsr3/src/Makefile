CC      = gcc34
PROF    = -O -ggdb
NOCRYPT =
#Uncomment the next line if you want request support
#DBUGFLG = -DREQUESTS
C_FLAGS = -g3 -Wall $(PROF) $(NOCRYPT) $(DBUGFLG)
L_FLAGS = $(PROF)
LIBS=-lm -lcrypt

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o comm.o const.o \
          db.o fight.o handler.o magic.o save.o special.o update.o \
          bit.o mem.o olc.o olc_act.o olc_save.o string.o mob_commands.o \
          mob_prog.o gr_magic.o chat_new.o interp.o\
	  chat_act.o ore_prog.o quest.o raceskill.o act_room.o track.o 

C_FILES = act_comm.c act_info.c act_move.c act_obj.c act_wiz.c comm.c const.c \
	  db.c fight.c handler.c magic.c save.c special.c update.c \
	  bit.c mem.c olc.c olc_act.c olc_save.c string.c mob_commands.c \
	  mob_prog.c gr_magic.c chat_new.c interp.c \
	  chat_act.c ore_prog.c quest.c raceskill.c act_room.c track.c 

H_FILES = merc.h

all:
	make tsr 

tsr: $(O_FILES)
	@echo "[1;35mBuilding [1;31mTSR [1;32mIII[0;37m"
	rm -f tsr 
	$(CC) $(L_FLAGS) -o tsr $(O_FILES) ${LIBS}
	chmod g+w tsr 
	chmod g+w $(O_FILES)

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<

clean:
	rm -f $(O_FILES)
