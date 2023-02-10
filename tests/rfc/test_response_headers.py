import pytest
import utils
from client import Client


@pytest.mark.rfc
class TestResponseHeaders:
    """
    rfc9110 section 5.6.7: Date/Time Formats

    When a sender generates a field that contains one or more timestamps defined as
    HTTP-date, the sender MUST generate those timestamps in the IMF-fixdate format.

    IMF-fixdate example:
    Sun, 06 Nov 1994 08:49:37 GMT
    """

    def test_date_format(self, client: Client):
        r = client.get()
        try:
            datestring = utils.get_from_dict("date", r.headers)
        except KeyError:
            return
        utils.parse_imf_date(datestring)

    def test_content_length(self, client: Client):
        headers = {"Host": client._server_addr}
        r = client.sock_get(headers=headers)
        try:
            content_length = int(utils.get_from_dict("content-length", r.headers))
        except KeyError:
            return
        raw = r.read()
        assert len(raw) == content_length
