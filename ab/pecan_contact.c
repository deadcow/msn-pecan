/**
 * Copyright (C) 2007-2008 Felipe Contreras
 *
 * Purple is the legal property of its developers, whose names are too numerous
 * to list here.  Please refer to the COPYRIGHT file distributed with this
 * source distribution.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */

#include "pecan_contact.h"
#include "pecan_contact_priv.h"
#include "pecan_contactlist_priv.h"
#include "pecan_log.h"

#include "cvr/slp.h"

#include <string.h>

#ifdef HAVE_LIBPURPLE
#include "fix_purple.h"

/* libpurple stuff. */
#include "fix_purple_win32.h"
#include <cipher.h>
#include <account.h>
#endif /* HAVE_LIBPURPLE */

PecanContact *
pecan_contact_new (PecanContactList *contactlist)
{
    PecanContact *contact;

    contact = g_new0 (PecanContact, 1);

    contact->contactlist = contactlist;
    contact->groups = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);

    return contact;
}

void
pecan_contact_free (PecanContact *contact)
{
    g_return_if_fail (contact);

    if (contact->clientcaps)
        g_hash_table_destroy (contact->clientcaps);

    g_hash_table_destroy (contact->groups);

#ifdef HAVE_LIBPURPLE
    if (contact->msnobj)
        msn_object_destroy (contact->msnobj);
#endif /* HAVE_LIBPURPLE */

    g_free (contact->passport);
    g_free (contact->friendly_name);
    g_free (contact->personal_message);
    g_free (contact->store_name);
    g_free (contact->guid);
    g_free (contact->phone.home);
    g_free (contact->phone.work);
    g_free (contact->phone.mobile);

    g_free (contact);
}

void
pecan_contact_update (PecanContact *contact)
{
#ifdef HAVE_LIBPURPLE
    PurpleAccount *account;

    account = msn_session_get_account (contact->contactlist->session);

    if (contact->status)
    {
        if (!strcmp (contact->status, "offline") && contact->mobile)
        {
            purple_prpl_got_user_status (account, contact->passport, "offline", NULL);
            purple_prpl_got_user_status (account, contact->passport, "mobile", NULL);
        }
        else
        {
            purple_prpl_got_user_status (account, contact->passport, contact->status, NULL);
            purple_prpl_got_user_status_deactive (account, contact->passport, "mobile");
        }
    }

    if (contact->idle)
        purple_prpl_got_user_idle (account, contact->passport, TRUE, -1);
    else
        purple_prpl_got_user_idle (account, contact->passport, FALSE, 0);
#endif /* HAVE_LIBPURPLE */
}

void
pecan_contact_set_state (PecanContact *contact,
                         const gchar *state)
{
    const gchar *status;

    if (!g_ascii_strcasecmp (state, "BSY"))
        status = "busy";
    else if (!g_ascii_strcasecmp (state, "BRB"))
        status = "brb";
    else if (!g_ascii_strcasecmp (state, "AWY"))
        status = "away";
    else if (!g_ascii_strcasecmp (state, "PHN"))
        status = "phone";
    else if (!g_ascii_strcasecmp (state, "LUN"))
        status = "lunch";
    else
        status = "available";

    if (!g_ascii_strcasecmp (state, "IDL"))
        contact->idle = TRUE;
    else
        contact->idle = FALSE;

    contact->status = status;
}

void
pecan_contact_set_passport (PecanContact *contact,
                            const gchar *passport)
{
    g_return_if_fail (contact);

    g_free (contact->passport);
    contact->passport = g_strdup (passport);
}

static gboolean
contact_is_account (PecanContact *contact)
{
    if (strcmp (msn_session_get_username (contact->contactlist->session), contact->passport) == 0)
        return TRUE;

    return FALSE;
}

void
pecan_contact_set_friendly_name (PecanContact *contact,
                                 const gchar *name)
{
    g_return_if_fail (contact);

    pecan_debug ("passport=[%s],name=[%s]", contact->passport, name);

    if (contact->friendly_name && name &&
        strcmp (contact->friendly_name, name) == 0)
    {
        return;
    }

    g_free (contact->friendly_name);
    contact->friendly_name = g_strdup (name);

#ifdef HAVE_LIBPURPLE
    {
        PurpleAccount *account;
        PurpleConnection *gc;
        MsnSession *session;

        session = contact->contactlist->session;
        account = msn_session_get_account (session);
        gc = purple_account_get_connection (account);

        purple_buddy_set_nickname (gc, contact->passport, contact->friendly_name);
    }

    /* If contact == account; display and friendly are the same thing. */
    /** @todo this is a libpurple specific thing */
    if (contact_is_account (contact))
    {
        pecan_debug ("contact is account");
        pecan_contact_set_store_name (contact, name);
    }
#endif /* HAVE_LIBPURPLE */
}

