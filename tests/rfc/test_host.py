import pytest
from client import Client, req_test, socket_test


@pytest.mark.rfc
class TestHost:
    """
    rfc9112 section 3.2: Request Target

    A server MUST respond with a 400 (Bad Request) status code to any HTTP/1.1 request
    message that lacks a Host header field and to any request message that contains
    more than one Host header field line or a Host header field with an invalid field
    value.
    """

    """
		Subject says: The first server for a host:port will be the default for this host:port (that means
		it will answer to all the requests that don’t belong to an other server).

		Because of that we wont validate this test
    """

    @pytest.mark.skip
    def test_header_with_no_host(self, client: Client):
        headers = {}
        socket_test(client.sock_get, headers, status_code=400)

    def test_header_with_two_hosts(self, client: Client):
        host = f"Host: {client._server_addr}\r\n"
        headers = f"GET / HTTP/1.1\r\n{host}{host}\r\n"
        socket_test(client.sock_get, headers, status_code=400)

    """
		Subject says: The first server for a host:port will be the default for this host:port (that means
		it will answer to all the requests that don’t belong to an other server).

		Because of that we wont validate this test
    """

    @pytest.mark.skip
    def test_header_with_invalid_host(self, client: Client):
        headers = {"Host": "johndoe@email.com"}
        req_test(client.get, headers, status_code=400)
