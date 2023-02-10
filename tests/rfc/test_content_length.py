import pytest
from client import Client, req_test, socket_test


@pytest.mark.rfc
class TestContentLength:
    def test_neg_content_length(self, client: Client):
        headers = {"Content-Length": "-1"}
        req_test(client.get, headers, status_code=400)

    def test_big_content_length(self, client: Client):
        headers = {"Content-Length": f"{10**24}"}
        req_test(client.get, headers, status_code=400)

    def test_alpha_content_length(self, client: Client):
        headers = {"Content-Length": "ABCD"}
        req_test(client.get, headers, status_code=400)

    def test_duplicated_content_length(self, client: Client):
        host = f"Host: {client._server_addr}/1234\r\n"
        length_one = "Content-Length: 1\r\n"
        length_two = "Content-Length: 1\r\n"
        headers = f"GET / HTTP/1.1\r\n{host}{length_one}{length_two}\r\n"
        socket_test(client.sock_get, headers, status_code=400)
