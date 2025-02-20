/**
 * \file obj-desc.c
 * \brief Create object name descriptions
 *
 * Copyright (c) 1997 - 2007 Angband contributors
 *
 * This work is free software; you can redistribute it and/or modify it
 * under the terms of either:
 *
 * a) the GNU General Public License as published by the Free Software
 *    Foundation, version 2, or
 *
 * b) the "Angband licence":
 *    This software may be copied and distributed for educational, research,
 *    and not for profit purposes provided that this copyright and statement
 *    are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"
#include "obj-chest.h"
#include "obj-desc.h"
#include "obj-gear.h"
#include "obj-ignore.h"
#include "obj-knowledge.h"
#include "obj-tval.h"
#include "obj-util.h"

/**
 * Puts the object base kind's name into buf.
 */
void object_base_name(char *buf, size_t max, int tval, bool plural)
{
	struct object_base *kb = &kb_info[tval];
	size_t end = 0;

	if (kb->name && kb->name[0]) 
		(void) obj_desc_name_format(buf, max, end, kb->name, NULL, plural);
}


/**
 * Puts a very stripped-down version of an object's name into buf.
 * If easy_know is true, then the IDed names are used, otherwise
 * flavours, scroll names, etc will be used.
 *
 * Just truncates if the buffer isn't big enough.
 */
void object_kind_name(char *buf, size_t max, const struct object_kind *kind,
					  bool easy_know)
{
	/* If not aware, the plain flavour (e.g. Copper) will do. */
	if (!easy_know && !kind->aware && kind->flavor)
		my_strcpy(buf, kind->flavor->text, max);

	/* Use proper name (Healing, or whatever) */
	else
		(void) obj_desc_name_format(buf, max, 0, kind->name, NULL, false);
}


/**
 * A modifier string, put where '#' goes in the basename below.  The weird
 * games played with book names are to allow the non-essential part of the
 * name to be abbreviated when there is not much room to display.
 */
static const char *obj_desc_get_modstr(const struct object_kind *kind)
{
	if (tval_can_have_flavor_k(kind))
		return kind->flavor ? kind->flavor->text : "";

	if (tval_is_book_k(kind))
		return kind->name;

	return "";
}

/**
 * An object's basic name - a generic name for flavored objects (with the
 * actual name added later depending on awareness, the name from object.txt
 * for almost everything else, and a bit extra for books. 
 */
static const char *obj_desc_get_basename(const struct object *obj, bool aware,
										 bool terse, int mode)
{
	bool show_flavor = !terse && obj->kind->flavor;

	if (mode & ODESC_STORE)
		show_flavor = false;
	if (aware && !OPT(player, show_flavors)) show_flavor = false;

	/* Artifacts are special */
	if (obj->artifact && (aware || object_is_known_artifact(obj) || terse ||
						  !obj->kind->flavor))
		return obj->kind->name;

	/* Analyze the object */
	switch (obj->tval)
	{
		case TV_FLASK:
		case TV_CHEST:
		case TV_SHOT:
		case TV_BOLT:
		case TV_ARROW:
		case TV_BOW:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_THROWW:
		case TV_BONE:
		case TV_DIGGING:
		case TV_BOOTS:
		case TV_GLOVES:
		case TV_CLOAK:
		case TV_CROWN:
		case TV_HELM:
		case TV_SHIELD:
		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_DRAG_ARMOR:
		case TV_LIGHT:
		case TV_FOOD:
		case TV_TERRAIN:
			return obj->kind->name;

		case TV_AMULET:
			return (show_flavor ? "& # Amulet~" : "& Amulet~");

		case TV_RING:
			return (show_flavor ? "& # Ring~" : "& Ring~");

		case TV_STAFF:
			return (show_flavor ? "& # Sta|ff|ves|" : "& Sta|ff|ves|");

		case TV_WAND:
			return (show_flavor ? "& # Wand~" : "& Wand~");

		case TV_ROD:
			return (show_flavor ? "& # Rod~" : "& Rod~");

		case TV_POTION:
			return (show_flavor ? "& # Potion~" : "& Potion~");

		case TV_SCROLL:
			return (show_flavor ? "& Scroll~ titled #" : "& Scroll~");

		case TV_MAGIC_BOOK:
			if (terse)
				return "& Book~ #";
			else
				return "& Book~ of Magic Spells #";

		case TV_PRAYER_BOOK:
			if (terse)
				return "& Book~ #";
			else
				return "& Holy Book~ of Prayers #";

		case TV_NATURE_BOOK:
			if (terse)
				return "& Book~ #";
			else
				return "& Book~ of Nature Magics #";

		case TV_SHADOW_BOOK:
			if (terse)
				return "& Tome~ #";
			else
				return "& Necromantic Tome~ #";

		case TV_OTHER_BOOK:
			if (terse)
				return "& Book~ #";
			else
				return "& Book of Mysteries~ #";

		case TV_MUSHROOM:
			return (show_flavor ? "& # Mushroom~" : "& Mushroom~");
	}

	return "(nothing)";
}


