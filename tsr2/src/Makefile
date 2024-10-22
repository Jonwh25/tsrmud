CC      = gcc
PROF    =
NOCRYPT =
#Uncomment the next line if you want request support
#DBUGFLG = -DREQUESTS
C_FLAGS = -g3 -Wall $(PROF) $(NOCRYPT) $(DBUGFLG)
L_FLAGS = $(PROF)

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o boards.o \
	  build.o clans.o comm.o comments.o const.o db.o deity.o fight.o \
	  handler.o hashstr.o ibuild.o id.o interp.o magic.o makeobjs.o \
	  mapout.o misc.o mpxset.o mud_comm.o mud_prog.o player.o requests.o \
	  reset.o save.o shops.o skills.o special.o tables.o track.o update.o \
	  grub.o alias.o note.o remort.o quest.o

C_FILES = act_comm.c act_info.c act_move.c act_obj.c act_wiz.c boards.c \
	  build.c clans.c comm.c comments.c const.c db.c deity.c fight.c \
	  handler.c hashstr.c ibuild.c id.c interp.c magic.c makeobjs.c \
	  mapout.c misc.c mpxset.c mud_comm.c mud_prog.c player.c requests.c \
	  reset.c save.c shops.c skills.c special.c tables.c track.c update.c \
	  grub.c alias.c note.c remort.c quest.c

H_FILES = mud.h bet.h

all:
#	co $(H_FILES)
	make tsr 
#	rm -f $(H_FILES)

tsr: $(O_FILES)
	rm -f tsr
	$(CC) $(L_FLAGS) -o tsr $(O_FILES)
	chmod g+w tsr 
	chmod g+w $(O_FILES)

.c.o: mud.h
	$(CC) -c $(C_FLAGS) $<

clean:
	rm -f $(O_FILES)
