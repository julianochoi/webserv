import pytest
from client import Client

"""
rfc9110 section 6.2: Control Data

A server MUST NOT send a version to which it is not conformant. A server can send a 505
(HTTP Version Not Supported) response if it wishes, for any reason, to refuse service of
the client's major protocol version.
"""


@pytest.mark.rfc
def test_http_version(client: Client):
    headers = {"Host": client._server_addr}
    version = "0.1"
    r = client.sock_get(headers=headers, version=version)
    assert r.status == 505 or r.status // 100 == 4  # 4XX status code expected