/**
 * Start to description, indicating number/uniqueness (a, the, no more, 7, etc)
 */
static size_t obj_desc_name_prefix(char *buf, size_t max, size_t end,
		const struct object *obj, const char *basename,
		const char *modstr, bool terse)
{
	if (obj->number == 0) {
		strnfcat(buf, max, &end, "no more ");
	} else if (obj->number > 1) {
		strnfcat(buf, max, &end, "%d ", obj->number);
	} else if (object_is_known_artifact(obj)) {
		strnfcat(buf, max, &end, "the ");
	} else if (*basename == '&') {
		bool an = false;
		const char *lookahead = basename + 1;

		while (*lookahead == ' ') lookahead++;

		if (*lookahead == '#') {
			if (modstr && is_a_vowel(*modstr))
				an = true;
		} else if (is_a_vowel(*lookahead)) {
			an = true;
		}

		if (!terse) {
			if (an)
				strnfcat(buf, max, &end, "an ");
			else
				strnfcat(buf, max, &end, "a ");			
		}
	}

	return end;
}



/**
 * Formats 'fmt' into 'buf', with the following formatting characters:
 *
 * '~' at the end of a word (e.g. "fridge~") will pluralise
 *
 * '|x|y|' will be output as 'x' if singular or 'y' if plural
 *    (e.g. "kni|fe|ves|")
 *
 * '#' will be replaced with 'modstr' (which may contain the pluralising
 * formats given above).
 */
size_t obj_desc_name_format(char *buf, size_t max, size_t end,
		const char *fmt, const char *modstr, bool pluralise)
{
	/* Copy the string */
	while (*fmt) {
		/* Skip */
		if (*fmt == '&') {
			while (*fmt == ' ' || *fmt == '&')
				fmt++;
			continue;
		} else if (*fmt == '~') {
			/* Pluralizer (regular English plurals) */
			char prev = *(fmt - 1);

			if (!pluralise)	{
				fmt++;
				continue;
			}

			/* e.g. cutlass-e-s, torch-e-s, box-e-s */
			if (prev == 's' || prev == 'h' || prev == 'x')
				strnfcat(buf, max, &end, "es");
			else
				strnfcat(buf, max, &end, "s");
		} else if (*fmt == '|') {
			/* Special plurals 
			* e.g. kni|fe|ves|
			*          ^  ^  ^ */
			const char *singular = fmt + 1;
			const char *plural   = strchr(singular, '|');
			const char *endmark  = NULL;

			if (plural) {
				plural++;
				endmark = strchr(plural, '|');
			}

			if (!singular || !plural || !endmark) return end;

			if (!pluralise)
				strnfcat(buf, max, &end, "%.*s", plural - singular - 1,
						 singular);
			else
				strnfcat(buf, max, &end, "%.*s", endmark - plural, plural);

			fmt = endmark;
		} else if (*fmt == '#') {
			/* Add modstr, with pluralisation if relevant */
			end = obj_desc_name_format(buf, max, end, modstr, NULL,	pluralise);
		}

		else
			buf[end++] = *fmt;

		fmt++;
	}

	buf[end] = 0;

	return end;
}


