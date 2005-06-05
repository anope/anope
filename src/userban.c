/* Host and IP (incl. CIDR) banning routines
 *
 * (C) 2003-2005 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * $Id$
 *
 */

#include "services.h"

/*************************************************************************/

#define BANHOSTHASH(x) (*(x) & 0x1F)
#define BANINFOHASH(x) ((((x)->type & BANTYPE_CIDR4) || (*((x)->host) == '*') || (*((x)->host) == '?')) ? 32 : BANHOSTHASH((x)->host))

/*************************************************************************/

/**
 * Turn a cidr value into a netmask
 * @param cidr CIDR value
 * @return Netmask value
 */
uint32 cidr_to_netmask(uint16 cidr)
{
    if (cidr == 0)
        return 0;

    return (0xFFFFFFFF - (1 << (32 - cidr)) + 1);
}

/**
 * Turn a netmask into a cidr value
 * @param mask Netmask
 * @return CIDR value
 */
uint16 netmask_to_cidr(uint32 mask)
{
    int tmp = 0;

    while (!(mask & (1 << tmp)) && (tmp < 32))
        tmp++;

    return (32 - tmp);
}

/*************************************************************************/

/**
 * Check if the given string is some sort of wildcard
 * @param str String to check
 * @return 1 for wildcard, 0 for anything else
 */
int str_is_wildcard(const char *str)
{
    while (*str) {
        if ((*str == '*') || (*str == '?'))
            return 1;
        str++;
    }

    return 0;
}

/**
 * Check if the given string is a pure wildcard
 * @param str String to check
 * @return 1 for pure wildcard, 0 for anything else
 */
int str_is_pure_wildcard(const char *str)
{
    while (*str) {
        if (*str != '*')
            return 0;
        str++;
    }

    return 1;
}

/*************************************************************************/

/**
 * Check if the given string is an IP or CIDR mask, and fill the given
 * ip/cidr params if so.
 * @param str String to check
 * @param ip The ipmask to fill when a CIDR is found
 * @param mask The CIDR mask to fill when a CIDR is found
 * @param host Displayed host
 * @return 1 for IP/CIDR, 0 for anything else
 */
int str_is_cidr(char *str, uint32 * ip, uint32 * mask, char **host)
{
    int i;
    int octets[4] = { -1, -1, -1, -1 };
    char *s = str;
    char buf[512];
    uint16 cidr;

    for (i = 0; i < 4; i++) {
        octets[i] = strtol(s, &s, 10);
        /* Bail out if the octet is invalid or wrongly terminated */
        if ((octets[i] < 0) || (octets[i] > 255)
            || ((i < 3) && (*s != '.')))
            return 0;
        if (i < 3)
            s++;
    }

    /* Fill the IP - the dirty way */
    *ip = octets[3];
    *ip += octets[2] * 256;
    *ip += octets[1] * 65536;
    *ip += octets[0] * 16777216;

    if (*s == '/') {
        s++;
        /* There's a CIDR mask here! */
        cidr = strtol(s, &s, 10);
        /* Bail out if the CIDR is invalid or the string isn't done yet */
        if ((cidr > 32) || (*s))
            return 0;
    } else {
        /* No CIDR mask here - use 32 so the whole ip will be matched */
        cidr = 32;
    }

    *mask = cidr_to_netmask(cidr);
    /* Apply the mask to avoid 255.255.255.255/8 bans */
    *ip &= *mask;

    /* Refill the octets to fill the host */
    octets[0] = (*ip & 0xFF000000) / 16777216;
    octets[1] = (*ip & 0x00FF0000) / 65536;
    octets[2] = (*ip & 0x0000FF00) / 256;
    octets[3] = (*ip & 0x000000FF);

    if (cidr == 32)
        snprintf(buf, 512, "%d.%d.%d.%d", octets[0], octets[1], octets[2],
                 octets[3]);
    else
        snprintf(buf, 512, "%d.%d.%d.%d/%d", octets[0], octets[1],
                 octets[2], octets[3], cidr);

    *host = sstrdup(buf);

    return 1;
}

/*************************************************************************/

/**
 * Create a BanInfo for the given mask. This function destroys the given
 * mask as a side effect.
 * @param mask Host/IP/CIDR mask to parse
 * @param id Ban ID to use for this ban
 * @return BanInfo struct for the given mask, NULL if creation failed
 */
