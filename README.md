# webserv

This project is about writing our own HTTP server.

# Tests

#### Requirements

The tests for this project were developed using Python and have the following requirements:

* Python >= 3.8.1
* [Poetry](https://python-poetry.org/docs/ "Poetry is a tool for dependency management and packaging in Python.")
* [pytest](https://docs.pytest.org "pytest is a mature full-featured Python testing tool that helps you write better programs.")

## Installing test tools

In order to install the test requirements, we execute the following:

```bash
make test_install
```

This installs Poetry in the folder specified in the Makefile as `POETRY_HOME`. Default: `/opt/poetry`

It will also create a Python [virtual environment](https://docs.python.org/3.8/library/venv.html "Documentation for the Python venv module.") [[Note]](#virtual-environment)

> Note: It is also important to notice that these were originally designed to run an Ubuntu 20.04, so in order to use in other OS, some adaptation may be required.

## Running tests

In order to run the tests, we first need to configure our server settings at `./tests/config.py`

- `SERVER_ADDR`: The address where the server is hosted (localhost, 127.0.0.1, etc).
- `SERVER_PORT`: The port binding of the server to be tested.

Then, in order to run the tests, we execute:

```bash
make test
```

## Uninstall tests

If you would like to do a clean uninstall of the installed packages and tools required to run the test, simply execute:

```bash
make test_uninstall
```

## Notes

###### Virtual Environment

A virtual environment is useful because it isolates a Python binary and packages from the base installation of the system.

This allows us a better version control over the packages required to run a project.
Links:

- [Venv documentation](https://docs.python.org/3.8/library/venv.html "https://docs.python.org/3.8/library/venv.html")
- [Real Python guide](https://realpython.com/python-virtual-environments-a-primer/ "Python Virtual Environments: A Primer")
