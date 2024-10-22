CC      = gcc
PROF    =
OPTOMIZ = -O2
DEBUG   = -g3
WARN    = -Wall
#C_FLAGS = $(PROF) $(OPTOMIZ) $(WARN) $(DEBUG)
C_FLAGS = -c -O -Wall -g3
L_FLAGS = -static -g
CRYPT   = -lcrypt
SYSTEM  =

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_room.o act_wiz.o \
          board.o bounty.o bit.o buffer.o chat_act.o chat_new.o comm.o const.o \
          db.o fight.o gr_magic.o handler.o houses.o interp.o magic.o mem.o \
          olc.o olc_act.o olc_help.o olc_save.o \
	  player.o quest.o raceskill.o recycle.o save.o scan.o \
          special.o ssm.o string.o tables.o track.o update.o hunt_info.o \
	  hunt_cost.o
          

eos: $(O_FILES)
#	@echo "."
	@echo "[1;35mBuilding the [1;31mNew [1;32mProject[0;37m"
	rm -f eos
	$(CC) $(L_FLAGS) -o eos $(O_FILES) $(CRYPT)
	@echo "Done."

clean:
	rm -f *.o
	rm -f eos
	make

.c.o: merc.h
#	@echo -n "."
	$(CC) -c $(C_FLAGS) $(SYSTEM) $<

1234:
#	@echo "."
	@echo "Updating eos on 1234."
#	rm -f /home/storm/EOS/src/envy
	cp -f envy /home/storm/EOS/src
	@echo "Done."

2222:
#	@echo "."
	@echo "Updating envy on 2222."
#	rm -f /home/storm/EOSBUILD/src/envy
	cp -f envy /home/storm/EOSBUILD/src
	@echo "Done."

port:
#	make
	make 1234
	make 2222
