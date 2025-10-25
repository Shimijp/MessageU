import sys

from Response import *


def handle_request(conn, data, req_code, client_id):
    response = GeneralError()
    match req_code:
        case ReqCodes.REG.value:
            uuid_res = register_client(conn, data)
            if uuid_res != 0:
                response = Register(uuid_res)
        case ReqCodes.USR_LST.value:
            clients = req_usr_lst(data, conn, client_id)
            if clients:
                response = UserList(clients)
        case ReqCodes.REQ_PUB_KEY.value:
            req_id, key= request_public_key(conn, data, client_id)
            print(f"requested key: {key}")
            if key:
                try:
                    response = PubKey(req_id, key)
                except Exception as e:
                    print(f"Error creating PubKey response: {e}", file=sys.stderr)
        case _:
            print(f"no such code {req_code}!", file= sys.stderr)
    return response.to_bytes()