/**
 * Format object obj's name into 'buf'.
 */
static size_t obj_desc_name(char *buf, size_t max, size_t end,
		const struct object *obj, bool prefix, int mode, bool terse)
{
	bool store = mode & ODESC_STORE ? true : false;
	bool spoil = mode & ODESC_SPOIL ? true : false;
	
	/* Actual name for flavoured objects if aware, or in store, or spoiled */
	bool aware = object_flavor_is_aware(obj) || store || spoil;
	/* Pluralize if (not forced singular) and
	 * (not a known/visible artifact) and
	 * (not one in stack or forced plural) */
	bool plural = !(mode & ODESC_SINGULAR) &&
		!obj->artifact &&
		(obj->number != 1 || (mode & ODESC_PLURAL));
	const char *basename = obj_desc_get_basename(obj, aware, terse, mode);
	const char *modstr = obj_desc_get_modstr(obj->kind);

	/* Quantity prefix */
	if (prefix)
		end = obj_desc_name_prefix(buf, max, end, obj, basename, modstr, terse);

	/* Base name */
	end = obj_desc_name_format(buf, max, end, basename, modstr, plural);

	/* Append extra names of various kinds */
	if (object_is_known_artifact(obj))
		strnfcat(buf, max, &end, " %s", obj->artifact->name);
	else if ((obj->known->ego && !(mode & ODESC_NOEGO)) || (obj->ego && store)) {
		if (obj->kind->tval == TV_STAFF) {
			/* Staffs can have both flavors and egos */
			if (aware && !obj->artifact && (terse)) strnfcat(buf, max, &end, " '%s'", obj->kind->name);
			else if (aware && !obj->artifact) strnfcat(buf, max, &end, " of %s", obj->kind->name);
		}
		strnfcat(buf, max, &end, " %s", obj->ego->name);
	}
	else if (aware && !obj->artifact &&
			 (obj->kind->flavor || obj->kind->tval == TV_SCROLL)) {
		if (terse)
			strnfcat(buf, max, &end, " '%s'", obj->kind->name);
		else
			strnfcat(buf, max, &end, " of %s", obj->kind->name);
	}

	return end;
}

/**
 * Is obj armor?
 */
static bool obj_desc_show_armor(const struct object *obj)
{
	if (player->obj_k->ac && (obj->ac || tval_is_armor(obj))) return true;

	return false;
}

/**
 * Special descriptions for types of chest traps
 */
static size_t obj_desc_chest(const struct object *obj, char *buf, size_t max,
							 size_t end)
{
	if (!tval_is_chest(obj)) return end;

	/* The chest is unopened, but we know nothing about its trap/lock */
	if (obj->pval && !obj->known->pval) return end;

	/* Describe the traps */
	strnfcat(buf, max, &end, format(" (%s)", chest_trap_name(obj)));

	return end;
}

/**
 * Describe combat properties of an item - damage dice, to-hit, to-dam, armor
 * class, missile multipler
 */