BanInfo *ban_create(char *mask, uint32 id)
{
    char *user, *host, *cidrhost;
    char buf[BUFSIZE];
    BanInfo *ban;
    uint32 ip;
    uint32 cidr;

    ban = scalloc(1, sizeof(BanInfo));
    ban->id = id;
    ban->type = BANTYPE_NONE;

    host = strchr(mask, '@');
    if (host) {
        *host++ = '\0';
        /* If the user is purely a wildcard, ignore it */
        if (str_is_pure_wildcard(mask))
            user = NULL;
        else
            user = mask;
    } else {
        /* If there's no user in the mask, assume a pure wildcard */
        user = NULL;
        host = mask;
    }

    if (user) {
        ban->user = sstrdup(user);
        /* Check if we have a wildcard user */
        if (str_is_wildcard(user))
            ban->type |= BANTYPE_USER_WILD;
        else
            ban->type |= BANTYPE_USER;
    }
    /* Only check the host if it's not a pure wildcard */
    if (*host && !str_is_pure_wildcard(host)) {
        if (ircd->nickip && str_is_cidr(host, &ip, &cidr, &cidrhost)) {
            ban->cidr_ip = ip;
            ban->cidr_mask = cidr;
            ban->type |= BANTYPE_CIDR4;
            host = cidrhost;
        } else if (strchr(host, '/')) {
            /* If we still have a CIDR we're fucked (scientific term) */
            return NULL;
        } else {
            ban->host = sstrdup(host);
            if (str_is_wildcard(host))
                ban->type |= BANTYPE_HOST_WILD;
            else
                ban->type |= BANTYPE_HOST;
        }
    }

    if (!user)
        user = "*";
    if (!host)
        host = "*";
    snprintf(buf, BUFSIZE, "%s@%s", user, host);
    ban->mask = sstrdup(buf);

    return ban;
}

/*************************************************************************/

/**
 * Match the given BanInfo to the given user/host and optional IP addy
 * @param ban BanInfo struct to match against
 * @param user User to match against
 * @param host Host to match against
 * @param ip IP to match against, set to 0 to not match this
 * @return 1 for a match, 0 for no match
 */
int ban_match(BanInfo * ban, char *user, char *host, uint32 ip)
{
    if ((ban->type & BANTYPE_CIDR4) && ip
        && ((ip & ban->cidr_mask) != ban->cidr_ip))
        return 0;
    if ((ban->type & BANTYPE_USER) && (stricmp(ban->user, user) != 0))
        return 0;
    if ((ban->type & BANTYPE_HOST) && (stricmp(ban->host, host) != 0))
        return 0;
    if ((ban->type & BANTYPE_USER_WILD)
        && !match_wild_nocase(ban->user, user))
        return 0;
    if ((ban->type & BANTYPE_HOST_WILD)
        && !match_wild_nocase(ban->host, host))
        return 0;

    return 1;
}

/**
 * Match the given BanInfo to the given hostmask and optional IP addy
 * @param ban BanInfo struct to match against
 * @param mask Hostmask to match against
 * @param ip IP to match against, set to 0 to not match this
 * @return 1 for a match, 0 for no match
 */
int ban_match_mask(BanInfo * ban, char *mask, uint32 ip)
{
    char *user;
    char *host;

    if (ban->type & (BANTYPE_USER | BANTYPE_USER_WILD)) {
        host = strchr(mask, '@');
        if (host) {
            *host++ = '\0';
            user = mask;
        } else {
            user = NULL;
        }
    }
    if (ban->type & (BANTYPE_HOST | BANTYPE_HOST_WILD)) {
        if (ban->type & (BANTYPE_USER | BANTYPE_USER_WILD)) {
            if (!user)
                host = mask;
        } else {
            host = strchr(mask, '@');
            if (host)
                host++;
            else
                host = mask;
        }
    }

    return ban_match(ban, user, host, ip);
}

/*************************************************************************/

/**
 * Create and initialize a new ban list
 * @param hashed Should the list be hashed or not?
 * @return Pointer to the created BanList object
 */