void
pecan_contact_set_personal_message (PecanContact *contact,
                                    const gchar *value)
{
    g_return_if_fail (contact);

    pecan_debug ("passport=[%s],value=[%s]", contact->passport, value);

    if (contact->personal_message && value &&
        strcmp (contact->personal_message, value) == 0)
    {
        return;
    }

    g_free (contact->personal_message);
    contact->personal_message = g_strdup (value);
}

void
pecan_contact_set_store_name (PecanContact *contact,
                              const gchar *name)
{
    const gchar *tmp_name;
    g_return_if_fail (contact);

    pecan_debug ("passport=[%s],name=[%s]", contact->passport, name);

    /** @todo this is a hack to disable display names. */
    if (name &&
        strcmp (contact->passport, name) == 0)
    {
        tmp_name = NULL;
    }
    else
    {
        tmp_name = name;
    }

    if (contact->store_name && tmp_name &&
        strcmp (contact->store_name, tmp_name) == 0)
    {
        return;
    }

    g_free (contact->store_name);
    contact->store_name = g_strdup (tmp_name);

#ifdef HAVE_LIBPURPLE
    {
        PurpleAccount *account;
        PurpleConnection *gc;
        MsnSession *session;

        session = contact->contactlist->session;
        account = msn_session_get_account (session);
        gc = purple_account_get_connection (account);

        purple_buddy_set_displayname (gc, contact->passport, contact->store_name);
    }

    /* If contact == account; display and friendly are the same thing. */
    /** @todo this is a libpurple specific thing */
    if (contact_is_account (contact))
    {
        pecan_debug ("contact is account");
        pecan_contact_set_friendly_name (contact, name);
    }
#endif /* HAVE_LIBPURPLE */
}

void
pecan_contact_set_guid (PecanContact *contact,
                        const gchar *guid)
{
    g_return_if_fail (contact);

    g_free (contact->guid);
    contact->guid = g_strdup (guid);
}

void
pecan_contact_set_buddy_icon (PecanContact *contact,
                              PecanBuffer *image)
{
    MsnObject *msnobj = pecan_contact_get_object (contact);

    g_return_if_fail (contact);

    if (!image)
    {
        pecan_contact_set_object (contact, NULL);
        return;
    }

#ifdef HAVE_LIBPURPLE
    {
        PurpleCipherContext *ctx;
        char *base64;
        unsigned char digest[20];

        if (!msnobj)
        {
            msnobj = msn_object_new ();
            msn_object_set_local (msnobj);
            msn_object_set_type (msnobj, MSN_OBJECT_USERTILE);
            msn_object_set_location (msnobj, "TFR2C2.tmp");
            msn_object_set_creator (msnobj, pecan_contact_get_passport (contact));

            pecan_contact_set_object (contact, msnobj);
        }

        msn_object_set_image (msnobj, image);

        /* Compute the SHA1D field. */
        memset (digest, 0, sizeof (digest));

        ctx = purple_cipher_context_new_by_name ("sha1", NULL);
        purple_cipher_context_append (ctx, (const guchar *) image->data, image->size);
        purple_cipher_context_digest (ctx, sizeof (digest), digest, NULL);

        base64 = purple_base64_encode (digest, sizeof (digest));
        msn_object_set_sha1d (msnobj, base64);
        g_free (base64);

        msn_object_set_size (msnobj, image->size);

        {
            gchar *buf;

            /* Compute the SHA1C field. */
            buf = pecan_strdup_printf ("Creator%sSize%dType%dLocation%sFriendly%sSHA1D%s",
                                       msn_object_get_creator (msnobj),
                                       msn_object_get_size (msnobj),
                                       msn_object_get_type (msnobj),
                                       msn_object_get_location (msnobj),
                                       msn_object_get_friendly (msnobj),
                                       msn_object_get_sha1d (msnobj));

            memset (digest, 0, sizeof (digest));

            purple_cipher_context_reset (ctx, NULL);
            purple_cipher_context_append (ctx, (const guchar *) buf, strlen (buf));
            purple_cipher_context_digest (ctx, sizeof (digest), digest, NULL);
            purple_cipher_context_destroy (ctx);

            g_free (buf);
        }

        base64 = purple_base64_encode (digest, sizeof (digest));
        msn_object_set_sha1c (msnobj, base64);
        g_free (base64);
    }
#endif /* HAVE_LIBPURPLE */
}

