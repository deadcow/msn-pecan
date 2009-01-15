/**
 * Copyright (C) 2007-2008 Felipe Contreras
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef FIX_PURPLE_H
#define FIX_PURPLE_H

struct _PurpleConnection;

#include <glib.h>

/* libpurple stuff. */
#include <account.h>

#if !GLIB_CHECK_VERSION(2,3,1)

#if GLIB_SIZEOF_LONG == 8
#define G_GSIZE_FORMAT "lu"
#else
#define G_GSIZE_FORMAT "u"
#endif

#endif /* !GLIB_CHECK_VERSION(2,3,1) */

#ifndef G_GNUC_NULL_TERMINATED
#	if     __GNUC__ >= 4
#		define G_GNUC_NULL_TERMINATED __attribute__((__sentinel__))
#	else
#		define G_GNUC_NULL_TERMINATED
#	endif
#endif

#if !GLIB_CHECK_VERSION(2,16,0)
int g_strcmp0 (const char *str1, const char *str2);
#endif /* !GLIB_CHECK_VERSION(2,16,0) */

#if !GLIB_CHECK_VERSION(2,14,0)
static inline guint
g_timeout_add_seconds (guint interval,
                       GSourceFunc function,
                       gpointer data)
{
    g_return_val_if_fail (function != NULL, 0);

    return g_timeout_add_full (G_PRIORITY_DEFAULT, interval * 1000, function, data, NULL);
}
#endif /* !GLIB_CHECK_VERSION(2,14,0) */

void purple_buddy_set_displayname (struct _PurpleConnection *gc, const gchar *who, const gchar *value);
void purple_buddy_set_nickname (struct _PurpleConnection *gc, const gchar *who, const gchar *value);

/**
 * Returns a protocol-specific boolean setting for an account.
 *
 * @param account       The account.
 * @param name          The name of the setting.
 *
 * @return The value.
 */
gboolean purple_account_get_bool_without_default_value(const struct _PurpleAccount *account,
                                                       const char *name);

void *
purple_request_action(void *handle, const char *title, const char *primary,
                             const char *secondary, int default_action,
                             PurpleAccount *account, const char *who, PurpleConversation *conv,
                             void *user_data, size_t action_count, ...);

void
purple_sound_play_file(const char *filename, const PurpleAccount *account);

#endif /* FIX_PURPLE_H */
