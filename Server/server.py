import selectors
import socket
import warnings
from Requests import decode_header, HEADER_SIZE, MAX_PAYLOAD_SIZE
from Response import GeneralError
from requestHandler import handle_request
from ClientDB import create_db
sel = selectors.DefaultSelector()





def get_server_port():
    """Read port from myport.info or fall back to 1357."""
    try:
        with open("myport.info", "r") as f:
            port = int(f.read().strip())
            print(f"[init] Using port from file: {port}")
            return port
    except FileNotFoundError:
        warnings.warn("[init] myport.info not found, using default 1357")
    except ValueError:
        warnings.warn("[init] invalid myport.info, using default 1357")
    return 1357





# Create a selector to handle multiple sockets concurrently

def start_server(host="0.0.0.0", port=1234):
    """
    Create and start the server socket, register it with the selector.
    (Pass the port you want when calling this; same signature as your template)
    """
    server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_sock.bind((host, port))
    server_sock.listen()
    server_sock.setblocking(False)
    sel.register(server_sock, selectors.EVENT_READ, data=None)
    print(f"Listening on {host}:{port}")

def accept_wrapper(sock: socket.socket, db):
    """
    Accept a new client connection and register it with the selector.
    data holds per-connection state: addr, in_buf, out_buf, db.
    """
    client_sock, addr = sock.accept()
    print(f"Accepted connection from {addr}")
    client_sock.setblocking(False)
    state = {
        "addr": addr,
        "in_buf": bytearray(),
        "out_buf": bytearray(),
        "db": db,
    }
    sel.register(client_sock, selectors.EVENT_READ, data=state)

def _queue_write(sock: socket.socket, state: dict, data: bytes):
    if not data:
        return
    state["out_buf"] += data
    # ensure we're registered for WRITE if there's something to send
    sel.modify(sock, selectors.EVENT_READ | selectors.EVENT_WRITE, data=state)

def service_connection(key: selectors.SelectorKey, mask: int):
    """
    Handle communication with a client socket according to your framed protocol:
    [HEADER][PAYLOAD]...
    """
    sock: socket.socket = key.fileobj
    state: dict = key.data
    addr = state["addr"]
    in_buf = state["in_buf"]
    out_buf = state["out_buf"]
    db = state["db"]

    # READ
    if mask & selectors.EVENT_READ:
        try:
            chunk = sock.recv(4096)
        except ConnectionResetError:
            print(f"Client reset connection: {addr}")
            sel.unregister(sock)
            sock.close()
            return
        if not chunk:
            # client closed
            sel.unregister(sock)
            sock.close()
            print(f"Client closed connection: {addr}")
            return

        in_buf += chunk

        # Parse as many complete frames as possible
        while True:
            if len(in_buf) < HEADER_SIZE:
                break

            header = decode_header(in_buf[:HEADER_SIZE])
            if not header:
                # invalid header → send error and close
                try:
                    _queue_write(sock, state, GeneralError().to_bytes())
                except Exception:
                    pass
                in_buf.clear()
                # we will close after flushing any pending writes below
                break

            payload_len = header["payload_size"]
            if payload_len > MAX_PAYLOAD_SIZE:
                try:
                    _queue_write(sock, state, GeneralError().to_bytes())
                except Exception:
                    pass
                in_buf.clear()
                break

            total = HEADER_SIZE + payload_len
            if len(in_buf) < total:
                break

            payload = bytes(in_buf[HEADER_SIZE:total])
            code = header["code"]
            client_id = header["client_id"]
            print(f" Received request from {addr}: code={code}, payload_size={payload_len}")


            # consume exactly this frame
            del in_buf[:total]

            try:
                resp = handle_request(db, payload, code, client_id)
            except Exception as e:
                print(f"handle_request error from {addr}: {e}")
                resp = GeneralError().to_bytes()

            _queue_write(sock, state, resp)

    # WRITE (non-blocking; partial sends)
    if mask & selectors.EVENT_WRITE:
        if out_buf:
            try:
                sent = sock.send(out_buf)
            except (BlockingIOError, InterruptedError):
                return
            except BrokenPipeError:
                sel.unregister(sock)
                sock.close()
                return
            if sent > 0:
                del out_buf[:sent]
        # If nothing left to write, go back to READ only
        if not out_buf:
            sel.modify(sock, selectors.EVENT_READ, data=state)

def run():
    try:

        port = get_server_port()
    except Exception:
        port = 1357

    db = create_db()
    start_server("0.0.0.0", port)
    try:
        while True:
            events = sel.select(timeout=None)
            for key, mask in events:
                if key.data is None:
                    # Accept new client(s)
                    accept_wrapper(key.fileobj, db)
                else:
                    service_connection(key, mask)
    except KeyboardInterrupt:
        print("Interrupted by user, closing...")
    finally:
        sel.close()

if __name__ == "__main__":
    run()
