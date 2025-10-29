import struct
import sys
from enum import Enum


from ClientDB import find_client_by_name, add_client_to_db, return_all_clients, search_client_in_db, get_public_key
import uuid

from MessgasDB import insert_message, fetch_messages_for_client

HEADER_FMT  = "<16sBHI"
HEADER_SIZE = struct.calcsize(HEADER_FMT)
REGISTER_FMT = "<255s160s"
CLIENT_ID_FMT = "<16s"
CLIENT_LST_FMT = "<16s255s"
PUB_KEY_FMT = "<160s"
PUB_KEY_RSP_FMT = "<16s160s"
MSG_HEADER_FMT = "<16sBI"
MSG_ITEM_FMT = "<16sIBI"
MAX_PAYLOAD_SIZE =  2 ** 32 #4GB
class ReqCodes(Enum):
    REG  = 600
    USR_LST = 601
    REQ_PUB_KEY = 602
    SEND_MSG = 603
    REQ_MSG = 604


class MsgCodes(Enum):
    REQ_SYM_KEY = 1
    SEND_SYM_KEY = 2
    SEND_TEXT_MSG = 3


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
        return None, None, None

def decode_msg_header(data: bytes):
    try:
        d_id, msg_code, msg_size = struct.unpack(MSG_HEADER_FMT, data[:struct.calcsize(MSG_HEADER_FMT)])
        print(f"Decoded message header: d_id={d_id}, msg_code={msg_code}, msg_size={msg_size}")
        return d_id, msg_code, msg_size

    except struct.error as e:
        print(f"Failed to unpack message header: {e}", file = sys.stderr)
        return None, None, None
def decode_msg_req(data: bytes):
    d_id, msg_code, msg_size = decode_msg_header(data)
    if not d_id or not msg_code or not msg_size:
        return None, None, None, None
    print(f"Decoded message header: d_id={d_id}, msg_code={msg_code}, msg_size={msg_size}")
    if msg_size > 0:
        msg_payload = data[struct.calcsize(MSG_HEADER_FMT):struct.calcsize(MSG_HEADER_FMT)+msg_size]
        if len(msg_payload) != msg_size:
            print("Payload size mismatch", file = sys.stderr)
            return None
    else:
        msg_payload = b''
    return d_id, msg_code, msg_size, msg_payload
def handle_message_request(conn, data, client_id):
    d_id, msg_code, msg_size, msg_payload = decode_msg_req(data)
    if not d_id or not msg_code or not msg_size or not msg_payload:
        return None, None
    # Further processing based on msg_code can be implemented here
    if msg_code == MsgCodes.REQ_SYM_KEY.value:
        # Handle symmetric key request
        pass
    elif msg_code == MsgCodes.SEND_SYM_KEY.value:
        # Handle sending symmetric key
        pass
    elif msg_code == MsgCodes.SEND_TEXT_MSG.value:
        msg_id = insert_message(conn, d_id, client_id, msg_code, msg_payload)
        if msg_id:
            print(f"Message {msg_id} saved for client {d_id}")
            return msg_id,  d_id
def convert_msg_to_bytes(msg_id, to_id, from_id, msg_type, content):
    try:

        size = len(content)
        if len(from_id) != 16:
            print("FromClient must be 16 bytes", file=sys.stderr)
            return None

        msg_header = struct.pack(f"{MSG_ITEM_FMT}{size}s", from_id, msg_id,msg_type, size, content)
        return msg_header
    except struct.error as e:
        print(f"Failed to pack message to bytes: {e}", file=sys.stderr)
        return None
def get_all_msgs(conn, client_id):
    acc_bytes = bytearray()
    msgs = fetch_messages_for_client(conn,  client_id)
    for msg in msgs:
        msg_id, to_client, from_client, msg_type, content = msg
        print(f"Fetched message: msg_id={msg_id}, to_client={to_client}, from_client={from_client}, msg_type={msg_type}, content_size={len(content)}")
        msg_bytes = convert_msg_to_bytes(msg_id, to_client, from_client, msg_type, content)
        if msg_bytes:
            acc_bytes += msg_bytes
    return bytes(acc_bytes) , len(acc_bytes)
