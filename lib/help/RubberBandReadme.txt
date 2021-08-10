Some details of RubberBand features and differences from Vanilla Angband:
====================================================================================

  Note: PC stands for player character (that little @-sign that you move around and 
kill letters with).

Dungeon/terrain changes:
- There are several different types of terrain including water, trees, statues, fountains, chasms, etc.
The implementation of some of these aren't completely finished in this alpha version. At the moment, chasms
are only created in certain premade "interesting rooms" and vaults. (In the future, water will strengthen 
lightning attacks and weaken fire attacks, and there will be more interesting statue descriptions, etc.)
- Some terrain including rubble and water slow down characters moving through it (both monsters and the PC,
but water doesn't slow down monsters that swim and neither terrain will slow down flying monsters or very
large monsters).

Monster changes:
- The monster list has been significantly changed. Yeeks and icky things are (almost) completely removed.
Monsters with the 'y' symbol are light fairies (sprites, gnomes, etc) which are fast, magical, and hurt 
by darkness. Monsters with the 'i' symbol are dark fairies (goblins, trooping fairies, etc) which also tend
to be fast, magicial, and troublesome.
- Some monster races are sometimes evil and sometimes not evil. So detect evil will detect some cutpurses 
and kobolds but not all.
- Certain monsters may drop teeth, tusks which can be used as weapons (I mention bone weapons below). Also,
large elemental type monsters may create lava, rubble, or water when they die.
- Certain monsters may disguise as terrain features, namely gargoyles like to disguise as statues, and 
dryads ('y') like to disguise as trees. (But I'm not sure if this is fully working yet in the alpha version.)
- Some monsters can grab you, keeping you from moving and giving a severe reduction in spell fail rate %
among other effects. But simply reading ?phase or ?teleport gets you out of the monster's grasp.
- Some monsters can slime you, a lasting effect that can accumulate to deadly levels (but rarely does in
my games even without being careful about it at all. I'll likely make it a little stronger in a 
future version.)
- A few other timed effects monsters can give you include being charmed (makes you not want to hurt monsters,
which in effect is similar to fear), making you run around in a careless frenzy (penalties to skills and
spell fail rate along with a minor increase to damage), disease (temporarily lowers strength and 
constitution), insanity (temporarily lowers intelligence and wisdom), and a lasting curse (lowers saving
throw along with some other skills and lasts longer than most timed effects).
- There are also a few new monster spells, including temporary invisibility, and causing some of those
new timed effects.
- Some dungeon monsters may appear in the town and a couple town monsters may appear in shallow dungeon
levels.

Object changes:
- Many weapons have adjusted weight (mostly reduced from V). I might adjust the blows table to match
this reduction some, but I haven't yet.
- Some classes can now wield smaller weapons in the off-hand (shield slot). A weapon will tell you in its
description whether you can wield it in your off-hand. Attacking with your off-hand weapon works in a very
similar way as shield bashing does, but off-hand attacks happen slightly more often, are less likely to
make you stumble, and don't cause stunning like a shield bash does. Wielding a very heavy weapon as your 
main weapon will keep you from being able to wield a weapon in your off-hand.
- Magic staffs are wieldable as melee weapons and can get weapon egos, though magic staffs are less
likely than normal weapons to get combat bonuses or egos.
- There are two new weapon types: Weapons meant for throwing are now a separate type and aren't quite as
effective in melee. (A couple weapons like the dagger and spear are still just as effective whether used in 
melee or thrown.) Bone weapons (like dragon teeth and mumak tusks) are mostly flavor junk, but may come in 
handy on occation. They rarely appear except as monster drops.
- Grenades are thrown weapons which effectually provide a single-use ball spell to damage monsters. 
- Slings use throwing weapon skill rather than bow skill.
- The other added objects include several new types of mushrooms which often have odd and varied effects. 
- Alternate effects of certain wands: Wands of heal monster do damage to undead and have no effect on monsters
which are neither living nor undead. Wands/rods of disable traps do damage to trap monsters (including mimics, 
arrow launchers, and creeping coins among others). Potions of Surprise sometimes have good effects.

Other changes:
- There are two new player races: Hobglibs are a race of stocky magical humanoids with skin like a potato. 
Their personality is generally indifferent and apathetic about most things except their eccentric intrests, 
which usually includes unusual and experimental uses of magic. They are natural alchemists and get racial 
poison resistance and potion knowledge. There are also Goblins which get inherent +2 to speed.
- Kobolds no longer get poison resistance (They're short dog-people, why should they get it?) Instead, they 
get the CHARM druid player flag which makes status effects more effective on animals. I also plan to give 
them a smell ability which could let them detect some monsters which are out of sight (or invisible), but
that smell ability isn't implemented yet.
- You have a luck rating which starts at 1 and ranges from -5 to 5. Your luck affects many things in
(mostly) minor ways. At the moment, there's no way to see your luck rating in the game, but if you get a
message that says "You feel lucky to be alive" that means your luck just saved your life (this only happens
with positive luck rating and it makes your luck rating go down, likely putting your luck rating at 0.) Not
a lot of things can change your luck rating except for rare potions and mushrooms of Luck, and watch out
for black cats.

Some Future plans:
- Making the new terrains have more effect on the game.
- some new classes including alchemist, tourist, barbarian (warrior-type with a few nature-realm spells and
bonuses with heavy weapons), rescuer (stealthy prayer class), assassin (stealthy shadow realm half-caster), 
thief (alchemy realm version of the rogue, more magic and a little weaker at fighting than the rogue), and 
chaos warrior (novelty class with powerful but often unpredicable spells).
- Adding new spells and a even a couple new magic realms (Alchemy for the alchemist class and the thief, 
and Chance magic for the Tourist class and chaos warrior).
- themed levels
- ability to pick up rubble and statues and move them (They'll be extremely heavy obviously, but I'm sure
we'll find some fun uses for it...)
- more fun magic items, effects, and egos including teleport control, taming, telekinesis, "Lucky" ego, and 
bug-zapping.
- weaken movement speed effect. slightly weaken digging without a digging tool (or other digging magic).
- alignment with bonuses for sticking to your alignment and penalties for violating it. Necromancers and 
Blackguards are always evil. Paladins and Rescuers are always good. Other characters choose their alignment. 
(I may make all alignment effects optional because I'm guessing it might be unpopular...)
- I'm also considering possibly making a sister-variant with mostly similar code and mechanics as RubberBand,
but changing the flavor and monster list to fit the fantasy world in my stories (I write fantasy novels, 
but I don't have anything published yet). There are already a few monsters from the land of Fenegron in 
RubberBand including nulx (swamp monsters) and the Hobglib race.
