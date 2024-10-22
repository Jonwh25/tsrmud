#MOBPROGS
#2
say I am just a bartender!  Now hurry up and order something $r!
say I might just have to kick you out myself if you don't!
~
#3000
yell The Realms are in danger! Beware!
~
#3001
if level $n > 151
bow $n
say Greetings oh mighty one.
else
mob echoat $n The Healer greets you with a smile.
endif
~
#3002
if isdelay $i
break
else
mob echoat $n $I stumbles into you!
mob echoaround $n $I stumbles into $N.
mob remember $n
mob delay 5
endif
~
#3003
if hastarget $i
puke $q
burp
say pppbbbbt! Ssssory about that...
hiccup
else
mob forget
endif
~
#3005
mumble
emote sweeps the room.
say I hate this job!
scream
~
#3006
emote sniffs you experimentally.
emote snarls at you and begins to bark loudly! WOOF! WOOF! WOOF! WOOF!
~
#3010
emote mumbles something about lousy mortals and their quests.
~
#8600
bow %n
say Welcome $N. I am $I. 
grin
emote shuffles through a list of papers on his desk.
smile $n
say ah yes, Your here for a quest.
twiddle
~
#8601
yell hmm...You guys having fun?
~
#8602
yell Damn I'm smooth!
~
#8603
yell Hey! Stop killing those creatures! They will make your sword rust!
~
#8604
yell Fine! Don't talk to me! Just let me die a happy fairy dragon!
~
#8605
yell Dude! I think I lost my brain!
~
#8606
yell Bite me jerky!
~
#8607
ooc The New classes aren't completed yet!
ooc The Gods need to get their asses in gear!
~
#8608
yell Watch it there Punky chops, or I'll rap you across the base of the skull with a pipe wrench!
~
#8610
~
#20000
if level $n == 151
say Welcome to the Hall of Rebirth $n.
mob echo Please Choose a Race.
mob echo Lich, Chaon, Balor, Brownie, Centaur, Gnome, Halfling, Werewolf,
mob echo Mind Flayer, Satyr, Archon, or Pit Fiend.
say Please say out loud what race you would like to be.
else
say You do not yet belong here. Please leave.
endif
~
#20001
if level $n == 151
mob echo Please Choose a Class:
mob echo Abbot, Assassin, Defiler, Dominator, Mediator, Overlord, Paladin,
mob echo Samurai, Sorcerer, Underlord, Warlord, or Wizard.
say Please say out loud which you would like to be.
else
say Your not worthy of my time...get lost.
endif
~
#28000
if clan $n = 'strife'
nod
else
mob gtransfer $n 28001
emote rumbles "None shall pass!"
bash $n
endif

~
#0

#$
