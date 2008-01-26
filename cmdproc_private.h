/**
 * Copyright (C) 2008 Felipe Contreras.
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

#ifndef MSN_CMDPROC_PRIVATE_H
#define MSN_CMDPROC_PRIVATE_H

#include "cmdproc.h"
#include "table.h"

struct MsnSession;
struct MsnHistory;
struct ConnObject;

struct MsnCmdProc
{
    struct MsnSession *session;

    GQueue *txqueue;

    MsnCommand *last_cmd;

    MsnTable *cbs_table;

    gpointer data; /**< Extra data, like the switchboard. */
    guint cmd_count;

    struct MsnHistory *history;
    struct ConnObject *conn;
};

#endif /* MSN_CMDPROC_PRIVATE_H */
