from enum import Enum
import struct


from Requests import *


class ResponseCode(Enum):
    REG_SUCCESS = 2100
    USR_LST = 2101
    PUB_KEY = 2102
    SVD_MSG = 2103
    PULL_MSGS = 2104
    GEN_ERR = 9000

class Header:
    def __init__(self, version, code, payload_size):
        self.version = version
        self.code = code
        self.payload_size = payload_size
    def to_dict(self):
        return {
            "version": self.version,
            "code": self.code,
            "payload_size": self.payload_size
        }

    def to_bytes(self):
        return struct.pack(HEADER_FMT,  self.version, self.code, self.payload_size)


class Register(Header):
    def __init__(self, uuid):
        super().__init__(version = 2, code = ResponseCode.REG_SUCCESS.value, payload_size = struct.calcsize(REGISTER_FMT))
        self.uuid = uuid

    def to_bytes(self):
        header_bytes = super().to_bytes()
        payload_bytes = struct.pack(REGISTER_FMT, self.uuid)
        return header_bytes + payload_bytes
class UserList(Header):
    def __init__(self, users):
        self.users = users
        payload_size = sum([struct.calcsize("<16s255s") for _ in users])
        super().__init__(version = 2, code = ResponseCode.USR_LST.value, payload_size = payload_size)

    def to_bytes(self):
        header_bytes = super().to_bytes()
        payload_bytes = b''.join([struct.pack(CLIENT_LST_FMT, user_id, user_name.ljust(255, b'\x00')) for user_id, user_name in self.users])
        return header_bytes + payload_bytes
class PubKey(Header):
    def __init__(self, key):
        super().__init__(version = 2 , code = ResponseCode.PUB_KEY , payload_size = struct.calcsize(PUB_KEY_FMT))
        self.key = key
    def to_bytes(self):
        header_bytes = super().to_bytes()
        payload_bytes = struct.pack(PUB_KEY_FMT, self.key)
        return header_bytes+payload_bytes

class GeneralError(Header):
    def __init__(self):
        super().__init__(version = 2 , code = ResponseCode.GEN_ERR.value, payload_size = 0)
    def to_bytes(self):
        return super().to_bytes()

if __name__ == "__main__":
    # Example usage
    reg_response = Register(name=b"TestUser", uuid=b'\x01' * 16)
    print("Register Response Bytes:", reg_response.to_bytes())

    user_list_response = UserList(users=None)
    print("User List Response Bytes:", user_list_response.to_bytes())