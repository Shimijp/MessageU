
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
        import struct
        HEADER_FMT  = "<BHI"
        return struct.pack(HEADER_FMT,  self.version, self.code, self.payload_size)
