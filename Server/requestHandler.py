import sys
from http.client import responses

from Requests import *
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
            req_key = request_public_key(conn, data, client_id)
            if req_key:
                response = PubKey(req_key)
        case _:
            print(f"no such code {req_code}!", file= sys.stderr)
    return response.to_bytes()










