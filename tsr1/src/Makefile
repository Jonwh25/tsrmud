CC      = gcc32
PROF    = -O -g
NOCRYPT =
C_FLAGS =  -Wall $(PROF) $(NOCRYPT)
L_FLAGS =  $(PROF)
LIBRARY =

O_FILES = act_comm.o act_enter.o act_info.o act_move.o act_obj.o act_wiz.o \
          alias.o arena.o ban.o bit.o comm.o const.o db.o db2.o effects.o \
	  finger.o fight.o flags.o  handler.o healer.o interp.o \
	  lookup.o lore.o magic.o magic2.o magic3.o mem.o mobguard.o \
	  mob_cmds.o mob_prog.o music.o note.o olc.o olc_act.o olc_help.o \
	  olc_mpcode.o olc_save.o quest.o recycle.o remort.o save.o scan.o \
	  skills.o skills2.o special.o string.o tables.o update.o 

tsr: $(O_FILES)
	rm -f tsr 
	$(CC) $(L_FLAGS) -o tsr $(O_FILES) $(LIBRARY)

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<
