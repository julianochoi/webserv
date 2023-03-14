#include "CgiHandler.hpp"

CgiHandler::CgiHandler() : _env(NULL), _argv(NULL), _timeout(0) {}

CgiHandler::CgiHandler(size_t timeout) : _env(NULL), _argv(NULL), _timeout(timeout) {}

CgiHandler::CgiHandler(CgiHandler const &cgi_handler) {
    this->_env_map = cgi_handler._env_map;
    this->_filepath = cgi_handler._filepath;
    this->_full_path = cgi_handler._full_path;
    this->_env = cgi_handler._env;
    this->_argv = cgi_handler._argv;
    this->_cgi = cgi_handler._cgi;
}

CgiHandler& CgiHandler::operator=(CgiHandler const &cgi_handler) {
    if(this != &cgi_handler)
    {
        this->_env_map = cgi_handler._env_map;
        this->_filepath = cgi_handler._filepath;
        this->_full_path = cgi_handler._full_path;
        this->_env = cgi_handler._env;
        this->_argv = cgi_handler._argv;
        this->_cgi = cgi_handler._cgi;
    }
    return *this;
}

CgiHandler::~CgiHandler(void){
    int i = 0;
    if (this->_env)
    {
        while(this->_env[i])
            delete[] this->_env[i++];
        delete[] this->_env;
    }
    if (this->_argv)
    {
        for (int i = 0; i < 3; i++)
            delete[] _argv[i];
        delete[] _argv;
    }
}

std::string	CgiHandler::_get_default_cgi(std::string extension) {
    std::string temp;

    if (extension == ".py")
        return "/usr/bin/python3";
    else if (extension == ".php")
        return "/usr/bin/php-cgi";
    else
        throw UnsupportedCGI(extension);
}

cgi_pair CgiHandler::_get_cgi(Server &server, ServerLocation &location) {
    cgi_pair pair("", "");

    if (location.cgi_extension().size())
    {
        pair.first = location.cgi_extension();
        if (location.cgi_path().size())
            pair.second = location.cgi_path();
    }
    else
    {
        pair.first = server.cgi_extension();
        if (server.cgi_path().size())
            pair.second = server.cgi_path();
    }
    if (!pair.second.size())
        pair.second = _get_default_cgi(pair.first);

    return pair;
}

void CgiHandler::_add_header_to_env(Request &request) {
    std::string tmp_key;
    env_map header = request.headers();
    for(env_map::iterator it = header.begin(); it != header.end(); it++)
	{
        tmp_key = "http_" + it->first;
        std::replace(tmp_key.begin(), tmp_key.end(), '-', '_');
        std::transform(tmp_key.begin(), tmp_key.end(), tmp_key.begin(), toupper);
        _env_map[tmp_key] = it->second;
    }
}


char** CgiHandler::_build_env(Server &server, Request &request) {
    char **_envp;
    _add_header_to_env(request);
    _env_map["AUTH_TYPE"] = "";
    _env_map["GATEWAY_INTERFACE"] = "CGI/1.1";
    _env_map["REQUEST_METHOD"] = request.method();
    _env_map["SCRIPT_NAME"] = _filepath;
    _env_map["SERVER_NAME"] = server.host();
    _env_map["SERVER_PORT"] = Utils::itoa(server.port());
    _env_map["SERVER_PROTOCOL"] = "HTTP/1.1";
    _env_map["SERVER_SOFTWARE"] = "webserv";

    _env_map["PATH_INFO"] = request.path();
    _env_map["PATH_TRANSLATED"] = _full_path;
    // GET Specific
    if (request.method() == "GET" && request.query().size())
        _env_map["QUERY_STRING"] = request.query();
    // POST Specific
    if (request.method() == "POST")
    {
        if (request.body().length())
            _env_map["CONTENT_LENGTH"] = Utils::itoa(request.body().length());
        if (_env_map.find("HTTP_CONTENT_TYPE") != _env_map.end())
            _env_map["CONTENT_TYPE"] = _env_map["HTTP_CONTENT_TYPE"];
    }

    _envp = new char*[_env_map.size() + 1];
    int i = 0;
    std::string tmp;
    for(env_map::iterator it = _env_map.begin(); it != _env_map.end(); it++)
	{
        tmp = it->first + "=" + it->second;
        _envp[i] = new char[tmp.length() + 1];
        strcpy(_envp[i], tmp.c_str());
        i++;
    }
    _envp[_env_map.size()] = NULL;
    return _envp;
}

