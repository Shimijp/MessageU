import warnings
import socket
import selectors
from read_header import parse_header, HEADER_SIZE, MAX_PAYLOAD_SIZE
from ClientDB import create_db, search_client_in_db
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


def open_port(port):
    """
    Opens a TCP server socket on the specified port.

    Args:
        port (int): The port number to bind the server socket to.

    Returns:
        socket.socket: The opened server socket.
    """
    server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_sock.bind(('127.0.0.1', port))
    server_sock.listen()
    conn,  client_address = server_sock.accept()
    return conn, client_address
def handle_client(conn, client_address, db):
    """
    Handles communication with a connected client.

    Args:
        conn (socket.socket): The client connection socket.
        client_address (tuple): The client's address.
    """

    was_header = False
    buffer = bytearray()
    print(f"Connection from {client_address}")
    try:
        while True:
            data = conn.recv(4096)

            if not data:
                print(f"Client {client_address} disconnected")
                break

            buffer.extend(data)
            if  len(buffer) >= HEADER_SIZE and not was_header:

                header = parse_header(buffer[:HEADER_SIZE])
                if not header:
                    print(f"Invalid header from {client_address}, closing connection")
                    break

                payload_size = header['payload_size']
                if payload_size > MAX_PAYLOAD_SIZE:
                    print(f"Payload too large from {client_address}, closing connection")
                    break

                was_header = True
                print(f"Received header from {client_address}: {header}")
                client_id = header["client_id"]
                client_name = header["client_name"]
                public_key = search_client_in_db(db, client_id,client_name)
                print(public_key)
                buffer = buffer[HEADER_SIZE:]  # Remove header from buffer








    except Exception as e:
        print(f"Error handling client {client_address}: {e}")
    finally:
        conn.close()
if __name__ == '__main__':
    conn = create_db()
    server_port = get_server_info()
    net_conn, client_addr = open_port(server_port)
    handle_client(net_conn, client_addr, conn)
    conn.close()

