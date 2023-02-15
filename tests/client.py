import socket
from http.client import HTTPResponse
from typing import Callable, Union

import config
from requests import PreparedRequest, Response, request


def req_test(f: Callable, headers: dict, *, status_code: int, **kwargs):
    """
    Use if `f` is a callable that uses the `requests` lib.
    """
    r = f(headers=headers, **kwargs)
    assert r.status_code == status_code


def socket_test(f: Callable, headers: str, *, status_code: int, **kwargs):
    """
    Use if `f` is a callable that uses the `request_by_socket` method.
    """
    r = f(headers=headers, **kwargs)
    assert r.status == status_code


# TODO adapt to multi-server
class Client:
    def __init__(self):
        self._server_addr = config.SERVER_ADDR
        self._server_port = config.SERVER_PORT
        self._debug = config.DEBUG

    @staticmethod
    def _debug_request(req: PreparedRequest):
        print("Request:")
        print(f"\tMethod: {req.method}")
        print(f"\tURL: {req.url}")
        print("\tHeaders:")
        for k, v in req.headers.items():
            print(f"\t\t{k}: {v}")

    @staticmethod
    def _debug_response(r: Union[Response, HTTPResponse]):
        print("Response:")
        print("\tHeaders:")
        for k, v in r.headers.items():
            print(f"\t\t{k}: {v}")
        try:
            print(f"\tStatus: {r.status_code} | Reason: {r.reason}")
            print(f"Elapsed: {r.elapsed.total_seconds()}s")
            print(f"\tEncoding: {r.encoding}")
            print(f"\tText: {r.text}")
        except AttributeError:
            print(f"\tStatus: {r.status} | Reason: {r.reason}")
            print(f"\tVersion: {r.version}")
            print(f"\tText: {r.read().decode(r.info().get_content_charset('utf8'))}")

    def _request(self, method: str, endpoint: str = "", **kwargs) -> Response:
        url = f"http://{self._server_addr}:{self._server_port}/{endpoint}"
        r = request(method=method, url=url, **kwargs)
        if self._debug:
            self._debug_request(r.request)
            self._debug_response(r)
        return request(method=method, url=url, **kwargs)

    def get(self, endpoint: str = "", **kwargs):
        return self._request(method="get", endpoint=endpoint, **kwargs)

    def post(self, endpoint: str = "", **kwargs):
        return self._request(method="post", endpoint=endpoint, **kwargs)

    def put(self, endpoint: str = "", **kwargs):
        return self._request(method="put", endpoint=endpoint, **kwargs)

    def delete(self, endpoint: str = "", **kwargs):
        return self._request(method="delete", endpoint=endpoint, **kwargs)

    def sock_get(self, headers: dict, **kwargs):
        return self._request_by_socket(method="GET", headers=headers, **kwargs)

    def sock_post(self, headers: dict, **kwargs):
        return self._request_by_socket(method="POST", headers=headers, **kwargs)

    def sock_put(self, headers: dict, **kwargs):
        return self._request_by_socket(method="PUT", headers=headers, **kwargs)

    def sock_delete(self, headers: dict, **kwargs):
        return self._request_by_socket(method="DELETE", headers=headers, **kwargs)

    @staticmethod
    def _header_to_str(
        headers: dict,
        method: str,
        target: str,
        protocol: str,
        version: str,
    ) -> str:
        request_line = f"{method} {target} {protocol}/{version}\r\n"
        header_lines = "\r\n".join([f"{k}: {v}" for k, v in headers.items()])
        return f"{request_line}{header_lines}\r\n\r\n"

    def _request_by_socket(
        self,
        headers: Union[str, dict],
        method: str = "GET",
        target: str = "/",
        protocol: str = "HTTP",
        version: str = "1.1",
        timeout: float = 15.0,
    ):
        if isinstance(headers, dict):
            req_header = self._header_to_str(headers, method, target, protocol, version)
        elif isinstance(headers, str):
            req_header = headers
        else:
            raise TypeError

        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(timeout)
        sock.connect((self._server_addr, self._server_port))
        sock.send(req_header.encode())

        http_response = HTTPResponse(sock)
        http_response.begin()

        if self._debug:
            print(f"{req_header = }")
            self._debug_response(http_response)

        return http_response
