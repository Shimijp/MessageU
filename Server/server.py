import warnings
import socket


def get_server_info():
    try:
        with open('myport.info', 'r') as f:
            port = int(f.read())
    except FileNotFoundError:
        warnings.warn("no port file was found, switching to default port")
        port = 1357
    return port

def open_port(port):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('127.0.0.1', port))










