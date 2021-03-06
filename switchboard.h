/**
 * Copyright (C) 2008 Felipe Contreras
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

#ifndef MSN_SWITCHBOARD_H
#define MSN_SWITCHBOARD_H

#include <glib.h>

typedef struct MsnSwitchBoard MsnSwitchBoard;

/**
 * A switchboard error.
 */
typedef enum
{
    MSN_SB_ERROR_NONE, /**< No error. */
    MSN_SB_ERROR_CAL, /**< The user could not join (answer the call). */
    MSN_SB_ERROR_OFFLINE, /**< The account is offline. */
    MSN_SB_ERROR_USER_OFFLINE, /**< The user to call is offline. */
    MSN_SB_ERROR_CONNECTION, /**< There was a connection error. */
    MSN_SB_ERROR_TOO_FAST, /**< We are sending too fast */
    MSN_SB_ERROR_AUTHFAILED, /**< Authentication failed joining the switchboard session */
    MSN_SB_ERROR_UNKNOWN /**< An unknown error occurred. */

} MsnSBErrorType;

/**
 * A switchboard flag.
 */
typedef enum
{
    MSN_SB_FLAG_IM = 0x01, /**< This switchboard is being used for a conversation. */
    MSN_SB_FLAG_FT = 0x02, /**< This switchboard is being used for file transfer. */

} MsnSBFlag;

#include "io/pecan_cmd_server.h"
#include "io/pecan_node.h"

struct MsnSession;
struct MsnMessage;
struct MsnCmdProc;

struct _PurpleConversation;

/**
 * A switchboard.
 *
 * A place where a bunch of users send messages to the rest of the users.
 */
struct MsnSwitchBoard
{
    struct MsnSession *session;
    struct MsnCmdProc *cmdproc;
    char *im_user;

    MsnSBFlag flag;
    char *auth_key;
    char *session_id;

    struct _PurpleConversation *conv; /**< The conversation that displays the
                                messages of this switchboard, or @c NULL if
                                this is a helper switchboard. */

    gboolean empty;			/**< A flag that states if the swithcboard has no
                                          users in it. */
    gboolean invited;		/**< A flag that states if we were invited to the
                                  switchboard. */
    gboolean ready;			/**< A flag that states if this switchboard is
                                          ready to be used. */
    gboolean closed;		/**< A flag that states if the switchboard has
                                  been closed by the user. */
    gboolean destroying;	/**< A flag that states if the switchboard is
                                  alredy on the process of destruction. */

    int current_users;
    int total_users;
    GList *users;

    int chat_id;

    GQueue *msg_queue; /**< Queue of messages to send. */
    GQueue *invites; /**< Queue of participants to invite. */
    GList *ack_list; /**< List of messages waiting for an ack. */

    MsnSBErrorType error; /**< The error that occurred in this switchboard
                            (if applicable). */
    GList *slplinks; /**< The list of slplinks that are using this switchboard. */

    PecanCmdServer *conn;
    gulong open_handler;
    gulong close_handler;
    gulong error_handler;
};

/**
 * Initialize the variables for switchboard creation.
 */
void msn_switchboard_init(void);

/**
 * Destroy the variables for switchboard creation.
 */
void msn_switchboard_end(void);

/**
 * Creates a new switchboard.
 *
 * @param session The MSN session.
 *
 * @return The new switchboard.
 */
MsnSwitchBoard *msn_switchboard_new(struct MsnSession *session);

/**
 * Destroys a switchboard.
 *
 * @param swboard The switchboard to destroy.
 */
void msn_switchboard_destroy(MsnSwitchBoard *swboard);

/**
 * Sets the auth key the switchboard must use when connecting.
 *
 * @param swboard The switchboard.
 * @param key     The auth key.
 */
void msn_switchboard_set_auth_key(MsnSwitchBoard *swboard, const char *key);

/**
 * Returns the auth key the switchboard must use when connecting.
 *
 * @param swboard The switchboard.
 *
 * @return The auth key.
 */
const char *msn_switchboard_get_auth_key(MsnSwitchBoard *swboard);

/**
 * Sets the session ID the switchboard must use when connecting.
 *
 * @param swboard The switchboard.
 * @param id      The session ID.
 */
