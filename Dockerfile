FROM debian

RUN apt update && apt install -y \
                               gcc \
                               cmake \
                               bash \
                               cppcheck \
                               sudo \
                               curl \
                               python3-venv \
                               python3-pip \
                               valgrind \
                               siege

RUN mkdir /app

EXPOSE 3000-4000

RUN curl -sSL https://install.python-poetry.org | sudo POETRY_HOME=/opt/poetry python3 -

WORKDIR /app
COPY . /app

RUN python3 -m venv .venv
RUN /opt/poetry/bin/poetry install

ENTRYPOINT ["make", "run"]
# ENTRYPOINT ["sleep", "infinity"]
# RUN /opt/poetry/bin/poetry run pytest -> run at terminal after docker exec -it my-webserv-webserv-1 /bin/sh