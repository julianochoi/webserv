import subprocess
from pathlib import Path


def search_upwards_for_file(filename):
    """Search in the current directory and all directories above it
    for a file of a particular name.

    Args:
    ---
    @filename: string, the filename to look for.

    Returns
    -------
    pathlib.Path, the location of the first file found or
    None, if none was found
    """
    d = Path.cwd()
    root = Path(d.root)

    while d != root:
        attempt = d / filename
        if attempt.exists():
            return d
        d = d.parent

    return None


def run_server():
    makefile_path = search_upwards_for_file("Makefile")
    subprocess.run("make", check=True, cwd=makefile_path)

    webserv_path = search_upwards_for_file("webserv")
    p = subprocess.Popen(["./webserv", "default.conf"], cwd=webserv_path)
    p.kill()


if __name__ == "__main__":
    run_server()
