import struct
import sys

HEADER_FMT  = "<16sBHI"
HEADER_SIZE = struct.calcsize(HEADER_FMT)
MAX_PAYLOAD_SIZE = 1024 ** 2

def check_size(data):
    return len(data) >= HEADER_SIZE

def parse_header(data: bytes):
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
