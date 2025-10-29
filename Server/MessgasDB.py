import sqlite3 as sql

def insert_message(conn, id_to, id_from, msg_type, content=b""):
    """

    """
    try:
        cur = conn.cursor()
        cur.execute(
            """
            INSERT INTO messages (ToClient, FromClient, Type, Content)
            VALUES (?, ?, ?, ?)
            """,
            (id_to, id_from, int(msg_type), content),
        )
        conn.commit()
        return cur.lastrowid
    except sql.Error as e:
        print(f"An error occurred while adding the message: {e}")
        return None


def fetch_messages_for_client(conn, client_id):
    """
    """
    try:
        cur = conn.cursor()
        cur.execute(
            """
            SELECT msg_id, ToClient, FromClient, Type, Content
            FROM messages
            WHERE ToClient = ?
            """,
            (client_id,),
        )
        return cur.fetchall()
    except sql.Error as e:
        print(f"An error occurred while fetching messages: {e}")
        return []


def delete_msg_for_client(conn, client_id):
    """
    """
    try:
        cur = conn.cursor()
        cur.execute("DELETE FROM messages WHERE ToClient = ?", (client_id,))
        conn.commit()
        return cur.rowcount > 0
    except sql.Error as e:
        print(f"An error occurred while deleting the message: {e}")
        return False


