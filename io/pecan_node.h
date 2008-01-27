/*
 * Copyright (C) 2006-2008 Felipe Contreras.
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef PECAN_NODE_H
#define PECAN_NODE_H

#include <glib-object.h>

typedef enum PecanNodeType PecanNodeType;
typedef struct PecanNode PecanNode;

#define PECAN_NODE_ERROR pecan_node_error_quark ()

enum
{
    PECAN_NODE_ERROR_OPEN,
    PECAN_NODE_ERROR_READ,
    PECAN_NODE_ERROR_WRITE
};

enum PecanNodeType
{
    PECAN_NODE_NS, /**< Notification Server */
    PECAN_NODE_PASSPORT, /**< Passport Server (for login) */
    PECAN_NODE_CS, /**< Contact Server (addressbook stuff) */
    PECAN_NODE_SB /**< Switcbhard Server (a conversation) */
};

#define PECAN_NODE_TYPE (pecan_node_get_type ())
#define PECAN_NODE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), PECAN_NODE_TYPE, PecanNode))
#define PECAN_NODE_CLASS(c) (G_TYPE_CHECK_CLASS_CAST ((c), PECAN_NODE_TYPE, PecanNodeClass))
#define PECAN_IS_NODE(obj) (G_TYPE_CHECK_TYPE ((obj), PECAN_NODE_TYPE))
#define PECAN_IS_NODE_CLASS(c) (G_TYPE_CHECK_CLASS_TYPE ((c), PECAN_NODE_TYPE))
#define PECAN_NODE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), PECAN_NODE_TYPE, PecanNodeClass))

PecanNode *pecan_node_new (gchar *name, PecanNodeType type);
void pecan_node_free (PecanNode *conn);
void pecan_node_connect (PecanNode *conn, const gchar *hostname, gint port);
void pecan_node_close (PecanNode *conn);

GIOStatus pecan_node_read (PecanNode *conn, gchar *buf, gsize count, gsize *bytes_read, GError **error);
GIOStatus pecan_node_write (PecanNode *conn, const gchar *buf, gsize count, gsize *bytes_written, GError **error);
void pecan_node_parse (PecanNode *conn, gchar *buf, gsize bytes_read);
void pecan_node_link (PecanNode *conn, PecanNode *next);
void pecan_node_error (PecanNode *conn);

#endif /* PECAN_NODE_H */