void
pecan_contact_add_group_id (PecanContact *contact,
                            const gchar *group_guid)
{
    const gchar *passport;

    g_return_if_fail (contact);

    passport = pecan_contact_get_passport (contact);

    pecan_debug ("passport=[%s],group_guid=[%s]", contact->passport, group_guid);

    if (group_guid)
    {
        g_hash_table_insert (contact->groups, g_strdup (group_guid), "foo");
    }

#ifdef HAVE_LIBPURPLE
    {
        PecanContactList *contactlist;
        PurpleAccount *account;
        PurpleBuddy *b = NULL;
        PurpleGroup *g = NULL;
        const gchar *group_name;

        contactlist = contact->contactlist;
        group_name = pecan_contactlist_find_group_name (contactlist, group_guid);
        account = msn_session_get_account (contactlist->session);

        /* If this contact is in the no-group, remove him, since now he is in a
         * group. */
        if (group_guid)
        {
            const gchar *t_group_name;
            PurpleGroup *t_g;

            t_group_name = pecan_contactlist_find_group_name (contactlist, NULL);
            t_g = purple_find_group (t_group_name);

            if (t_g)
            {
                b = purple_find_buddy_in_group (account, passport, t_g);

                if (b)
                {
                    purple_blist_remove_buddy (b);
                }
            }
        }

        if (group_name)
            g = purple_find_group (group_name);

        /* If the group is not there, add it */
        if (!g)
        {
            g = purple_group_new (group_name);
            purple_blist_add_group (g, NULL);
        }

        b = purple_find_buddy_in_group (account, passport, g);

        if (!b)
        {
            b = purple_buddy_new (account, passport, NULL);
            purple_blist_add_buddy (b, NULL, g, NULL);
        }

        b->proto_data = contact;
    }
#endif /* HAVE_LIBPURPLE */
}

void
pecan_contact_remove_group_id (PecanContact *contact,
                               const gchar *group_guid)
{
    g_return_if_fail (contact);
    g_return_if_fail (group_guid);

    pecan_debug ("passport=[%s],group_guid=[%s]", contact->passport, group_guid);

    g_hash_table_remove (contact->groups, group_guid);
}

guint
pecan_contact_get_group_count (PecanContact *contact)
{
    return g_hash_table_size (contact->groups);
}

void
pecan_contact_set_home_phone (PecanContact *contact,
                              const gchar *number)
{
    g_return_if_fail (contact);

    g_free (contact->phone.home);

    contact->phone.home = (!number ? NULL : g_strdup (number));
}

void
pecan_contact_set_work_phone (PecanContact *contact,
                              const gchar *number)
{
    g_return_if_fail (contact);

    g_free (contact->phone.work);

    contact->phone.work = (!number ? NULL : g_strdup (number));
}

void
pecan_contact_set_mobile_phone (PecanContact *contact,
                                const gchar *number)
{
    g_return_if_fail (contact);

    g_free (contact->phone.mobile);

    contact->phone.mobile = (!number ? NULL : g_strdup (number));
}

void
pecan_contact_set_object (PecanContact *contact,
                          MsnObject *obj)
{
#ifdef HAVE_LIBPURPLE
    g_return_if_fail (contact);

    if (contact->msnobj)
        msn_object_destroy (contact->msnobj);

    contact->msnobj = obj;

    if (contact->list_op & MSN_LIST_FL_OP)
        msn_queue_buddy_icon_request (contact);
#endif /* HAVE_LIBPURPLE */
}

void
pecan_contact_set_client_caps (PecanContact *contact,
                               GHashTable *info)
{
    g_return_if_fail (contact);
    g_return_if_fail (info);

    if (contact->clientcaps)
        g_hash_table_destroy (contact->clientcaps);

    contact->clientcaps = info;
}

const gchar *
pecan_contact_get_passport (const PecanContact *contact)
{
    g_return_val_if_fail (contact, NULL);

    return contact->passport;
}

const gchar *
pecan_contact_get_friendly_name (const PecanContact *contact)
{
    g_return_val_if_fail (contact, NULL);

    return contact->friendly_name;
}

const gchar *
pecan_contact_get_personal_message (const PecanContact *contact)
{
    g_return_val_if_fail (contact, NULL);

    return contact->personal_message;
}

const gchar *
pecan_contact_get_store_name (const PecanContact *contact)
{
    g_return_val_if_fail (contact, NULL);

    return contact->store_name;
}

const gchar *
pecan_contact_get_guid (const PecanContact *contact)
{
    g_return_val_if_fail (contact, NULL);

    return contact->guid;
}

const gchar *
pecan_contact_get_home_phone (const PecanContact *contact)
{
    g_return_val_if_fail (contact, NULL);

    return contact->phone.home;
}

const gchar *
pecan_contact_get_work_phone (const PecanContact *contact)
{
    g_return_val_if_fail (contact, NULL);

    return contact->phone.work;
}

const gchar *
pecan_contact_get_mobile_phone (const PecanContact *contact)
{
    g_return_val_if_fail (contact, NULL);

    return contact->phone.mobile;
}

MsnObject *
pecan_contact_get_object (const PecanContact *contact)
{
    g_return_val_if_fail (contact, NULL);

    return contact->msnobj;
}

GHashTable *
pecan_contact_get_client_caps (const PecanContact *contact)
{
    g_return_val_if_fail (contact, NULL);

    return contact->clientcaps;
}
