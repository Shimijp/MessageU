import struct
import sys
from enum import Enum


from ClientDB import find_client_by_name, add_client_to_db, return_all_clients, search_client_in_db, get_public_key
import uuid
HEADER_FMT  = "<16sBHI"
HEADER_SIZE = struct.calcsize(HEADER_FMT)
REGISTER_FMT = "<255s160s"
CLIENT_ID_FMT = "<16s"
CLIENT_LST_FMT = "<16s255s"
PUB_KEY_FMT = "<160s"
PUB_KEY_RSP_FMT = "<16s160s"
MAX_PAYLOAD_SIZE =  2 ** 32 #4GB
class ReqCodes(Enum):
    REG  = 600
    USR_LST = 601
    REQ_PUB_KEY = 602
    SEND_MSG = 603
    REQ_MSG = 604

def check_size(data):
    return len(data) >= HEADER_SIZE

def decode_header(data: bytes):
    if not check_size(data):
        return None

    try:
        client_id, version, code, payload_size = struct.unpack(HEADER_FMT, data[:HEADER_SIZE])
    except struct.error as e:
        print(f"Failed to unpack header: {e}", file = sys.stderr)
        return None
    return {
        "client_id": client_id,
        "version": version,
        "code": code,
        "payload_size": payload_size
    }

def decode_reg(data: bytes):
    if len(data) != struct.calcsize(REGISTER_FMT):
        print("Data too short for register payload", file = sys.stderr)
        return None
    try:
        name, public_key = struct.unpack(REGISTER_FMT, data)

    except struct.error as e:
        print(f"Failed to unpack register payload: {e}", file = sys.stderr)
        return None
    return name.strip(b'\x00'), public_key

def register_client(conn, data):
    name, key = decode_reg(data)
    if not name or not key:
        return 0
    client_id, public_key = find_client_by_name(conn, name)
    if client_id and public_key:
        print("Client already registered", file = sys.stderr)
        return 0
    else:
        client_id = bytes(uuid.uuid4().bytes)
        was_added = add_client_to_db(client_id, name, key, conn)
        if was_added:
            try:
                struct.pack(CLIENT_ID_FMT, client_id)
            except struct.error as e:
                print(f"Failed to pack client ID: {e}", file = sys.stderr)
                return 0
            return client_id
        return 0


def req_usr_lst(data: bytes, conn, client_id):
    if not search_client_in_db(client_id, conn):
        print(f"Client {client_id} not registered", file = sys.stderr)
        return None
    if len(data) > 0:
        print("Data too long", file = sys.stderr)
        return None
    clients = return_all_clients(conn, client_id)
    return clients


def request_public_key(conn, data, client_id):
    client_name = search_client_in_db(client_id, conn)
    print(f"client id: {client_id}")
    print(f"client name: {client_name}")
    if not client_name:
        print(f"not a registered client! {client_id}",  file= sys.stderr)
        return None
    try:
        req_id, = struct.unpack(CLIENT_ID_FMT, data)

        print(f"request key for: {req_id}")
        res_id, key = get_public_key(conn, req_id)
        if not res_id:
            print(f"no client with id {req_id} was found!", file=sys.stderr)
            return None
        return res_id, key
    except struct.error as e:
        print(f"unable to get key,  {e}",  file = sys.stderr)
        return None

