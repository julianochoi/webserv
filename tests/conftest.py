import pytest
from client import Client


@pytest.fixture(scope="session")
def client():
    return Client()
