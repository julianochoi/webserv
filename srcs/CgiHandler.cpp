#include "CgiHandler.hpp"

CgiHandler::CgiHandler() : _env(NULL), _argv(NULL) {}

CgiHandler::CgiHandler(CgiHandler const &cgi_handler) {
    this->_env_map = cgi_handler._env_map;
    this->_filepath = cgi_handler._filepath;
    this->_env = cgi_handler._env;
    this->_argv = cgi_handler._argv;
    this->_cgi = cgi_handler._cgi;
}

CgiHandler& CgiHandler::operator=(CgiHandler const &cgi_handler) {
    if(this != &cgi_handler)
    {
        this->_env_map = cgi_handler._env_map;
        this->_filepath = cgi_handler._filepath;
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

cgi_pair CgiHandler::_get_cgi() {
    cgi_pair pair("", "");

    if (_location.cgi_extension().size())
    {
        pair.first = _location.cgi_extension();
        if (_location.cgi_path().size())
            pair.second = _location.cgi_path();
    }
    else
    {
        pair.first = _server.cgi_extension();
        if (_server.cgi_path().size())
            pair.second = _server.cgi_path();
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


char** CgiHandler::_build_env(Request &request) {
    char **_envp;
    _add_header_to_env(request);
    _env_map["AUTH_TYPE"] = "";
    _env_map["GATEWAY_INTERFACE"] = "CGI/1.1";
    _env_map["REQUEST_METHOD"] = request.method();
    _env_map["SCRIPT_NAME"] = _filepath;
    _env_map["SERVER_NAME"] = _server.host();
    _env_map["SERVER_PORT"] = Utils::itoa(_server.port());
    _env_map["SERVER_PROTOCOL"] = "HTTP/1.1";
    _env_map["SERVER_SOFTWARE"] = "webserv";

    // TODO generate path info
    _env_map["PATH_INFO"] = "";  // The extra path information, as given in the requested URL.
    _env_map["PATH_TRANSLATED"] = "";  // The virtual-to-real mapped version of PATH_INFO.
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

    _argv[0] = new char[_cgi.second.length() + 1];
    strcpy(_argv[0], _cgi.second.c_str());

    _argv[1] = new char[_filepath.length() + 1];
    strcpy(_argv[1], _filepath.c_str());

    _argv[2] = NULL;
    return _argv;
}


void CgiHandler::build(Server server, ServerLocation location, Request request, std::string filepath){
    // TODO validate request method, if filepath exists
    this->_server = server;
    this->_location = location;
    this->_filepath = filepath;
    this->_cgi = _get_cgi();
    this->_argv = _build_argv();
    this->_env = _build_env(request);
}

std::string	CgiHandler::_get_cgi_output(std::FILE *tmp_file) {
    int size;
    char *tmp;
    std::string buffer;

    fseek(tmp_file, 0 , SEEK_END);
    size = ftell(tmp_file);
    rewind(tmp_file);

    tmp = new char[size + 1];
    fread(tmp, sizeof(char), size, tmp_file);

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

void CgiHandler::handle(int client_fd) {
    int	pid, status;
    std::string buffer;
	std::FILE *tmp_file = std::tmpfile();
	int tmp_fd = fileno(tmp_file);

	pid = fork();
	if (pid == 0)
    {
		dup2(tmp_fd, STDOUT_FILENO);
        execve(this->_argv[0], this->_argv, this->_env);
        close(tmp_fd);
    }
	waitpid(pid, &status, 0);
    // TODO do we need to check status?
    buffer = _get_cgi_output(tmp_file);
    _send_cgi_response(client_fd, buffer);
    fclose(tmp_file);

}
