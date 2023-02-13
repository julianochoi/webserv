import config
import pytest
from client import Client, req_test, socket_test


@pytest.mark.rfc
class TestRequestHeaders:
    def test_missing_header_field_name(self, client: Client):
        headers = {"": client._server_addr}
        r = client.sock_get(headers=headers)
        assert r.status // 100 == 4  # 4XX status code expected

    """
    rfc9110 section 5.1: Field Names

    Field names are case-insensitive and ought to be registered within the "Hypertext
    Transfer Protocol (HTTP) Field Name Registry."
    """

    def test_case_insensitive(self, client: Client):
        headers = {"hOsT": client._server_addr}
        socket_test(client.sock_get, headers, status_code=200)

    """
    rfc9112 section 3: Request Line

    A server that receives a request-target longer than any URI it wishes to parse MUST
    respond with a 414 (URI Too Long) status code.
    """

    def test_request_line_too_long(self, client: Client):
        headers = {"Host": client._server_addr}
        target = "A" * config.MAX_URI_LENGTH
        socket_test(client.sock_get, headers, status_code=414, target=target)

    """
    rfc9112 section 5.1: Field Line Parsing

    No whitespace is allowed between the field name and colon.
    A server MUST reject, with a response status code of 400 (Bad Request).
    """

    def test_space_between_header_name_and_colon(self, client: Client):
        headers = {"Host ": f"{client._server_addr}"}
        req_test(client.get, headers, status_code=400)