static size_t obj_desc_combat(const struct object *obj, char *buf, size_t max, 
							  size_t end, int mode)
{
	bool spoil = mode & ODESC_SPOIL ? true : false;

	/* Display damage dice if they are known */
	if (kf_has(obj->kind->kind_flags, KF_SHOW_DICE) &&
		(player->obj_k->dd && player->obj_k->ds))
		strnfcat(buf, max, &end, " (%dd%d)", obj->dd, obj->ds);

	/* Display shooting power as part of the multiplier */
	if (kf_has(obj->kind->kind_flags, KF_SHOW_MULT))
		strnfcat(buf, max, &end, " (x%d)",
				 obj->pval + obj->modifiers[OBJ_MOD_MIGHT]);

	/* No more if the object hasn't been assessed */
	if (!((obj->notice & OBJ_NOTICE_ASSESSED) || spoil)) return end;

	/* Show weapon bonuses if we know of any */
	if (player->obj_k->to_h && player->obj_k->to_d &&
		(tval_is_weapon(obj) || obj->to_d ||
		 (obj->to_h && !tval_is_body_armor(obj)) ||
		 (!object_has_standard_to_h(obj) && !obj->artifact && !obj->ego))) {
		/* In general show full combat bonuses */
		strnfcat(buf, max, &end, " (%+d,%+d)", obj->to_h, obj->to_d);
	} else if (obj->to_h < 0 && object_has_standard_to_h(obj)) {
		/* Special treatment for body armor with only a to-hit penalty */
		strnfcat(buf, max, &end, " (%+d)", obj->to_h);
	} else if (obj->to_d != 0 && player->obj_k->to_d) {
		/* To-dam rune known only */
		strnfcat(buf, max, &end, " (%+d)", obj->to_d);
	} else if (obj->to_h != 0 && player->obj_k->to_h) {
		/* To-hit rune known only */
		strnfcat(buf, max, &end, " (%+d)", obj->to_h);
	}

	/* Show armor bonuses */
	if (player->obj_k->to_a) {
		if (obj_desc_show_armor(obj))
			strnfcat(buf, max, &end, " [%d,%+d]", obj->ac, obj->to_a);
		else if (obj->to_a)
			strnfcat(buf, max, &end, " [%+d]", obj->to_a);
	} else if (obj_desc_show_armor(obj)) {
		strnfcat(buf, max, &end, " [%d]", obj->ac);
	}

	return end;
}

/**
 * Describe remaining light for refuellable lights
 */
static size_t obj_desc_light(const struct object *obj, char *buf, size_t max,
							 size_t end)
{
	/* Fuelled light sources get number of remaining turns appended */
	if (tval_is_light(obj) && !of_has(obj->flags, OF_NO_FUEL))
		strnfcat(buf, max, &end, " (%d turns)", obj->timeout);

	return end;
}

/**
 * Describe numerical modifiers to stats and other player qualities which
 * allow numerical bonuses - speed, stealth, etc
 */
static size_t obj_desc_mods(const struct object *obj, char *buf, size_t max,
							size_t end)
{
	int i, j, num_mods = 0;
	int mods[OBJ_MOD_MAX] = { 0 };

	/* Run through possible modifiers and store distinct ones */
	for (i = 0; i < OBJ_MOD_MAX; i++) {
		/* Check for known non-zero mods */
		if (obj->modifiers[i] != 0) {
			/* If no mods stored yet, store and move on */
			if (!num_mods) {
				mods[num_mods++] = obj->modifiers[i];
				continue;
			}

			/* Run through the existing mods, quit on duplicates */
			for (j = 0; j < num_mods; j++)
				if (mods[j] == obj->modifiers[i]) break;

			/* Add another mod if needed */
			if (j == num_mods)
				mods[num_mods++] = obj->modifiers[i];
		}
	}

	if (!num_mods) return end;

	/* Print the modifiers */
	strnfcat(buf, max, &end, " <");
	for (j = 0; j < num_mods; j++) {
		if (j) strnfcat(buf, max, &end, ", ");
		strnfcat(buf, max, &end, "%+d", mods[j]);
	}
	strnfcat(buf, max, &end, ">");

	return end;
}

/**
 * Describe charges or charging status for re-usable items with magic effects
 */
static size_t obj_desc_charges(const struct object *obj, char *buf, size_t max,
							   size_t end, int mode)
{
	bool aware = object_flavor_is_aware(obj) || (mode & ODESC_STORE);

	/* Wands and staffs have charges, others may be charging */
	if (aware && tval_can_have_charges(obj)) {
		strnfcat(buf, max, &end, " (%d charge%s)", obj->pval,
				 PLURAL(obj->pval));
	} else if (obj->timeout > 0) {
		if (tval_is_rod(obj) && obj->number > 1)
			strnfcat(buf, max, &end, " (%d charging)", number_charging(obj));
		else if (tval_is_rod(obj) || obj->activation || obj->effect)
			/* Artifacts, single rods */
			strnfcat(buf, max, &end, " (charging)");
	}

	return end;
}

/**
 * Add player-defined inscriptions or game-defined descriptions
 */
