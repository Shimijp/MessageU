import sys
import warnings
import socket
import selectors
from Requests import decode_header, HEADER_SIZE, MAX_PAYLOAD_SIZE
from ClientDB import create_db
from Requests import *
from Response import GeneralError
from requestHandler import handle_request
sel = selectors.DefaultSelector()


def get_server_info():
    """
    Reads the port number from 'myport.info'.
    Falls back to a default port if the file is not found or contains invalid data.

    Returns:
        int: The port number for the server.
    """
    try:
        with open('myport.info', 'r') as f:
            port = int(f.read())
            print(f"found port : {port}")
    except FileNotFoundError:
        warnings.warn("no port file was found, switching to default port")
        port = 1357
    except ValueError:
        warnings.warn("invalid file content, switching to default port")
        port = 1357
    return port



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



def handle_client(conn: socket.socket,db, addr):
    """
    Read from a single client until it closes the connection.
    Frames: [HEADER][PAYLOAD] ... repeated.
    """
    print(f"[conn] Connected: {addr}")
    buffer = bytearray()

    try:
        while True:
            chunk = conn.recv(4096)
            if not chunk:
                # EOF: client closed its write side
                print(f"[conn] Disconnected: {addr}")
                break

            buffer.extend(chunk)

            # Try to parse as many complete messages as exist in buffer
            while True:
                # Need at least a full header
                if len(buffer) < HEADER_SIZE:
                    break

                header = decode_header(buffer[:HEADER_SIZE])
                if not header:
                    print("[warn] Invalid header, sending error response")
                    conn.sendall(GeneralError().to_bytes())
                    buffer.clear()
                    continue

                payload_len = header["payload_size"]

                # Safety guard
                if payload_len > MAX_PAYLOAD_SIZE:
                    print(f"[error] payload_size={payload_len} exceeds MAX; closing")
                    conn.sendall(GeneralError().to_bytes())
                    buffer.clear()
                    continue

                total = HEADER_SIZE + payload_len
                if len(buffer) < total:
                    # Not yet a full frame; wait for more data
                    break

                # Slice out a single complete frame
                payload = bytes(buffer[HEADER_SIZE:total])
                code = header["code"]
                client_id = header["client_id"]
                # Consume this frame from the buffer
                del buffer[:total]

                # Process the message
                response = handle_request(db, payload, code, client_id)
                conn.sendall(response)
                buffer.clear()

    except ConnectionResetError:
        print(f"[conn] Reset by peer: {addr}")
    except Exception as e:
        print(f"[conn] Error {addr}: {e}")
    finally:
        try:
            conn.shutdown(socket.SHUT_RDWR)
        except Exception:
            pass
        conn.close()


def run_server(db, host="127.0.0.1", port=None, backlog=128):
    if port is None:
        port = get_server_port()

    # Create a single listening socket (do this once)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_sock:
        server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_sock.bind((host, port))
        server_sock.listen(backlog)
        print(f"[init] Listening on {host}:{port}")

        # Accept loop: handle one client at a time (synchronously)
        while True:
            try:
                client_sock, client_addr = server_sock.accept()
            except KeyboardInterrupt:
                print("\n[init] Shutting down server")
                break
            except Exception as e:
                print(f"[init] accept() error: {e}")
                continue

            # Handle this client to completion (blocking),
            # then return to accept() for the next client.
            handle_client(client_sock, db,client_addr)


if __name__ == "__main__":
    db  = create_db()
    run_server(db)
    db.close()


