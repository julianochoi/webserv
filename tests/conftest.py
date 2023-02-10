import subprocess
from pathlib import Path

import pytest
from client import Client


def search_upwards_for_file(filename, dir_path: bool = False):
    """Search in the current directory and all directories above it
    for a file of a particular name.

    If `dir_path` returns the directory that the file is found.
    Otherwise returns path to file.
    """
    d = Path.cwd()
    root = Path(d.root)

    while d != root:
        attempt: Path = d / filename
        if attempt.exists():
            return d if dir_path else attempt
        d = d.parent

    return None


@pytest.fixture(scope="session", autouse=True)
def run_server():
    makefile_path = search_upwards_for_file("Makefile", dir_path=True)
    subprocess.run("make", check=True, cwd=makefile_path, capture_output=True)

    webserv_path = search_upwards_for_file("webserv", dir_path=True)
    with open("logs/stdout.txt", "a") as out, open("logs/stderr.txt", "a") as err:
        p = subprocess.Popen(["./webserv", "default.conf"], cwd=webserv_path, stdout=out, stderr=err)
    p.poll()
    if p.returncode:
        exit(p.returncode)
    yield None
    p.kill()


@pytest.fixture(scope="session")
def client():
    return Client()