static size_t obj_desc_inscrip(const struct object *obj, char *buf,
							   size_t max, size_t end)
{
	const char *u[6] = { 0, 0, 0, 0, 0, 0 };
	int n = 0;

	/* Get inscription */
	if (obj->note)
		u[n++] = quark_str(obj->note);

	/* Use special inscription, if any */
	if (!object_flavor_is_aware(obj)) {
		if (tval_can_have_charges(obj) && (obj->pval == 0))
			u[n++] = "empty";
		if (object_flavor_was_tried(obj))
			u[n++] = "tried";
	}

	/* Note curses */
	if (obj->known->curses)
		u[n++] = "cursed";

	/* Note ignore */
	if (ignore_item_ok(obj))
		u[n++] = "ignore";

	/* Note unknown properties */
	if (!object_runes_known(obj) && (obj->known->notice & OBJ_NOTICE_ASSESSED))
		u[n++] = "??";

	if (n) {
		int i;
		for (i = 0; i < n; i++) {
			if (i == 0)
				strnfcat(buf, max, &end, " {");
			strnfcat(buf, max, &end, "%s", u[i]);
			if (i < n - 1)
				strnfcat(buf, max, &end, ", ");
		}

		strnfcat(buf, max, &end, "}");
	}

	return end;
}


/**
 * Add "unseen" to the end of unaware items in stores,
 * and "??" to not fully known unflavored items 
 */
static size_t obj_desc_aware(const struct object *obj, char *buf, size_t max,
							 size_t end)
{
	if (!object_flavor_is_aware(obj)) {
		strnfcat(buf, max, &end, " {unseen}");
	} else if (!object_runes_known(obj)) {
		strnfcat(buf, max, &end, " {??}");
	} else if (obj->known->curses) {
		strnfcat(buf, max, &end, " {cursed}");
	}

	return end;
}


/**
 * Describes item `obj` into buffer `buf` of size `max`.
 *
 * ODESC_PREFIX prepends a 'the', 'a' or number
 * ODESC_BASE results in a base description.
 * ODESC_COMBAT will add to-hit, to-dam and AC info.
 * ODESC_EXTRA will add pval/charge/inscription/ignore info.
 * ODESC_PLURAL will pluralise regardless of the number in the stack.
 * ODESC_STORE turns off ignore markers, for in-store display.
 * ODESC_SPOIL treats the object as fully identified.
 *
 * Setting 'prefix' to true prepends a 'the', 'a' or the number in the stack,
 * respectively.
 *
 * \returns The number of bytes used of the buffer.
 */
size_t object_desc(char *buf, size_t max, const struct object *obj, int mode)
{
	bool prefix = mode & ODESC_PREFIX ? true : false;
	bool spoil = mode & ODESC_SPOIL ? true : false;
	bool terse = mode & ODESC_TERSE ? true : false;

	size_t end = 0;

	/* Simple description for null item */
	if (!obj || !obj->known)
		return strnfmt(buf, max, "(nothing)");

	/* Unknown itema and cash get straightforward descriptions */
	if (obj->known && obj->kind != obj->known->kind) {
		if (prefix)
			return strnfmt(buf, max, "an unknown item");
		return strnfmt(buf, max, "unknown item");
	}

	if (tval_is_money(obj))
		return strnfmt(buf, max, "%d gold pieces worth of %s%s",
				obj->pval, obj->kind->name,
				ignore_item_ok(obj) ? " {ignore}" : "");

	/* Egos and kinds whose name we know are seen */
	if (obj->known->ego && !spoil)
		obj->ego->everseen = true;

	if (object_flavor_is_aware(obj) && !spoil)
		obj->kind->everseen = true;

	/** Construct the name **/

	/* Copy the base name to the buffer */
	end = obj_desc_name(buf, max, end, obj, prefix, mode, terse);

	/* Combat properties */
	if (mode & ODESC_COMBAT) {
		if (tval_is_chest(obj))
			end = obj_desc_chest(obj, buf, max, end);
		else if (tval_is_light(obj))
			end = obj_desc_light(obj, buf, max, end);

		end = obj_desc_combat(obj->known, buf, max, end, mode);
	}

	/* Modifiers, charges, flavour details, inscriptions */
	if (mode & ODESC_EXTRA) {
		end = obj_desc_mods(obj->known, buf, max, end);

		end = obj_desc_charges(obj, buf, max, end, mode);

		if (mode & ODESC_STORE)
			end = obj_desc_aware(obj, buf, max, end);
		else
			end = obj_desc_inscrip(obj, buf, max, end);
	}

	return end;
}

