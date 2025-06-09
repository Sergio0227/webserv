#include "ClientInfo.hpp"

void	BodyInfo::reset()
{
	body_size = 0;
	body_str.clear();
	email.clear();
	passw.clear();
}

void	RequestInfo::reset()
{
	request.clear();
	http_version.clear();
	path.clear();
	absolute_path.clear();
	query.clear();
	headers.clear();
	boundary.clear();
	body.reset();
}

void	ClientInfo::reset()
{
	status_code = 0;
	status_msg.clear();
	close_connection = false;
	dir_listening = false;
	run_cgi = false;
	keep_alive = false;
	file_uploaded = false;
	info.reset();
}