/* list-identify-flags.h - object property knowledge flags
 *
 * Reordering these flags will break savefiles, adding new ones at the end
 * will not. Flags below start from 0 on line 11, so a flag's sequence number
 * is its line number minus .
 *
 * These flags are for object properties which do not have their own internal
 * form of identification (like, for example, slays) or a convenient way of
 * representing knowledge by flags (like object flags)
 */
ID(NONE)
ID(ARTIFACT)
ID(EGO_ITEM)
ID(WEIGHT)
ID(PVAL)
ID(DD)
ID(DS)
ID(AC)
ID(TO_A)
ID(TO_H)
ID(TO_D)