BanList *banlist_create(int hashed)
{
    BanList *bl;

    bl = scalloc(1, sizeof(BanList));
    bl->flags = BANLIST_NONE;

    if (hashed) {
        bl->flags |= BANLIST_HASHED;
        bl->bans = scalloc(33, sizeof(BanInfo));
    } else {
        bl->bans = scalloc(1, sizeof(BanInfo));
    }

    bl->next_id = 1;

    return bl;
}

/*************************************************************************/

/**
 * Create a ban and add it to the given banlist
 * @param bl BanList object the banmask should be added to
 * @param mask The mask to parse and add to the banlist
 * @return 1 for success, 0 for failure
 */
int ban_add(BanList * bl, char *mask)
{
    int hash;
    char *hostmask;
    BanInfo *ban;

    hostmask = sstrdup(mask);
    ban = ban_create(hostmask, bl->next_id);
    free(hostmask);
    if (!ban)
        return 0;

    bl->next_id++;

    if (bl->flags & BANLIST_HASHED) {
        hash = BANINFOHASH(ban);
        ban->next = bl->bans[hash];
        ban->prev = NULL;
        if (bl->bans[hash])
            bl->bans[hash]->prev = ban;
        bl->bans[hash] = ban;
    } else {
        ban->next = *(bl->bans);
        ban->prev = NULL;
        if (*(bl->bans))
            (*(bl->bans))->prev = ban;
        *(bl->bans) = ban;
    }

    return 1;
}

/*************************************************************************/

/**
 * Delete the given ban from the given banlist
 * @param bl BanList object the banmask should be deleted from
 * @param ban The ban to be deleted, has to be in the given banlist
 */
void ban_del(BanList * bl, BanInfo * ban)
{
    if (ban->next)
        ban->next->prev = ban->prev;
    if (ban->prev)
        ban->prev->next = ban->next;
    else if (bl->flags & BANLIST_HASHED)
        bl->bans[BANINFOHASH(ban)] = ban->next;
    else
        *(bl->bans) = ban->next;

    if (ban->user)
        free(ban->user);
    if (ban->host)
        free(ban->host);
    free(ban->mask);

    free(ban);
}

/*************************************************************************/

/**
 * Match a user, host, and ip to a banlist
 * @param bl BanList that should be matched against
 * @param user The user to match
 * @param host The host to match
 * @param ip The ip to match
 * @return 1 for match, 0 for no match
 */
int banlist_match(BanList * bl, char *user, char *host, uint32 ip)
{
    BanInfo *ban;

    if (bl->flags & BANLIST_HASHED) {
        for (ban = bl->bans[BANHOSTHASH(host)]; ban; ban = ban->next) {
            if (ban_match(ban, user, host, ip))
                return 1;
        }
        /* Now check for all wildcard-qualified bans */
        for (ban = bl->bans[32]; ban; ban = ban->next) {
            if (ban_match(ban, user, host, ip))
                return 1;
        }
    } else {
        for (ban = *(bl->bans); ban; ban = ban->next) {
            if (ban_match(ban, user, host, ip))
                return 1;
        }
    }

    /* We matched none, yay */
    return 0;
}

/**
 * Match a mask and ip to a banlist
 * @param bl BanList that should be matched against
 * @param mask The user@host mask to match
 * @return 1 for match, 0 for no match
 */
int banlist_match_mask(BanList * bl, char *mask, uint32 ip)
{
    char *user;
    char *host;

    host = strchr(mask, '@');
    if (host) {
        *host++ = '\0';
        user = mask;
    } else {
        user = NULL;
        host = mask;
    }

    return banlist_match(bl, user, host, ip);
}

/*************************************************************************/

/**
 * Lookup a ban id in a banlist
 * @param bl BanList to search in
 * @param id Ban id to lookup
 * @return Pointer to BanInfo for the given id, or NULL if not found
 */
BanInfo *banlist_lookup_id(BanList * bl, uint32 id)
{
    int i;
    BanInfo *ban;

    if (bl->flags & BANLIST_HASHED) {
        for (i = 0; i < 33; i++) {
            for (ban = bl->bans[i]; ban; ban = ban->next) {
                if (ban->id == id)
                    return ban;
            }
        }
    } else {
        for (ban = *(bl->bans); ban; ban = ban->next) {
            if (ban->id == id)
                return ban;
        }
    }
}

/* EOF */
