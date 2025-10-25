import ClientDB
import sqlite3 as sql
def insert_message(db_conn, msg_id, id_to,id_from,msg_type, content =None):
    try:
        cursor = db_conn.cursor()
        cursor.execute("""
                INSERT INTO messages (ID, TO_CLIENT, FROM_CLIENT, TYPE, CONTENT)
                VALUES (?, ?, ?, ?, ?)
        """, (msg_id, id_to, id_from, msg_type, content))
        db_conn.commit()
        return True
    except sql.IntegrityError as e:
        print(f"Integrity error: {e}")
        return False
    except sql.Error as e:
        print(f"An error occurred while adding the message: {e}")
        return False

def fetch_messages_for_client(conn, client_id):
    try:
        cursor = conn.cursor()
        cursor.execute("SELECT ID, TO_CLIENT, FROM_CLIENT, TYPE, CONTENT FROM messages WHERE TO_CLIENT = ?", (client_id,))
        rows = cursor.fetchall()
        return rows
    except sql.Error as e:
        print(f"An error occurred while fetching messages: {e}")
        return []

def delete_msg_for_client(conn, client_id):
    try:
        cursor = conn.cursor()
        cursor.execute("DELETE FROM messages WHERE TO_CLIENT = ? ", (client_id,))
        conn.commit()
        return cursor.rowcount > 0
    except sql.Error as e:
        print(f"An error occurred while deleting the message: {e}")
        return False