char** CgiHandler::_build_argv() {
    _argv = new char *[3];
    std::string filename;

    _argv[0] = new char[_cgi.second.length() + 1];
    strcpy(_argv[0], _cgi.second.c_str());

    filename = this->_filepath.substr(this->_filepath.rfind("/") + 1);
    _argv[1] = new char[filename.length() + 1];
    strcpy(_argv[1], filename.c_str());

    _argv[2] = NULL;
    return _argv;
}

void CgiHandler::build(Server server, ServerLocation location, Request request, std::string filepath, std::string full_path){
    this->_filepath = filepath;
    this->_full_path = full_path;
    this->_cgi = _get_cgi(server, location);
    this->_argv = _build_argv();
    this->_env = _build_env(server, request);
    if (!this->_timeout)
        this->_timeout = CGI_TIMEOUT;
}

std::string	CgiHandler::_get_cgi_output(std::FILE *tmp_out_file) {
    int size;
    char *tmp;
    std::string buffer;

    fseek(tmp_out_file, 0 , SEEK_END);
    size = ftell(tmp_out_file);
    rewind(tmp_out_file);

    tmp = new char[size + 1];
    memset(tmp, 0, size + 1);
    fread(tmp, sizeof(char), size, tmp_out_file);

    buffer = std::string(tmp);

    delete [] tmp;
    return buffer;
}

void CgiHandler::_send_cgi_response(int client_fd, std::string &buffer) {
    std::string status_line, response;

    status_line = "HTTP/1.1 200 OK\r\n";
    response = status_line + buffer;
    send(client_fd, response.c_str(), response.length(), 0);
    close(client_fd);
}

void CgiHandler::_chdir_wrapper(const char *path) {
    int status = chdir(path);
    if (status != 0)
        throw ChdirError();
}


int CgiHandler::_exec_cgi(int tmp_in_fd, int tmp_out_fd) {
    int status;
    int std_io_fds[2];
    std_io_fds[0] = dup(STDIN_FILENO);
    std_io_fds[1] = dup(STDOUT_FILENO);
    char buffer[4096];
    memset(buffer, 0, 4096);

    pid_t intermediate_pid = fork();
    if (intermediate_pid == 0) {
        pid_t cgi_pid = fork();
        if (cgi_pid == 0) {
            dup2(tmp_in_fd, STDIN_FILENO);
            dup2(tmp_out_fd, STDOUT_FILENO);
            close(tmp_in_fd);
            close(tmp_out_fd);
            execve(this->_argv[0], this->_argv, this->_env);
            _exit(0);
        }

        pid_t timer_pid = fork();
        if (timer_pid == 0) {
            sleep(this->_timeout);
            _exit(0);
        }

        pid_t exited_pid = wait(NULL);
        if (exited_pid == cgi_pid)
            kill(timer_pid, SIGKILL);
        else
            kill(cgi_pid, SIGKILL);

        wait(&status);
        if (WIFSIGNALED(status))
            _exit(WIFSIGNALED(status));
        _exit(0);
    }
    waitpid(intermediate_pid, &status, 0);
    dup2(STDIN_FILENO, std_io_fds[0]);
    dup2(STDOUT_FILENO, std_io_fds[1]);
    close(std_io_fds[0]);
    close(std_io_fds[1]);
    return status;
}


void CgiHandler::handle(int client_fd, Request &request) {
    int status;
    std::string buffer, dir;
    std::FILE *tmp_in_file = std::tmpfile();
	std::FILE *tmp_out_file = std::tmpfile();
	int tmp_in_fd = fileno(tmp_in_file);
    int tmp_out_fd = fileno(tmp_out_file);
    char *_pwd;

    _pwd = getcwd(NULL, 0);
    dir = this->_filepath.substr(0, this->_filepath.rfind("/"));
    _chdir_wrapper(dir.c_str());

    fputs(request.body().c_str(), tmp_in_file);
    rewind(tmp_in_file);

    status = _exec_cgi(tmp_in_fd, tmp_out_fd);

    _chdir_wrapper(_pwd);
    free(_pwd);
    if (WIFEXITED(status))
        throw CGIError();

    buffer = _get_cgi_output(tmp_out_file);
    _send_cgi_response(client_fd, buffer);
    fclose(tmp_out_file);
}
