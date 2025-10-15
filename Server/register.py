from ClientDB import find_client_by_name, add_client_to_db
import uuid
import struct
HEADER_FMT  = "<255s160s"
def parse_request(data):
    try:
        name, public_key = struct.unpack(HEADER_FMT, data)
        return name, public_key
    except struct.error as e:
        print(f"Failed to unpack request: {e}")
        return None, None


def register_client(conn, name, pblc_key):
    client_id, public_key = find_client_by_name(conn, name)
    if client_id and public_key:
        return 0
    else:
        client_id = uuid.uuid4().bytes


        was_added = add_client_to_db(client_id, name, pblc_key, conn)
        if was_added:
            return client_id.bytes
        return 0




