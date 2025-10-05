

import sqlite3 as sql



def create_db():
    conn = sql.connect("defensive.db")
    conn.text_factory = bytes
    try:
        conn.executescript("CREATE TABLE IF NOT EXISTS clients(ID  BLOB(16) PRIMARY KEY,"
                           "UserName  TEXT NOT NULL UNIQUE,"
                           "PublicKey BLOB(160) NOT NULL,"
                           "LastSeen INTEGER NOT NULL ")
    except sql.Error as e:
        print(f"An error occurred while creating the database: {e}")


    return conn


def search_client_in_db(client_id, name, conn):
    try:
        cursor = conn.cursor()
        cursor.execute("SELECT PublicKey from clients WHERE ID =? AND UserName =?", (client_id, name))
        row = cursor.fetchone()
        return row[0] if row else None
    except sql.error as e:
        print(f"could not fetch data: {e}")
        return None