void msn_switchboard_set_session_id(MsnSwitchBoard *swboard, const char *id);

/**
 * Returns the session ID the switchboard must use when connecting.
 *
 * @param swboard The switchboard.
 *
 * @return The session ID.
 */
const char *msn_switchboard_get_session_id(MsnSwitchBoard *swboard);

/**
 * Sets whether or not we were invited to this switchboard.
 *
 * @param swboard The switchboard.
 * @param invite  @c TRUE if invited, @c FALSE otherwise.
 */
void msn_switchboard_set_invited(MsnSwitchBoard *swboard, gboolean invited);

/**
 * Returns whether or not we were invited to this switchboard.
 *
 * @param swboard The switchboard.
 *
 * @return @c TRUE if invited, @c FALSE otherwise.
 */
gboolean msn_switchboard_is_invited(MsnSwitchBoard *swboard);

/**
 * Connects to a switchboard.
 *
 * @param swboard The switchboard.
 * @param host    The switchboard server host.
 * @param port    The switcbharod server port.
 *
 * @return @c TRUE if able to connect, or @c FALSE otherwise.
 */
gboolean msn_switchboard_connect(MsnSwitchBoard *swboard,
                                 const char *host, int port);

/**
 * Disconnects from a switchboard.
 *
 * @param swboard The switchboard to disconnect from.
 */
void msn_switchboard_disconnect(MsnSwitchBoard *swboard);

/**
 * Closes the switchboard.
 *
 * Called when a conversation is closed.
 *
 * @param swboard The switchboard to close.
 */
void msn_switchboard_close(MsnSwitchBoard *swboard);

/**
 * Release a switchboard from a certain function.
 *
 * @param swboard The switchboard to release.
 * @param flag The flag that states the function.
 *
 * @return @c TRUE if the switchboard was closed, @c FALSE otherwise.
 */
gboolean msn_switchboard_release(MsnSwitchBoard *swboard, MsnSBFlag flag);

/**
 * Returns whether or not we currently can send a message through this
 * switchboard.
 *
 * @param swboard The switchboard.
 *
 * @return @c TRUE if a message can be sent, @c FALSE otherwise.
 */
gboolean msn_switchboard_can_send(MsnSwitchBoard *swboard);

/**
 * Sends a message through this switchboard.
 *
 * @param swboard The switchboard.
 * @param msg The message.
 * @param queue A flag that states if we want this message to be queued (in
 * the case it cannot currently be sent).
 *
 * @return @c TRUE if a message can be sent, @c FALSE otherwise.
 */
void msn_switchboard_send_msg(MsnSwitchBoard *swboard, struct MsnMessage *msg,
                              gboolean queue);

gboolean msn_switchboard_chat_leave(MsnSwitchBoard *swboard);
gboolean msn_switchboard_chat_invite(MsnSwitchBoard *swboard, const char *who);

void msn_switchboard_request(MsnSwitchBoard *swboard);
void msn_switchboard_request_add_user(MsnSwitchBoard *swboard, const char *user);

/** Displays handwritten messages (inserts them into conversation as custom smiley)
 *
 * @param swboard The command processor.
 * @param msgid     Unique ID for the message.
 * @param data     The image data.
 * @param passport     The email address of the sending user.
 */
void msn_handwritten_msg_show(MsnSwitchBoard *swboard, const char* msgid, const char* data, char* passport);

/**
 * Processes peer to peer messages.
 *
 * @param cmdproc The command processor.
 * @param msg     The message.
 */
void msn_p2p_msg(struct MsnCmdProc *cmdproc, struct MsnMessage *msg);

/**
 * Processes emoticon messages.
 *
 * @param cmdproc The command processor.
 * @param msg     The message.
 */
void msn_emoticon_msg(struct MsnCmdProc *cmdproc, struct MsnMessage *msg);

/**
 * Processes INVITE messages.
 *
 * @param cmdproc The command processor.
 * @param msg     The message.
 */
void msn_invite_msg(struct MsnCmdProc *cmdproc, struct MsnMessage *msg);

#endif /* MSN_SWITCHBOARD_H */