/* 
 * Get Statue Description 
 * (statdesc is kept in the pval of the statue terrain object)
 */
extern char get_statue_desc(int statdesc, int statuetype)
{
	/* default */
	char name = "statue";

	if (statuetype == 3) {
		/* small statue default */
		name = "small statue";

		/* Small Statue descriptions */
		if (statdesc == 1) name = "statue of Nain, a dwarf king who ruled in Khazad-dum in the third age";
		else if (statdesc == 2) name = "statue of a gnome with gemcutting tools in hand";
		else if (statdesc == 3) name = "statue of Azaghal, a dwarf lord of ancient times who dealt Glaurung the dragon a near-fatal wound as he was dying";
		else if (statdesc == 4) name = "statue of Dain II 'Ironfoot', a king of Durin's folk";
		else if (statdesc == 5) name = "statue of Durin III, a dwarf king in the second age who was given the chief of the Seven dwarven rings";
		else if (statdesc == 6) name = "statue of Thorin, King Under the Mountain, who died in the Battle of Five Armies";
		else if (statdesc == 7) name = "statue of Durin I, who began the building of Khazad-dum";
		else if (statdesc == 8) name = "statue of Thrain I, the first King Under the Mountain in Erebor after the dwarves fled from the balrog in Khazad-dum";
		else if (statdesc == 9) name = "statue of Isildur, who took the One Ring from Sauron's finger";
		else if (statdesc == 10) name = "statue of King Theoden of Rohan";
		else if (statdesc == 11) name = "statue of Tarondor, the king of Gondor who planted the seedling of the White Tree in the city which became Minas Tirith";
		else if (statdesc == 12) name = "statue of Earnur, who, with help from Glorfindel the elf lord, defeated Angmar and drove the Witch-King out of the north";
		else if (statdesc == 13) name = "statue of Bandobras Took, aka Bullroarer, the tallest hobbit in the history of the Shire";
		else if (statdesc == 14) name = "statue of Elendil who survived the fall of Numenor and became the first king of Arnor and Gondor";
		else if (statdesc == 15) name = "staute of Mughash the kobold lord";
		else if (statdesc == 16) name = "statue of a hunting dog";
		else if (statdesc == 17) name = "headless statue of a man holding a scroll";
		else if (statdesc == 18) name = "statue of a floating eye";
		else if (statdesc == 19) name = "staute of Mughash the kobold lord";
		else if (statdesc == 20) name = "statue of an eagle in flight";
		else if (statdesc == 21) name = "statue of a large cat";
		else if (statdesc == 22) name = "statue of a giant ant";
		else if (statdesc == 23) name = "statue of king cobra";
		else if (statdesc == 24) name = "statue of an archer with an arrow fitted to his bow";
		else if (statdesc == 25) name = "statue of a giant's severed head";
		/* Last few are the ones gargoyles diguise as */
		else if (statdesc == 26) name = "statue of a garden gnome";
		else if (statdesc == 27) name = "statue of some old guy you don't recognise";
		else if (statdesc == 28) name = "statue of a garden gnome";
		else if (statdesc == 29) name = "statue of a dancing goblin";
		else if (statdesc == 39) name = "statue of a dancing goblin";
		else if (statdesc == 31) name = "statue of something you're not sure what it's supposed to be, but it sends a shiver down your spine";
		else if (statdesc == 32) name = "statue of a hobgoblin with its tongue sticking out";
		else if (statdesc == 33) name = "statue of an orc captain";
	}
	if (statuetype == 4) {
		/* Statue descriptions */
		if (statdesc == 1) name = "huge statue of Al-Pharazon the Golden, the Last King of Numenor, oversized like his ego";
		else if (statdesc == 2) name = "statue of Eorl on the back of a mighty horse,  He was the king of Rohan who tamed the first of the mearas, the great horses";
		else if (statdesc == 3) name = "statue of Azaghal, a dwarf lord of ancient times who dealt Glaurung the dragon a near-fatal wound as he was dying";
		else if (statdesc == 4) name = "statue of Durin III, a dwarf king in the second age who was given the chief of the Seven dwarven rings";
		else if (statdesc == 5) name = "statue of Azog, the orc king who was killed by Dain II";
		else if (statdesc == 6) name = "statue of Durin I, who began the building of Khazad-dum";
		else if (statdesc == 7) name = "statue of Elendil who survived the fall of Numenor and became the first king of Arnor and Gondor";
		else if (statdesc == 8) name = "statue of Isildur, who took the One Ring from Sauron's finger";
		else if (statdesc == 9) name = "statue of King Theoden of Rohan";
		else if (statdesc == 10) name = "statue of Tarondor, the king of Gondor who planted the seedling of the White Tree in the city which became Minas Tirith";
		else if (statdesc == 11) name = "statue of Earnur, who, with help from Glorfindel the elf lord, defeated Angmar and drove the Witch-King out of the north";
		else if (statdesc == 12) name = "statue of Bandobras Took, aka Bullroarer, the tallest hobbit in the history of the Shire";
		else if (statdesc == 13) name = "statue of a beholder";
		else if (statdesc == 14) name = "statue of a pukelman (is that redundant?)";
		else if (statdesc == 15) name = "statue of an archer with an arrow fitted to his bow";
		else if (statdesc == 16) name = "statue of a winged horror";
		else if (statdesc == 17) name = "statue of a Rohirrim knight on horseback";
		else if (statdesc == 18) name = "statue of the Lord of the Nazgul on a black steed";
		else if (statdesc == 19) name = "statue of one of the great spiders, the decendants of Ungoliant";
		else if (statdesc == 20) name = "statue of a knight on horseback";
		else if (statdesc == 21) name = "statue of a fierce dragon";
		else if (statdesc == 22) name = "statue of a noble dragon";
		else if (statdesc == 23) name = "statue of the dullahan driving its black coach";
		else if (statdesc == 24) name = "statue of a headless horseman";
		else if (statdesc == 25) name = "statue of a giant serpent";
		/* Last few are the ones gargoyles diguise as */
		else if (statdesc == 26) name = "statue of an ogre chieftain";
		else if (statdesc == 27) name = "statue of an orc captain";
		else if (statdesc == 28) name = "statue of a gargoyle sitting and making an ugly face";
		else if (statdesc == 29) name = "statue of some old guy you don't recognise";
		else if (statdesc == 30) name = "statue of some old guy you don't recognise";
		else if (statdesc == 31) name = "statue which appears to be a stone troll turned to stone by a previous hero's magical light";
		else if (statdesc == 32) name = "statue of a gargoyle with wings spread";
		else if (statdesc == 33) name = "statue of a pooka in the form of giant goblin";
	}
	if (statuetype == 5) {
		/* fountain default */
		name = "fountain";

		/* Fountain descriptions */
		if (statdesc < 5) name = "statue of a giant serpent with water pouring from its mouth";
		else if (statdesc < 11) name = "fountain with spouts in the shape of four dragon heads";
		else if (statdesc < 15) name = "statue of a zhelung lizard leaping out of the water, spraying water and light";
		else if (statdesc < 18) name = "fountain with a statue of a mermaid";
		else if (statdesc < 20) name = "fountain covered in carvings of eyes and mouths";
		else if (statdesc < 25) name = "statue of a dragon breathing water instead of fire";
		else if (statdesc == 26) name = "statue of you screaming with a dozens arrows stuck in you and water pouring from the wounds. One of the arrows goes straight through your eye";
		else name = "fountain with the water pouring from a gargoyle's mouth";
	}
	return name;
}
