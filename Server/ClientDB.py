

import sqlite3 as sql
import time


def create_db():
    conn = sql.connect("defensive.db")
    conn.text_factory = bytes
    try:
        conn.executescript("""
                CREATE TABLE IF NOT EXISTS clients (
                ID BLOB(16) PRIMARY KEY,
                UserName TEXT NOT NULL UNIQUE,
                PublicKey BLOB(160) NOT NULL,
                LastSeen INTEGER NOT NULL DEFAULT 0
            );
        """)
        conn.executescript("""
        CREATE TABLE IF NOT EXISTS messages (
        msg_id INTEGER PRIMARY KEY AUTOINCREMENT,
        ToClient BLOB(16),
        FromClient BLOB(16),
        Type INTEGER,
        Content BLOB
    );""")
    except sql.Error as e:
        print(f"An error occurred while creating the database: {e}")


    return conn
def add_client_to_db(client_id, name, public_key, conn):
    try:
        cursor = conn.cursor()
        cursor.execute("""
                INSERT INTO clients (ID, UserName, PublicKey, LastSeen)
                VALUES (?, ?, ?, ?)
        """, (client_id, name, public_key, int(time.time())))
        conn.commit()
        return True
    except sql.IntegrityError as e:
        print(f"Integrity error: {e}")
        return False
    except sql.Error as e:
        print(f"An error occurred while adding the client: {e}")
        return False

def search_client_in_db(client_id: object, conn: object) -> object:
    try:
        cursor = conn.cursor()
        cursor.execute("SELECT PublicKey  from clients WHERE ID =? ", (client_id, ))
        row = cursor.fetchall()
        return row[0] if row else None
    except sql.Error as e:
        print(f"could not fetch data: {e}")
        return None

def find_client_by_name(conn, name):
    try:
        cursor = conn.cursor()
        cursor.execute("SELECT ID, PublicKey FROM clients WHERE UserName = ?", (name, ))
        row = cursor.fetchone()
        return (row[0], row[1]) if row else (None, None)
    except sql.Error as e:
        print(f"An error occurred while searching for client by name: {e}")
        return None, None


def return_all_clients(conn , id):
    try:
        cursor = conn.cursor()
        cursor.execute("SELECT ID, UserName FROM clients WHERE ID != ?", (id,))
        rows = cursor.fetchall()
        return rows
    except sql.Error as e:
        print(f"An error occurred while fetching all clients: {e}")
        return []

def get_public_key(conn, client_id):
    try:
        cursor = conn.cursor()
        cursor.execute("SELECT ID, PublicKey FROM clients WHERE ID = ?", (client_id,))
        row = cursor.fetchall()
        return row[0] if row else None
    except sql.Error as e:
        print(f"An error occurred while fetching  client: {e}")
        return None